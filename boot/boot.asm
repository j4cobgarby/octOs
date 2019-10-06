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
    dd MB_MAGICNUMBER
    dd MB_FLAGS
    dd MB_CHKSUM
    dd 0,0,0,0,0 ; 5 * 4 zero bytes in a row, because i'm not using flag 16
    dd MB_MODETYPE
    dd MB_WIDTH
    dd MB_HEIGHT
    dd MB_DEPTH

section .bss
align 16
;; the stack is defined here.
;; defined in .bss to save kernel space, since it'll just say
;; "16384 uninitialised bytes here", rather than actually list
;; that many bytes out in the object file!
stack_bottom:
    resb 16384
stack_top:

section .text   ; now the actual kernel entry point is in this section
global _start:function (_start.end - _start)    ; make the object file store the length of the _start symbol
_start: ; the actual entry point to the kernel!
    mov esp, stack_top  ; setup the stack, because obviously it's nice to have a stack, and also
                        ; C programs need a stack to run

    ;; load CPU features here, like IDT, GDT, etc.

    ;;

    extern kernel_main  ; kernel_main is defined in a C file
    call kernel_main    ; run the main kernel procedure

    ; only gets here if the kernel returns, which would be weird, but if it does happen
    ; then just halt the CPU
    ; if a NMI wakes it up, halt it again
    cli
.hlt hlt
    jmp .hlt
.end: ; marks the end of the _start symbol's code
