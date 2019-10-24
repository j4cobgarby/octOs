idt_start:  ; each idt entry is 8 bytes long
    times PIC_MASTER_OFFSET dq 0       ; pad with doublewords to get up to PIC entries
idt_pic_entries:
    dq 0        ; Timer 
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