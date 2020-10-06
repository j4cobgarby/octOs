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
    dw 0x0000;      ; limit low, to be put in later
    dw 0x0000;      ; base 0-15, put in later
    db 0x00;        ; base 16-23
    db 11101001b    ; access info
    db 0x00       ; flags, also also 16-19 of limit
    db 0x00         ; 24-31 of base
gdt_length equ $ - gdt_start - 1
gdt_descriptor  dw gdt_length   ; the length of the gdt
                dd gdt_start   ; the address of the gdt (both to be loaded in from C)

%include "src/arch/i386/tss.asm"

fill_tss_descriptor:
    mov dword [tss_entry + 4], stack_top  ; whenever ring 0 is entered (for a syscall or whatever), the handler needs a stack
        ; it'll basiclaly begin a new stack from the top of the stack buffer, since there's no need to make it keep the
        ; stack the same between syscalls

    mov word [gdt_tss_descriptor],       tss_limit & 0xffff          ; isolate the first 2 bytes of the limit
    mov word [gdt_tss_descriptor + 2],   (0x100000 + tss_entry - $$) & 0xffff          ; first 2 bytes of base of tss entry
    mov byte [gdt_tss_descriptor + 4],   ((0x100000 + tss_entry - $$) >> 16) & 0xff    ; next byte of the base
    mov byte [gdt_tss_descriptor + 6],   (tss_limit >> 16) & 0xf     ; next byte of limit
    mov byte [gdt_tss_descriptor + 7],   ((0x100000 + tss_entry - $$) >> 24) & 0xff     ; last byte of base
    ret

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