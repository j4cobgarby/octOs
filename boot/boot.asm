MB_FLAGS        equ 1<<0 | 1<<1 | 1<<2 ; 0x1: all boot modules aligned on 4KB boundaries, even 
                                        ;   though i don't think i'll need to use this
                                        ; 0x10: memory information available
                                        ; 0x100: means i can set parameters about the video output
MB_MAGICNUMBER  equ 0x1BADB002
MB_CHKSUM       equ -(MB_FLAGS + MB_MAGICNUMBER)
MB_MODETYPE     equ 1 ; textmode
MB_WIDTH        equ 80
MB_HEIGHT       equ 24

MB_DEPTH        equ 0

PIC1            equ 0x20    ; IO base address for master 8259 PIC
PIC2            equ 0xa0    ; addr for slave PIC
PIC1_CMD        equ PIC1
PIC1_DAT        equ PIC1 + 1
PIC2_CMD        equ PIC2
PIC2_DAT        equ PIC2 + 1

ICW1        equ 00010001b   ; ICW4 needed
ICW2_MASTER equ 0x60   ; 0x60
ICW2_SLAVE  equ 0x70   ; 0x70
ICW3_MASTER equ 00000100b
ICW3_SLAVE  equ 00000010b
ICW4        equ 00000001b   ; 8086/88 mode

section .multiboot_header:
align 4
multiboot_start:
    dd MB_MAGICNUMBER
    dd MB_FLAGS
    dd MB_CHKSUM
    dd 0,0,0,0,0 ; 5 * 4 zero bytes in a row, because i'm not using flag 16
    dd MB_MODETYPE
    dd MB_WIDTH
    dd MB_HEIGHT
    dd MB_DEPTH
multiboot_length equ $ - multiboot_start

section .bss
align 16
;; the stack is defined here.
;; defined in .bss to save kernel space, since it'll just say
;; "16384 uninitialised bytes here", rather than actually list
;; that many bytes out in the object file!
stack_bottom:
    resb 16384
stack_top:

section .isr    ; section for ISRs


section .text   ; now the actual kernel entry point is in this section
global _start:function (_start.end - _start)    ; make the object file store the length of the _start symbol

; I have two segments, gdt_code and gdt_data
; they're both kernel-only segments, so they can't be accessed from userspace code
; for security reasons.
; this is because i've set them to be only accessible by ring 0 code, which is the
; kernel.
gdt_start:
gdt_null:
    dq 0 ; 8 bytes of nothing
gdt_code:
    dw 0xffff       ; bits 0-15 of the length of the code segment
    dw 0x0000       ; bits 0-15 of the base of the code segment
    db 0x00         ; bits 16-23 of the base of the code segment
    db 0x9a         ; the access byte. this says that it can only be accessed by the kernel (ring 0)
    db 11001111b    ; bits 16-19 of the length of the segment, also flags saying 32 bit prot., and 4k blocks
    db 0x00         ; bits 24-31 of the base
gdt_data:
    dw 0xffff       ; bits 0-15 of length
    dw 0x0000       ; bits 0-15 of base
    db 0x00         ; bits 16-23 of base
    db 0x92         ; access byte. non executable, only accessible by kernel (ring 0)
    db 11001111b    ; bits 16-19 of length, and also flags
    db 0x00         ; bits 24-31 of base
gdt_length equ $ - gdt_start - 1

gdt_descriptor  dw gdt_length   ; the length of the gdt
                dd gdt_start   ; the address of the gdt (both to be loaded in from C)

idt_start:  ; each idt entry is 8 bytes long
    resd ICW2_MASTER * 2       ; pad with doublewords to get up to PIC entries
    dq 0        ; PIT timer 
idt_keyboard:
    dw 0x0000       ; bits 0-15 of offset
    dw 0x0008       ; code segment selector 
    db 0x00         ; unused
    db 10001110b    ; type
    dw 0x0000       ; bits 16-31 of offset

idt_length equ $ - idt_start

idt_descriptor  dw idt_length
                dd idt_start

pic_init:
    ; https://www.eeeguide.com/programming-8259/
    cli

    ; I will map the master PIC to 0x60+0:7 = 0110,0000b
    ; and the slave PIC to 0x70+0:7 = 0111,0000b (which is actually the default for it anyway)

    mov al, ICW1
    out PIC1_CMD, al
    out PIC2_CMD, al

    mov al, ICW2_MASTER
    out PIC1_DAT, al

    mov al, ICW2_SLAVE
    out PIC2_DAT, al

    mov al, ICW3_MASTER
    out PIC1_DAT, al

    mov al, ICW3_SLAVE
    out PIC2_DAT, al

    mov al, ICW4
    out PIC1_DAT, al
    out PIC2_DAT, al

    mov al, 0xfd        ; set masks to 0
    out PIC1_DAT, al
    mov al, 0xff
    out PIC2_DAT, al

    mov al, 0xae
    out 0x64, al

    ; Now both of the PICs are initialised correctly
    sti
    ret

isr_keyboard:
    pusha
    cli

    in al, 0x60

    mov al, 0x20    ; EOI
    out ICW2_MASTER, al

    mov eax, esp
    add eax, 4
    popa

    sti
    xchg bx, bx
    iret

_start: ; the actual entry point to the kernel!
    mov esp, stack_top  ; setup the stack, because obviously it's nice to have a stack, and also
                        ; C programs need a stack to run

    cli
    lgdt [gdt_descriptor]
    call .refresh_segments

    ; put addresses in the IDT
    mov eax, isr_keyboard   ; eax = 0xUUUULLLL, U=upper L=lower
    mov word [idt_keyboard], ax ; put the lower 16 bits into the IDT
    shr eax, 16 ; set ax to the upper bits of eax
    mov word [idt_keyboard + 6], ax ; put the upper 16 bits into the IDT with an offset

    lidt [idt_descriptor]

    call pic_init


    ;extern kernel_main  ; kernel_main is defined in a C file
    ;call kernel_main    ; run the main kernel procedure

    ; only gets here if the kernel returns, which would be weird, but if it does happen
    ; then just halt the CPU
    ; if a NMI wakes it up, halt it again
.hltloop:
    jmp .hltloop

.refresh_segments:
    jmp 0x08:.refresh_segment_registers
.refresh_segment_registers:
    mov ax, 0x10    ; 0x10 is the data selector in the GDT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

.end: ; marks the end of the _start symbol's code
