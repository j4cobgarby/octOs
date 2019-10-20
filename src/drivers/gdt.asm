; I have two segments, gdt_code and gdt_data
; they're both kernel-only segments, so they can't be accessed from userspace code
; for security reasons.
; this is because i've set them to be only accessible by ring 0 code, which is the
; kernel.
gdt_start:
gdt_null:
    dq 0 ; 8 bytes of nothing
gdt_code:           ; == KERNEL CODE ==
    dw 0xffff       ; bits 0-15 of the length of the code segment
    dw 0x0000       ; bits 0-15 of the base of the code segment
    db 0x00         ; bits 16-23 of the base of the code segment
    db 0x9a         ; the access byte. this says that it can only be accessed by the kernel (ring 0)
    db 11001111b    ; bits 16-19 of the length of the segment, also flags saying 32 bit prot., and 4k blocks
    db 0x00         ; bits 24-31 of the base
gdt_data:           ; == KERNEL DATA ==
    dw 0xffff       ; bits 0-15 of length
    dw 0x0000       ; bits 0-15 of base
    db 0x00         ; bits 16-23 of base
    db 0x92         ; access byte. non executable, only accessible by kernel (ring 0)
    db 11001111b    ; bits 16-19 of length, and also flags
    db 0x00         ; bits 24-31 of base
gdt_userland_code:  ; == USER CODE ==
    dw 0xffff       ; 0-15 of length
    dw 0x0000       ; 0-15 of base
    db 0x00         ; 16-23 of base
    db 11111010b    ; this says it's ring 3, code, only accessed by ring 3
    db 11001111b    ; 16-19 of length, also 32 bit and 4k granularity
    db 0x00         ; 24-31 of base
gdt_userland_data:  ; == USER DATA ==
    dw 0xffff
    dw 0x0000
    db 0x00
    db 11110010b    ; the only difference between this and the user code, is the 5th bit saying it's data
    db 11001111b
    db 0x00
gdt_tss_descriptor: ; == TASK STATE SEGMENT ==
gdt_length equ $ - gdt_start - 1
    dw 0x0000;      ; limit low, to be put in later
    dw 0x0000;      ; base 0-15, put in later
    db 0x00;        ; base 16-23
    db 11100010b    ; access info
    db 0x0000       ; flags, also also 16-19 of limit
    db 0x00         ; 24-31 of base

gdt_descriptor  dw gdt_length   ; the length of the gdt
                dd gdt_start   ; the address of the gdt (both to be loaded in from C)

; This procedure should be called after the lgdt
; instruction has been called.
; It sets the various segment registers to their
; new values.
refresh_segments:
    jmp 0x08:refresh_segment_registers
refresh_segment_registers:
    mov ax, 0x10    ; 0x10 is the data selector in the GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret