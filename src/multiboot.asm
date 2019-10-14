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