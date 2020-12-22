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
    kio_puts("[PMM] multiboot flags: ");
    kio_puthex(*mboot_info);
    kio_puts("\n[PMM] Kernel: ");
    kio_puthex((uint32_t)(&_kernel_start));
    kio_puts(" -> ");
    kio_puthex((uint32_t)(&_kernel_end));
    kio_putc('\n');
#endif
    kio_puts("[PMM] Initialising bitmap...");

    if ((*mboot_info) & 0x1) { // mem_lower and mem_upper are valid
        mem_lower = mboot_info[1]; // lower memory KB
        mem_upper = mboot_info[2]; // upper memory KB
#ifdef KERNEL_DEBUG
        kio_puts("\nMemory bitmap address: ");
        kio_puthex((uint32_t)&pmm_bitmap);
        kio_puts("\nLower memory limit (KBs): ");
        kio_puthex(mem_lower);
        kio_puts("\nUpper memory limit (KBs): ");
        kio_puthex(mem_upper);
        kio_putc('\n');
#endif
    } else {
        KIO_ERRORMSG("[PMM] Multiboot header didn't provide memory limits.\n");
        while(1);
    }

    amount_blocks = ((mem_upper << 10) + 0x100000) >> 12; // mem_upper convd. to bytes, then add on upper memory offset
    bitmapbytes = amount_blocks >> 3;

    kio_puts("Amount of bytes: ");
    kio_puthex(bitmapbytes);
    kio_puts("\n");

    for (uint32_t i = 0; i < bitmapbytes; i++)
        ((uint8_t*)&pmm_bitmap)[i] = 0xff;
    ((uint8_t*)&pmm_bitmap)[bitmapbytes] = 'A';

    kio_puts("Done.\n");

    if ((*mboot_info) & 0x40) {
        kio_puts("[PMM] Reading memory map...");

        mmap_length = mboot_info[11];
        mmap_addr = mboot_info[12];

#ifdef KERNEL_DEBUG
        kio_puts("\n[PMM] MMap address: ");
        kio_puthex(mmap_addr);
        kio_puts("   MMap length: ");
        kio_puthex(mmap_length);
        kio_puts("\n");
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
            kio_puts("- Region start: ");
            kio_puthex(entry_base);
            kio_puts(" Region length: ");
            kio_puthex(entry_length);
            
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

        kio_puts("Done.");
    } else {
        kio_puts("Multiboot header didn't provide memory map.\n");
        while(1);
    }

    pmm_sets(ADDR_BLOCK((uint32_t)&_kernel_start),
        (ADDR_BLOCK((uint32_t)&_kernel_end)) - (ADDR_BLOCK((uint32_t)&_kernel_start)));
}

void pmm_set(uint32_t block) {
    ((uint8_t*)&pmm_bitmap)[block >> 3] |= 1 << (block % 8);
}

void pmm_sets(uint32_t startblock, uint32_t amount) {
    for (uint32_t b = startblock; b < startblock+amount; b++) {
        pmm_set(b);
    }
}

void pmm_unset(uint32_t block) {
    ((uint8_t*)&pmm_bitmap)[block >> 3] &= ~(1 << (block % 8));
}

void pmm_unsets(uint32_t startblock, uint32_t amount) {
    for (uint32_t b = startblock; b < startblock+amount; b++) {
        pmm_unset(b);
    }
}