%include "src/multiboot.asm"

section .bss
align 16
;; the stack is defined here.
;; defined in .bss to save kernel space, since it'll just say
;; "16384 uninitialised bytes here", rather than actually list
;; that many bytes out in the object file!
stack_bottom:
    resb 8192
stack_top:

ring3_stack_bottom:
    resb 8192
ring3_stack_top:

section .text   ; now the actual kernel entry point is in this section
global _start:function (_start.end - _start)    ; make the object file store the length of the _start symbol

_kernel_start:

%include "src/common.asm"
%include "src/drivers/gdt.asm"
%include "src/drivers/idt.asm"
%include "src/drivers/pic.asm"
%include "src/drivers/pit.asm"
%include "src/drivers/keyboard.asm"
%include "src/drivers/syscall.asm"
%include "src/kernel/k_procs.asm"
%include "src/kernel/pmm.asm"

msg1 db "oct v0.1.2-a", 0
msg_memlower db 10, "Amount of lower memory: 0x", 0
msg_memupper db 10, "Amount of upper memory: 0x", 0
msg_ksize db 10, "Kernel size: 0x", 0
msg_noflag0 db 10, "Flag 0 not set. Kernel cannot determine memory size.", 0
msg_noflag6 db 10, "Flag 6 not set. Can't determine what memory is available.", 0
msg_sizeof db " length: ", 0
msg_start db "start: ",0
msg_type db " type: ", 0
msg_available db " (available)", 0
msg_acpi db " (acpi info)", 0
msg_hibernatemem db " (presvd. on hibernate)", 0
msg_defectivemem db " (defective memory)", 0
msg_unavailable db " (reserved)", 0
msg_kb db "K",0
msg_mb db "M",0

_start: ; kernel entry point
    ;warning: don't touch ebx until after pmm_init
    mov esp, stack_top
    cli

    call kterm_clear
    mov eax, msg1
    call kterm_putstr

    call pmm_init

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

    jmp hltlp
.end:

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
hltlp: hlt
    jmp hltlp

ptable_base dd 0    ; base address of process table
ptable_processes dw 0 ; amount of entries in process table

pmm_memlowersize dd 0
pmm_memuppersize dd 0
pmm_mmaplength dd 0
pmm_mmapaddress dd 0
pmm_amountblocks dd 0
pmm_bitmapbytes dd 0 ; amount of bytes needed to represent the bitmap
_kernel_end:
pmm_bitmap: ; the beginning of the physical memory manager's bitmap
            ; each bit will represent a page in memory, of size PMM_BLOCKSIZE