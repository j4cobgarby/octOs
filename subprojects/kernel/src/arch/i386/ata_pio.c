#include "ata_pio.h"
#include "asm_procs.h"

#include "kio.h"
#include "pmm.h"

void ata_pio_init() {
    // For now, only use ATA bus 0 master
    
    // Test out ATA thing
    uint32_t *target = pmm_alloc();
    
    struct ata_bus_t bus0;
    bus0.io_port_base = 0x1f0;
    bus0.io_ctrl_base = 0x3f6;

    kio_printf("Reading 1 sector\n");
    ata_pio_read(&bus0, 0, 0, 1, target);
    
    /* for (uint16_t i = 0; i < 128; i++) {
        kio_printf("%x ", target[i]);
        if (i % 8 == 7) {
            kio_putc('\n');
        }
    } */
}

void ata_pio_wait_status_set(struct ata_bus_t *bus, uint8_t mask) {
    while (!(inb(bus->io_port_base + ATA_PIO_STAT) & mask));
}

void ata_pio_wait_status_unset(struct ata_bus_t *bus, uint8_t mask) {
    while (inb(bus->io_port_base + ATA_PIO_STAT) & mask);
}

void ata_pio_read(struct ata_bus_t *bus, uint8_t drv, uint32_t lba, uint8_t n,
        void *dest) {
    kio_printf("1\n");
    ata_pio_wait_status_unset(bus, ATA_PIO_STATUS_BSY);

    outb(bus->io_port_base+ATA_PIO_SECT, n);

    outb(bus->io_port_base+ATA_PIO_LBAL, (uint8_t)lba);
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 8));
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 16));

    uint8_t drvreg = 0xe0; // bit 5-7 are set always
    drvreg |= drv << 4; // bit 4 = drive number
    drvreg |= (lba >> 24) & 0xf; // bits 0-3 = lba address of block
    outb(bus->io_port_base+ATA_PIO_DRV, drvreg);

    outb(bus->io_port_base+ATA_PIO_CMND, 0x20);

    uint16_t *target = (uint16_t*)dest;

    for (uint8_t i = 0; i < n; i++) {
        kio_printf("2\n");
        ata_pio_wait_status_unset(bus, ATA_PIO_STATUS_BSY);

        // Wait for data to be available
        kio_printf("3\n");
        ata_pio_wait_status_set(bus, ATA_PIO_STATUS_DRQ);

        // For each sector, read a sectors worth (256 words = 512 bytes)
        // of data to the target memory.
        for (int i = 0; i < 256; i++, target++) {
            kio_printf(".");
            *target = inw(bus->io_port_base+ATA_PIO_DATA);
        }

        kio_printf("4\n");
    }
}