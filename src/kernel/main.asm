%include "src/multiboot.asm"
%include "src/common.asm"

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

%include "src/drivers/gdt.asm"
%include "src/drivers/idt.asm"
%include "src/drivers/pic.asm"
%include "src/drivers/keyboard.asm"

_start: ; kernel entry point
        mov esp, stack_top      ; Set up stack
        cli     ; Disable interrupts until everything is set up

        call fill_tss_descriptor
        lgdt [gdt_descriptor]
        call load_tss
        call refresh_segments
        call init_keyboard      ; Put the address of the keyboard ISR
                                ; into the IDT
        lidt [idt_descriptor]
        call pic_init           ; Set up the PIC

        ; now the GDT is set up, as well as a TSS entry, and also the IDT is set up
        ; ready to go into userspace
.hltlp: hlt
        jmp .hltlp
.end: