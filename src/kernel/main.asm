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

ring3_stack_bottom:
    resb 16384
ring3_stack_top:

section .text   ; now the actual kernel entry point is in this section
global _start:function (_start.end - _start)    ; make the object file store the length of the _start symbol

%include "src/drivers/gdt.asm"
%include "src/drivers/idt.asm"
%include "src/drivers/pic.asm"
%include "src/drivers/pit.asm"
%include "src/drivers/keyboard.asm"
%include "src/drivers/syscall.asm"
%include "src/kernel/k_procs.asm"

msg1 db "oct alpha", 0
msg2 db 10,"Reading multiboot info at 0x", 0 
msg3 db 10,"Multiboot info flags: 0x", 0

_start: ; kernel entry point
    mov esp, stack_top
    cli

    call kterm_clear
    mov eax, msg1
    call kterm_putstr
    mov eax, msg2
    call kterm_putstr
    mov eax, ebx
    call kterm_puthex

    mov dword edx, [ebx]

    mov eax, msg3
    call kterm_putstr
    mov eax, edx
    call kterm_puthex

    bt edx, 0 ; flag 0 specifies if mem_lower and mem_upper are valid

    call fill_tss_descriptor
    lgdt [gdt_descriptor]
    call load_tss
    call refresh_segments

    ; Set up addresses of various IDT descriptors
    call init_pit
    call init_keyboard
    call init_syscall
    lidt [idt_descriptor]
    call pic_init

    jmp .hltlp

    ; set up paging
    ;mov eax, cr0
    ;or eax, 0x80000001   ; set cr0.pg and cr0.pe
                        ; i think that cr0.pe would've already been set
                        ;  by grub, but i'm setting it here just in case
    ;mov cr0, eax        ; am now using 32bit paging

    ; now the GDT is set up, as well as a TSS entry, and also the IDT is set up
    ; ready to go into userspace
; .jump_userland:
;     mov ax, 0x23 ; 0x23 is the 4th GDT entry (user data)
;     mov ds, ax   ; load the user data segment into all of
;     mov es, ax   ; the data segment registers
;     mov fs, ax
;     mov gs, ax

    ; ; set up call stack for iret
    ; extern userspace_entry
    ; push 0x23       ; the data segment again
    ; mov eax, ring3_stack_top
    ; push eax        ; the stack pointer
    ; pushf           ; push the EFLAGS register
    ; push 0x1b       ; user code segment
    ; push userspace_entry
    ; iret            ; "return" to the userland
.hltlp: hlt
    jmp .hltlp
.end:

.endofkernel: