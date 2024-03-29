%include "src/arch/i386/multiboot.asm"

section .bss
global pmm_bitmap
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

pmm_bitmap: ; the beginning of the physical memory manager's bitmap
            ; each bit will represent a page in memory, of size PMM_BLOCKSIZE
    resb 0x100000 ; allocate the maximum possible needed amount of memory (4GB worth of blocks)

section .data
    ; data here

section .text   ; now the actual kernel entry point is in this section
global _start:function (_start.end - _start)    ; make the object file store the length of the _start symbol
global _kernel_start
global _kernel_end

_kernel_start:

%include "src/arch/i386/common.asm"
%include "src/arch/i386/gdt.asm"
%include "src/arch/i386/idt.asm"
%include "src/arch/i386/pic.asm"
%include "src/arch/i386/pit.asm"
%include "src/arch/i386/keyboard.asm"
%include "src/arch/i386/syscall.asm"

extern pmm_init
extern vmm_init
extern kio_puts
extern kio_puts_attr
extern kio_init
extern kio_cls
extern kio_print_kernel_banner
extern register_exceptions
extern kmain

_start: ; kernel entry point
    ;warning: don't touch ebx until after pmm_init

    mov esp, stack_top
    cli

    call kio_cls
    call kio_init

    call kio_print_kernel_banner

    push dword ebx
    call pmm_init
    add esp, 4
    
    call vmm_init

    call fill_tss_descriptor
    lgdt [gdt_descriptor]
    call load_tss
    call refresh_segments

    ; Set up addresses of various IDT descriptors
    call init_pit
    call init_keyboard
    call init_syscall
    lidt [idt_descriptor]
    call register_exceptions
    call pic_init

    call kmain

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

ptable_base dd 0      ; base address of process table
ptable_pages db 0     ; amount of pages used to store the ptable
ptable_processes dw 0 ; amount of entries in process table

pmm_memlowersize dd 0
pmm_memuppersize dd 0
pmm_mmaplength dd 0
pmm_mmapaddress dd 0
pmm_amountblocks dd 0
pmm_bitmapbytes dd 0 ; amount of bytes needed to represent the bitmap
_kernel_end:
