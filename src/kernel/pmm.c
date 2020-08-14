#include "pmm.h"
#include "kio.h"

void pmm_init(uint32_t* mboot_info, uint8_t* bitmap) {
    uint32_t mem_lower, mem_upper;
    uint32_t amount_blocks, bitmapbytes;

    kio_puts("Kernel: ");
    kio_puthex((uint32_t)(&_kernel_start));
    kio_puts(" -> ");
    kio_puthex((uint32_t)(&_kernel_end));
    kio_puts("\nInitialising pmm bitmap.\n");

    if ((*mboot_info) & 0x1) { // mem_lower and mem_upper are valid
        uint32_t mem_lower = (*mboot_info + 4); // lower memory KB
        uint32_t mem_upper = (*mboot_info + 8); // upper memory KB
    }

    amount_blocks = ((mem_upper << 10) + 0x100000) >> 12;
    bitmapbytes = amount_blocks >> 3;

    for (uint32_t i = 0; i < bitmapbytes; i++)
        bitmap[i] = 0;
}