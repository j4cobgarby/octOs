tss_entry:
    dd 0x00000000 ; prev_tss: unused
    dd 0x00000000 ; esp0: the stack pointer which needs to be loaded when kernel mode is entered
    dd 0x00000010 ; ss0: the kernel stack segment (the data segment in the GDT)
    dd 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0    ; i won't use the rest of the TSS
                                                        ; because i'm using software task switching
tss_limit equ $ - tss_entry