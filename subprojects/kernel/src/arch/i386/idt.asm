PIC_MASTER_OFFSET   equ 0x20
PIC_SLAVE_OFFSET    equ 0x30

idt_start:  ; each idt entry is 8 bytes long
    dq 0    ; div by 0
    dq 0    
    dq 0    ; nmi
    dq 0
    dq 0    ;overflow
    dq 0
    dq 0
    dq 0
    dq 0    ; double fault
    dq 0    
    dq 0
    dq 0
    dq 0
    dq 0    ; #GP
    dq 0    ; #PF
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
    dq 0
idt_pic_entries:
idt_pit:
    dw 0x0000
    dw 0x0008
    db 0x00
    db 10001110b
    dw 0x0000
idt_keyboard:
    dw 0x0000       ; bits 0-15 of offset
    dw 0x0008       ; code segment selector 
    db 0x00         ; unused
    db 10001110b    ; type
    dw 0x0000       ; bits 16-31 of offset
    times (0x80 - PIC_MASTER_OFFSET - 2) dq 0
idt_syscall:
    dw 0x0000       ; bits 0-15 of offset
    dw 0x0008       ; segment selector
    db 0x00         ; unused
    db 11101110b    ; type
    dw 0x0000       ; offset 16-31

idt_length equ $ - idt_start

idt_descriptor  dw idt_length
                dd idt_start