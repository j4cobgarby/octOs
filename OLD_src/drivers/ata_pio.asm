ATA_BUS1_BASE       equ 0x1f0
ATA_BUS2_BASE       equ 0x170
ATA_BUS1_CTRL       equ 0x3f6
ATA_BUS2_CTRL       equ 0x376


ATA_DATA            equ 0
ATA_ERR             equ 1
ATA_FEATURES        equ 1
ATA_SCOUNT          equ 2
ATA_LBALOW          equ 3
ATA_LBAMID          equ 4
ATA_LBAHI           equ 5
ATA_DRIVEHEAD       equ 6
ATA_STATUS          equ 7
ATA_COMMAND         equ 7

ATA_BUS1_DATA       equ ATA_BUS1_BASE + ATA_DATA
ATA_BUS1_ERR        equ ATA_BUS1_BASE + ATA_ERR
ATA_BUS1_FEATURES   equ ATA_BUS1_BASE + ATA_FEATURES
ATA_BUS1_SCOUNT     equ ATA_BUS1_BASE + ATA_SCOUNT
ATA_BUS1_LBALOW     equ ATA_BUS1_BASE + ATA_LBALOW
ATA_BUS1_LBAMID     equ ATA_BUS1_BASE + ATA_LBAMID
ATA_BUS1_LBAHI      equ ATA_BUS1_BASE + ATA_LBAHI
ATA_BUS1_DRIVEHEAD  equ ATA_BUS1_BASE + ATA_DRIVEHEAD
ATA_BUS1_STATUS     equ ATA_BUS1_BASE + ATA_STATUS
ATA_BUS1_COMMAND    equ ATA_BUS1_BASE + ATA_COMMAND

ATA_BUS2_DATA       equ ATA_BUS2_BASE + ATA_DATA
ATA_BUS2_ERR        equ ATA_BUS2_BASE + ATA_ERR
ATA_BUS2_FEATURES   equ ATA_BUS2_BASE + ATA_FEATURES
ATA_BUS2_SCOUNT     equ ATA_BUS2_BASE + ATA_SCOUNT
ATA_BUS2_LBALOW     equ ATA_BUS2_BASE + ATA_LBALOW
ATA_BUS2_LBAMID     equ ATA_BUS2_BASE + ATA_LBAMID
ATA_BUS2_LBAHI      equ ATA_BUS2_BASE + ATA_LBAHI
ATA_BUS2_DRIVEHEAD  equ ATA_BUS2_BASE + ATA_DRIVEHEAD
ATA_BUS2_STATUS     equ ATA_BUS2_BASE + ATA_STATUS
ATA_BUS2_COMMAND    equ ATA_BUS2_BASE + ATA_COMMAND

ATA_BUS1_ALTSTATUS  equ ATA_BUS1_CTRL + 0
ATA_BUS1_DEVCTRL    equ ATA_BUS1_CTRL + 0
ATA_BUS1_DRIVEADDR  equ ATA_BUS1_CTRL + 1

ATA_BUS2_ALTSTATUS  equ ATA_BUS2_CTRL + 0
ATA_BUS2_DEVCTRL    equ ATA_BUS2_CTRL + 0
ATA_BUS2_DRIVEADDR  equ ATA_BUS2_CTRL + 1

ATA_TIMEOUT         equ 1000

ata_pio_detect:
    ; first check if any of the busses are floating (0xff)
    ; for any non-floating bus, use the IDENTIFY command for
    ; both master and slave

    mov dx, ATA_BUS1_STATUS
    in al, dx
    cmp al, 0xff
    jne .bus1_notff
.bus1_isff:
    and byte [ATA_BUS1_MASTER_STATUS], 11111110b ; the whole bus doesn't exist. this
    and byte [ATA_BUS1_SLAVE_STATUS],  11111110b ; is an error
    mov eax, 0
    mov ebx, bus1_nodrivesfound_str
    int 0x80
    jmp $ ; just wait here for now
    jmp .endffcheck
.bus1_notff:
    mov eax, 0
    mov ebx, bus1_drivepossible_str
    int 0x80
