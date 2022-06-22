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

## Dynamic Memory Allocation

When memory is needed dynamically (e.g. the amount is not known at boot),
it's not convenient to have to use the pmm_alloc function to get this memory,
since if you want an amount of memory that is less than the size of a page,
a lot of memory will be wasted. Therefore, functions like malloc are required
to allocate just a little bit of memory.

### In the kernel (old version)

The kernel can use the functions `kmalloc` and `kfree` to allocate and free a
specific amount of memory. The heap is a linked list of 4KB blocks of memory.

At the beginning of each block in the heap is some metadata, which contains the
following properties: (`block_meta_t`)

 - A pointer to the next block
 - The amount of free subblocks in the block
 - An array of bytes representing each subblock

And then, the rest of the block is available for `kmalloc` to allocate memory
in. In the subblock array, a value of 0 represents a free subblock, and every
other value represents an allocated subblock. Neighbouring allocations of
subblocks must be different values, so that `kfree` can differentiate between
them.

### In the kernel (new version)

The kernel can use the functions `kmalloc` and `kfree` to allocate and free a
specific amount of memory. In previous versions, this amount of memory had to
be less than 4KB of memory (the size of a block on the heap), but now allocat-
ed memory can span multiple blocks, and so can be any size up the total amount
of memory.

The heap is implemented as a linked list of blocks. Each block is up of cont-
iguous chunks of 4KB. A block can be as small as 4KB, but can have any number
of these chunks. At the beginning of a block is some metadata, which contains
the following properties: (`block_meta_t`)

 - The amount of 4KB chunks in this block.
 - A pointer to the next block
 - The amount of free subblocks in the block
 - An array of bytes representing the subblocks.

### In user processes

TODO