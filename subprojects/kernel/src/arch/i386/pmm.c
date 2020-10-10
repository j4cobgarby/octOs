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

    kio_puts("multiboot flags: ");
    kio_puthex(*mboot_info);
    kio_puts("\nKernel: ");
    kio_puthex((uint32_t)(&_kernel_start));
    kio_puts(" -> ");
    kio_puthex((uint32_t)(&_kernel_end));
    kio_puts("\nInitialising pmm bitmap.\n");

    if ((*mboot_info) & 0x1) { // mem_lower and mem_upper are valid
        kio_puts("Reading memory size...\n");
        mem_lower = (*mboot_info + 4); // lower memory KB
        mem_upper = (*mboot_info + 8); // upper memory KB
        kio_puts("Lower memory limit (KBs): ");
        kio_puthex(mem_lower);
        kio_puts("\nUpper memory limit (KBs): ");
        kio_puthex(mem_upper);
        kio_putc('\n');
    } else {
        kio_puts("Multiboot header didn't provide required info.\n");
        while(1);
    }

    amount_blocks = ((mem_upper << 10) + 0x100000) >> 12;
    bitmapbytes = amount_blocks >> 3;

    for (uint32_t i = 0; i < bitmapbytes; i++)
        bitmap[i] = 0;

    for (uint32_t block = ADDR_BLOCK((uint32_t)&_kernel_start); block <= ADDR_BLOCK((uint32_t)&_kernel_end); block++) {
        bitmap[block] = 1;
    }

    if ((*mboot_info) & 0x40) {
        kio_puts("Reading memory map...\n");

        mmap_length = mboot_info[11];
        mmap_addr = mboot_info[12];

        kio_puts("Mmap address: ");
        kio_puthex(mmap_addr);
        kio_puts("   Mmap length: ");
        kio_puthex(mmap_length);
        kio_putc('\n');
    /*
        mmap_entry_start = (uint32_t*)mmap_addr;

        while ((uint32_t)mmap_entry_start < mmap_addr + mmap_length) {
            kio_puts("====== memory map entry ======\n");
            entry_size = *(mmap_entry_start);
            kio_puthex((uint32_t)mmap_entry_start);
            kio_putc('\n');
            kio_puthex(entry_size);
            kio_putc('\n');
            //entry_base = *(mmap_entry_start + 4);
            //entry_length = *(mmap_entry_start + 12);
            //entry_type = *(mmap_entry_start + 20);

            //kio_puthex(entry_type);
            mmap_entry_start += 0x18;
        }*/

        for (mmap_entry_start = (uint32_t*)mmap_addr
            ; (uint32_t)mmap_entry_start < mmap_addr + mmap_length
            ; mmap_entry_start += (entry_size + sizeof(uint32_t))/4) {
            kio_puts("====== memory map entry ======\n");
            entry_size = *(mmap_entry_start);
            kio_puthex((uint32_t)mmap_entry_start);
            kio_putc('\n');
            kio_puthex(entry_size);
            kio_putc('\n');
        }
    } else {
        kio_puts("Multiboot header didn't provide memory map.\n");
        while(1);
    }
}