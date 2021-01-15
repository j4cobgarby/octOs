#include "ata_pio.h"
#include "asm_procs.h"

#include "kio.h"
#include "pmm.h"

void ata_pio_init() {
    // For now, only use ATA bus 0 master
    
    // Test out ATA thing
    uint32_t *target = pmm_alloc();
    ata_pio_read_sectors(2, (uint32_t)target, 0);
    
    for (uint16_t i = 0; i < 128; i++) {
        kio_printf("%x ", target[i]);
        if (i % 8 == 7) {
            kio_putc('\n');
        }
    }
}

void ata_pio_wait_status_set(uint8_t mask) {
    while (!(inb(ATA_PIO_PORT_BUS0_BASE+7) & mask));
}

void ata_pio_wait_status_unset(uint8_t mask) {
    while (inb(ATA_PIO_PORT_BUS0_BASE+7) & mask);
}

void ata_pio_read_sectors(uint8_t sectors, uint32_t target, uint32_t lba) {
    ata_pio_wait_status_unset(ATA_PIO_STATUS_BSY);

    outb(ATA_PIO_PORT_BUS0_BASE+2, sectors);

    // Set the lba address (across multiple registers)
    outb(ATA_PIO_PORT_BUS0_BASE+3, (uint8_t)lba);
    outb(ATA_PIO_PORT_BUS0_BASE+4, (uint8_t)(lba >> 8));
    outb(ATA_PIO_PORT_BUS0_BASE+4, (uint8_t)(lba >> 16));
    // This one has lba in the lower nibble only, top nibble = 0xe
    outb(ATA_PIO_PORT_BUS0_BASE+6, 0xe0 | ((lba >> 24) & 0xf));

    // Send the "read" command
    outb(ATA_PIO_PORT_BUS0_BASE+7, 0x20);

    uint16_t *target_ptr = (uint16_t*) target;

    for (uint8_t i = 0; i < sectors; i++) {
        ata_pio_wait_status_unset(ATA_PIO_STATUS_BSY);

        // Wait for data to be available
        ata_pio_wait_status_set(ATA_PIO_STATUS_DRQ);

        // For each sector, read a sectors worth (256 words = 512 bytes)
        // of data to the target memory.
        for (int i = 0; i < 256; i++, target_ptr++) {
            *target_ptr = inw(ATA_PIO_PORT_BUS0_BASE);
        }
    }

    kio_printf("[ATA] Finished reading %d sectors.\n", sectors);
}