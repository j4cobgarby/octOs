PIC1            equ 0x20    ; IO base address for master 8259 PIC
PIC2            equ 0xa0    ; addr for slave PIC
PIC1_CMD        equ PIC1
PIC1_DAT        equ PIC1 + 1
PIC2_CMD        equ PIC2
PIC2_DAT        equ PIC2 + 1

PIC_EOI         equ 0x20    ; byte to send to PIC CMD port to acknowledge end of interrupt

PIC_MASTER_OFFSET   equ 0x20
PIC_SLAVE_OFFSET    equ 0x30

SYSCALL_OFSET   equ 0x80

SCREEN_COLOUR   equ 0x31
SCREEN_MIN  equ 0xb8000
SCREEN_MAX  equ 0xb8fa0
SCREEN_SIZE equ SCREEN_MAX - SCREEN_MIN
SCREEN_ROWS equ 25
SCREEN_COLS equ 80