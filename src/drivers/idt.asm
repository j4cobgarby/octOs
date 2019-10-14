idt_start:  ; each idt entry is 8 bytes long
    resd PIC_MASTER_OFFSET * 2       ; pad with doublewords to get up to PIC entries
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