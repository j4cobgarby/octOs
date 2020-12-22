#include "pmm.h"
#include "kio.h"

extern uint8_t pmm_bitmap;

void pmm_init(uint32_t* mboot_info) {
    uint32_t mem_lower, mem_upper;
    uint32_t amount_blocks, bitmapbytes;
    uint32_t mmap_length, mmap_addr;
    uint32_t* mmap_entry_start; // start address of current mmap entry
    uint32_t entry_size;
    uint32_t entry_base;
    uint32_t entry_length;
    uint32_t entry_type;

#ifdef KERNEL_DEBUG
    kio_printf("[PMM] multiboot flags: %b\n", *mboot_info);
    kio_printf("[PMM] kernel limits: %x -> %x\n", (uint32_t)(&_kernel_start), (uint32_t)(&_kernel_end));
#endif
    kio_puts("[PMM] Initialising bitmap...");

    if ((*mboot_info) & 0x1) { // mem_lower and mem_upper are valid
        mem_lower = mboot_info[1]; // lower memory KB
        mem_upper = mboot_info[2]; // upper memory KB
#ifdef KERNEL_DEBUG
        kio_printf("\nPMM Bitmap address: %x\n", (uint32_t)&pmm_bitmap);
        kio_printf("Lower memory limit: %dKB\n", mem_lower);
        kio_printf("Upper memory limit: %dKB\n", mem_upper);
#endif
    } else {
        KIO_ERRORMSG("[PMM] Multiboot header didn't provide memory limits.\n");
        while(1);
    }

    amount_blocks = ((mem_upper << 10) + 0x100000) >> 12; // mem_upper convd. to bytes, then add on upper memory offset
    bitmapbytes = amount_blocks >> 3;

    pmm_blocks_max = amount_blocks;
    // All blocks are set at the beginning
    pmm_blocks_used = amount_blocks;

    for (uint32_t i = 0; i < bitmapbytes; i++)
        ((uint8_t*)&pmm_bitmap)[i] = 0xff;

    kio_puts("Done.\n");

    if ((*mboot_info) & 0x40) {
        kio_puts("[PMM] Reading memory map...");

        mmap_length = mboot_info[11];
        mmap_addr = mboot_info[12];

#ifdef KERNEL_DEBUG
        kio_printf("\n[PMM] mmap addr: %x, mmap length: %d\n", mmap_addr, mmap_length);
#endif

        for (mmap_entry_start = (uint32_t*)mmap_addr
            ; (uint32_t)mmap_entry_start < mmap_addr + mmap_length
            ; mmap_entry_start += (entry_size + 4)/4) {
            entry_size = mmap_entry_start[0]; // length of the table entry
            entry_base = mmap_entry_start[1];
            entry_length = mmap_entry_start[3]; // length of the memory region
            entry_type = mmap_entry_start[5];

            if (entry_type == 1) { // Available memory
                pmm_unsets(ADDR_BLOCK(entry_base), ADDR_BLOCK(entry_length));
            }

#ifdef KERNEL_DEBUG
            kio_printf("- Start: %x Length: %x", entry_base, entry_length);
            
            switch (entry_type) {
            case 1:
                kio_puts(" (Available)");
                break;
            case 3:
                kio_puts(" (ACPI)");
                break;
            case 4:
                kio_puts(" (To be presvd. on hibernation)");
                break;
            case 5:
                kio_puts(" (Defective)");
                break;
            default:
                kio_puts(" (Reserved)");
            }

            kio_putc('\n');
#endif
        }

        kio_puts("Done.\n");
    } else {
        kio_puts("Multiboot header didn't provide memory map.\n");
        while(1);
    }

    pmm_sets(ADDR_BLOCK((uint32_t)&_kernel_start),
        (ADDR_BLOCK((uint32_t)&_kernel_end)) - (ADDR_BLOCK((uint32_t)&_kernel_start)));
}

void pmm_set(uint32_t block) {
    if (!pmm_isset(block)) {
        // If this block wasn't set, then that means
        // one more block is now in use.
        pmm_blocks_used++;
    }

    // Set the bit in the bitmap
    ((uint8_t*)&pmm_bitmap)[block >> 3] |= 1 << (block % 8);
}

void pmm_sets(uint32_t startblock, uint32_t amount) {
    for (uint32_t b = startblock; b < startblock+amount; b++) {
        pmm_set(b);
    }
}

void pmm_unset(uint32_t block) {
    if (pmm_isset(block)) {
        // If this block was set, then that means now one less block
        // is in use.
        pmm_blocks_used--;
    }

    // Unset the bit in the bitmap
    ((uint8_t*)&pmm_bitmap)[block >> 3] &= ~(1 << (block % 8));
}

void pmm_unsets(uint32_t startblock, uint32_t amount) {
    for (uint32_t b = startblock; b < startblock+amount; b++) {
        pmm_unset(b);
    }
}

uint8_t pmm_isset(uint32_t block) {
    return 0;
}