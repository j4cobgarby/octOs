#include "pmm.h"
#include "kio.h"

void pmm_init(uint32_t* mboot_info, uint8_t* bitmap) {
    uint32_t mem_lower, mem_upper;
    uint32_t amount_blocks, bitmapbytes;
    uint32_t mmap_length, mmap_addr;
    uint32_t* mmap_entry_start; // start address of current mmap entry
    uint32_t entry_size;
    uint32_t entry_base;
    uint32_t entry_length;
    uint32_t entry_type;

#ifdef KERNEL_DEBUG
    kio_puts("multiboot flags: ");
    kio_puthex(*mboot_info);
    kio_puts("\nKernel: ");
    kio_puthex((uint32_t)(&_kernel_start));
    kio_puts(" -> ");
    kio_puthex((uint32_t)(&_kernel_end));
    kio_putc('\n');
#endif
    kio_puts("Initialising pmm bitmap...");

    if ((*mboot_info) & 0x1) { // mem_lower and mem_upper are valid
        mem_lower = (*mboot_info + 4); // lower memory KB
        mem_upper = (*mboot_info + 8); // upper memory KB
#ifdef KERNEL_DEBUG
        kio_puts("Lower memory limit (KBs): ");
        kio_puthex(mem_lower);
        kio_puts("\nUpper memory limit (KBs): ");
        kio_puthex(mem_upper);
        kio_putc('\n');
#endif
    } else {
        kio_puts("\nMultiboot header didn't provide required info.\n");
        while(1);
    }

    amount_blocks = ((mem_upper << 10) + 0x100000) >> 12;
    bitmapbytes = amount_blocks >> 3;

    for (uint32_t i = 0; i < bitmapbytes; i++)
        bitmap[i] = 0;

    for (uint32_t block = ADDR_BLOCK((uint32_t)&_kernel_start); block <= ADDR_BLOCK((uint32_t)&_kernel_end); block++) {
        bitmap[block] = 1;
    }

    kio_puts("Done.\n");

    if ((*mboot_info) & 0x40) {
        kio_puts("Reading memory map...");

        mmap_length = mboot_info[11];
        mmap_addr = mboot_info[12];

#ifdef KERNEL_DEBUG
        kio_puts("\nMMap address: ");
        kio_puthex(mmap_addr);
        kio_puts("   MMap length: ");
        kio_puthex(mmap_length);
        kio_puts("\n");
#endif

        for (mmap_entry_start = (uint32_t*)mmap_addr
            ; (uint32_t)mmap_entry_start < mmap_addr + mmap_length
            ; mmap_entry_start += (entry_size + 4)/4) {
            entry_size      = mmap_entry_start[0];
            entry_base      = mmap_entry_start[1];
            entry_length    = mmap_entry_start[3];
            entry_type      = mmap_entry_start[5];

#ifdef KERNEL_DEBUG
            kio_puts("- Memory region: ");
            kio_puthex(entry_base);
            kio_puts(" ==> ");
            kio_puthex(entry_base + entry_length);
            kio_putc('\n');
#endif
        }

        kio_puts("Done.");
    } else {
        kio_puts("Multiboot header didn't provide memory map.\n");
        while(1);
    }
}