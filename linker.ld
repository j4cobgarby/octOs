ENTRY(_start)

SECTIONS
{
    /* start putting code at 1MB, apparently this is a
     * normal place for kernel code to be loaded from*/
    . = 1M;

    /* multiboot header has to be early on in the image so
     * that grub finds it */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)   /* first the multiboot header */
        *(.text)        /* now the actual entry code */
    }

    /* now the uninitialised data */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(.bss) /* just the stack */
    }
}
