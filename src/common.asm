PIC1            equ 0x20    ; IO base address for master 8259 PIC
PIC2            equ 0xa0    ; addr for slave PIC
PIC1_CMD        equ PIC1
PIC1_DAT        equ PIC1 + 1
PIC2_CMD        equ PIC2
PIC2_DAT        equ PIC2 + 1

PIC_EOI         equ 0x20    ; byte to send to PIC CMD port to acknowledge end of interrupt

PIC_MASTER_OFFSET       equ 0x20
PIC_SLAVE_OFFSET        equ 0x30