.endffcheck:
    ; now need to use the IDENTIFY command
    mov dx, ATA_BUS1_DRIVEHEAD
    mov ax, 0xa0 ; select master
    call select_drive
    mov ecx, ATA_BUS1_MASTER_STATUS
    xchg bx, bx
    mov dx, ATA_BUS1_BASE
    call identify_drive
    mov al, [ATA_BUS1_MASTER_STATUS]
    bt ax, 0
    jc .bus1_master_identified
.bus1_master_not_ident:
    mov eax, 0
    mov ebx, bus1_master_not_exists_str
    int 0x80
    jmp .bus1_master_check_end
.bus1_master_identified:
    mov eax, 0
    mov ebx, bus1_master_exists_str
    int 0x80
.bus1_master_check_end:

    mov dx, ATA_BUS1_DRIVEHEAD
    mov ax, 0xb0
    call select_drive
    mov ecx, ATA_BUS1_SLAVE_STATUS
    mov dx, ATA_BUS1_BASE
    call identify_drive
    mov al, [ATA_BUS1_SLAVE_STATUS]
    bt ax, 0
    jc .bus1_slave_identified
.bus1_slave_not_ident:
    mov eax, 0
    mov ebx, bus1_slave_not_exists_str
    int 0x80
    jmp .bus1_slave_check_end
.bus1_slave_identified:
    mov eax, 0
    mov ebx, bus1_slave_exists_str
    int 0x80
.bus1_slave_check_end:
    ret

; dx <- ATA BUS base IO addr
identify_drive:
    mov al, 0x00
    add dx, ATA_SCOUNT
    out dx, al
    add dx, ATA_LBALOW-ATA_SCOUNT
    out dx, al
    add dx, ATA_LBAMID-ATA_LBALOW
    out dx, al
    add dx, ATA_LBAHI-ATA_LBAMID
    out dx, al
    mov al, 0xec ; ec is the identify command
    add dx, ATA_COMMAND-ATA_LBAHI
    out dx, al

    ;add dx, ATA_STATUS-ATA_COMMAND ; status is actually same port as command
    in al, dx ; read status

    cmp al, 0
    je .master_notexists
.master_exists:
    or byte [ecx], 0x1 ; set first bit of status
    jmp .mastercheck_end
.master_notexists:
    and byte [ecx], 0x0 ; clear first bit of status
.mastercheck_end:
    ret

ata_pio_read48:
ata_pio_write48:

select_drive: ; selects a drive and also waits for bsy and drq to clear
    pusha
    ; dx = the IO addr of the drive register
    ; ax = the drive (0xa0 for master, 0xb0 for slave)

    out dx, al

    inc dx ; set dx to the status register
    times 4 in al, dx ; ax = the status register

    xor ecx, ecx
.hang:
    inc ecx
    cmp ecx, ATA_TIMEOUT
    je .skip
    in al, dx
    bt ax, 7
    jc .hang ; wait until bsy is cleared
    bt ax, 3
    jc .hang ; and also until drq is cleared
.skip:
    popa
    ret

ATA_BUS1_MASTER_STATUS db 00000000b
ATA_BUS1_SLAVE_STATUS db 00000000b
; ^^
; bit 0: drive exists
; bit 1: supports lba48 mode

ATA_BUS1_MASTER_LBA28_SECTORS   dd 0
ATA_BUS1_MASTER_LBA48_SECTORS   dq 0
ATA_BUS1_SLAVE_LBA28_SECTORS    dd 0
ATA_BUS1_SLAVE_LBA48_SECTORS    dq 0

ata_identify_buffer:
    times 512 db 0 ; 256 16-bit values for returned data from IDENTIFY
bus1_nodrivesfound_str      db "No drive found on bus 1.",10,0
bus1_drivepossible_str      db "Drive(s) possible on bus 1.",10,0
bus1_master_exists_str      db "Master drive exists on ATA bus 1.",10,0
bus1_master_not_exists_str  db "Master drive doesn't exist on ATA bus 1.",10,0
bus1_slave_exists_str       db "Slave drive exists on ATA bus 1.",10,0
bus1_slave_not_exists_str   db "Slave drive doesn't exist on ATA bus 1.",10,0