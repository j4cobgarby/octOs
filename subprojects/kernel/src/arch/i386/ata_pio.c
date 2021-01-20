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
    ata_pio_read(&bus0, 1, 0, 1, target);
    
    for (uint16_t i = 0; i < 128; i++) {
        kio_printf("%x ", target[i]);
        if (i % 8 == 7) {
            kio_putc('\n');
        }
    }

    pmm_memset(target, 0, 512);
    ata_pio_write(&bus0, 1, 0, 1, target);

    kio_printf("Finished writing\n");
}

uint8_t ata_pio_read_status(struct ata_bus_t *bus, uint8_t drv) {
    uint8_t drvreg = 0xe0;
    drvreg |= drv << 4;
    outb(bus->io_port_base+ATA_PIO_DRV, drvreg);

    // Create a delay to allow the drive to bus the status on the bus
    inb(bus->io_port_base + ATA_PIO_STAT);
    inb(bus->io_port_base + ATA_PIO_STAT);
    inb(bus->io_port_base + ATA_PIO_STAT);
    inb(bus->io_port_base + ATA_PIO_STAT);
    return inb(bus->io_port_base + ATA_PIO_STAT);
}

void ata_pio_wait_status_set(struct ata_bus_t *bus, uint8_t drv, uint8_t mask) {
    while (!(ata_pio_read_status(bus, drv) & mask));
}

void ata_pio_wait_status_unset(struct ata_bus_t *bus, uint8_t drv, uint8_t mask) {
    while (ata_pio_read_status(bus, drv) & mask);
}

void ata_pio_read(struct ata_bus_t *bus, uint8_t drv, uint32_t lba, uint8_t n,
        void *dest) {
    ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);

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
        ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);
        ata_pio_wait_status_set(bus, drv, ATA_PIO_STATUS_DRQ);

        // For each sector, read a sectors worth (256 words = 512 bytes)
        // of data to the target memory.
        for (int i = 0; i < 256; i++, target++) {
            *target = inw(bus->io_port_base+ATA_PIO_DATA);
        }
    }
}

void ata_pio_write(struct ata_bus_t *bus, uint8_t drv, uint32_t lba, uint8_t n, 
        void *src) {
    ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);

    outb(bus->io_port_base+ATA_PIO_SECT, n);
    outb(bus->io_port_base+ATA_PIO_LBAL, (uint8_t)lba);
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 8));
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 16));
    uint8_t drvreg = 0xe0; // bit 5-7 are set always
    drvreg |= drv << 4; // bit 4 = drive number
    drvreg |= (lba >> 24) & 0xf; // bits 0-3 = lba address of block
    outb(bus->io_port_base+ATA_PIO_DRV, drvreg);
    outb(bus->io_port_base+ATA_PIO_CMND, 0x30);

    uint16_t *srcptr = (uint16_t*)src;

    for (uint8_t i = 0; i < n; i++) {
        ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);
        ata_pio_wait_status_set(bus, drv, ATA_PIO_STATUS_DRQ);

        for (int i = 0; i < 256; i++, srcptr++) {
            outw(bus->io_port_base+ATA_PIO_DATA, *srcptr);
        }
    }
}