#include "ata_pio.h"
#include "asm_procs.h"

#include "kio.h"
#include "pmm.h"
#include "fs/FAT12.h"

void ata_pio_init() {
    struct ata_bus_t bus0;
    bus0.io_port_base = 0x1f0;
    bus0.io_ctrl_base = 0x3f6;

    uint8_t *dest = pmm_alloc();
    ata_pio_read(&bus0, 1, 0, 1, dest);

/*     for (int i = 0; i < 512; i++) {
        if (i % 64 == 0) {
            kio_putc('\n');
        }
        if (dest[i] != '\n')
            kio_printf("%c", dest[i]);
    } */

    struct fat12_bpb_t *bpb = (struct fat12_bpb_t*)(dest + 0x0b);
    struct fat12_ebr_t *ebr = (struct fat12_ebr_t*)(dest + 0x0b + sizeof(struct fat12_bpb_t));
    /* kio_printf("\
Bytes per sector: %d\n\
Sectors per cluster: %d\n\
Reserved sectors: %d\n\
FAT count: %d\n\
Root dir entries: %d\n\
Total sectors: %d\n\
Media descriptor: %x\n\
Sectors per FAT: %d\n\
Sectors per track: %d\n\
Number of heads: %d\n\
Number of hidden sectors: %d\n\
Large sectors count: %d\n\
Drive number: %d\n\
Signature: %x\n\
Serial number: %x\n",
        bpb->bytes_per_sect, bpb->sect_per_clust, bpb->resvd_sects, bpb->number_of_fats,
        bpb->rootdir_entries, bpb->total_sects, bpb->media_desc, bpb->sect_per_fat,
        bpb->sect_per_track, bpb->number_heads, bpb->number_hidden_sects, bpb->largesect_count,
        ebr->drive_number, ebr->sig, ebr->serial); */

    uint16_t blocks_per_fat = (bpb->bytes_per_sect*bpb->sect_per_fat)/4096;
    kio_printf("Blocks per FAT: %d\n", blocks_per_fat);
    uint8_t *fat1 = pmm_allocs(blocks_per_fat);
    ata_pio_read(&bus0, 1, bpb->resvd_sects, blocks_per_fat, fat1);
    kio_printf("FAT1 @ %x\n", (uint32_t)fat1);
    uint32_t rootdir_sect = bpb->resvd_sects + (bpb->sect_per_fat*bpb->number_of_fats);
    struct fat12_dir_entry_t *rootdir = pmm_alloc();
    ata_pio_read(&bus0, 1, rootdir_sect, 1, rootdir);
    kio_printf("Root dir sector: %d\n", rootdir_sect);
    kio_printf("Filename: ");
    kio_puts_n(rootdir[0].filename, 8);
    kio_printf("\nExtension: ");
    kio_puts_n(rootdir[0].extension, 3);
    kio_printf("\nLength: %d, Start cluster: %d\n", rootdir[0].bytes,
        rootdir[0].cluster_start);
    kio_printf("%d-%d-%d\n", rootdir[0].date_created.year, rootdir[0].date_created.month,
        rootdir[0].date_created.day);
    kio_printf("%d:%d:%d\n", rootdir[0].time_created.hour, rootdir[0].time_created.mins,
        rootdir[0].time_created.secs*2);
    kio_printf("Corresponding fat entry: 0x%x\n", 
        fat12_read_fat_entry(fat1, rootdir[0].cluster_start));
    char *file_sector = pmm_alloc();
    uint32_t filesect_addr = rootdir_sect + (bpb->sect_per_clust*rootdir[0].cluster_start);
    kio_printf("Sector: %x\n", filesect_addr);
    ata_pio_read(&bus0, 1, filesect_addr, 1, file_sector);
    kio_puts_n(file_sector, rootdir[0].bytes);
    file_sector[0] = 'H';
    ata_pio_write(&bus0, 1, filesect_addr, 1, file_sector);
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
    if (drv > 1) {
#ifdef KERNEL_DEBUG
        kio_printf("[ATA] Invalid drive number %d\n", drv);
        return;
#endif
    }

    ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);

    outb(bus->io_port_base+ATA_PIO_SECT, n);
    outb(bus->io_port_base+ATA_PIO_LBAL, (uint8_t)lba);
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 8));
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 16));
    uint8_t drvreg = 0xe0; // bit 5-7 are set always
    drvreg |= drv << 4; // bit 4 = drive number
    drvreg |= (lba >> 24) & 0xf; // bits 0-3 = lba address of block
    outb(bus->io_port_base+ATA_PIO_DRV, drvreg);
    outb(bus->io_port_base+ATA_PIO_CMND, ATA_CMD_READ);

    uint16_t *target = (uint16_t*)dest;

    for (uint8_t i = 0; i < n; i++) {
        ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);
        ata_pio_wait_status_set(bus, drv, ATA_PIO_STATUS_DRQ);

        for (int i = 0; i < 256; i++, target++) {
            *target = inw(bus->io_port_base+ATA_PIO_DATA);
        }
    }
}

void ata_pio_write(struct ata_bus_t *bus, uint8_t drv, uint32_t lba, uint8_t n, 
        void *src) {
    if (drv > 1) {
#ifdef KERNEL_DEBUG
        kio_printf("[ATA] Invalid drive number %d\n", drv);
        return;
#endif
    }

    ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);

    outb(bus->io_port_base+ATA_PIO_SECT, n);
    outb(bus->io_port_base+ATA_PIO_LBAL, (uint8_t)lba);
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 8));
    outb(bus->io_port_base+ATA_PIO_LBAM, (uint8_t)(lba >> 16));
    uint8_t drvreg = 0xe0; // bit 5-7 are set always
    drvreg |= drv << 4; // bit 4 = drive number
    drvreg |= (lba >> 24) & 0xf; // bits 0-3 = lba address of block
    outb(bus->io_port_base+ATA_PIO_DRV, drvreg);
    outb(bus->io_port_base+ATA_PIO_CMND, ATA_CMD_WRITE);

    uint16_t *srcptr = (uint16_t*)src;

    for (uint8_t i = 0; i < n; i++) {
        ata_pio_wait_status_unset(bus, drv, ATA_PIO_STATUS_BSY);
        ata_pio_wait_status_set(bus, drv, ATA_PIO_STATUS_DRQ);

        for (int i = 0; i < 256; i++, srcptr++) {
            outw(bus->io_port_base+ATA_PIO_DATA, *srcptr);
        }
    }
}