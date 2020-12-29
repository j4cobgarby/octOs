# Memory management

## Physical memory

The code for managing physical memory is mostly in `pmm.h` and `pmm.c`,
as well as some in `main.asm`.

oct keeps track of which blocks of memory are free and which aren't using
a bitmap in memory, where each block in the bitmap represents a contiguous
4KB block of memory. If a bit in the bitmap is 1, then that block is taken.

Block 0 is always "taken", so that the alloc functions can return 0 as an
error value.

Some useful memory addresses to know:
 - The address at which the kernel begins is defined in the kernel's linker.ld 
    script. At the time of writing, this is set to 0x100000 (=1MB).

## Virtual memory

The kernel is always at virtual address 0x100000. Userspace memory will begin
at 2GB virtual (0x80000000). Virtual memory management is done mostly in
`paging.c` (and `paging.h`).