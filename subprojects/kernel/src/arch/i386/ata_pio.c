#include "ata_pio.h"
#include "asm_procs.h"

#include "fs/virtfs.h"
#include "kio.h"
#include "pmm.h"
#include "fs/fat16.h"
#include "fs/MBR.h"

#include <klib.h>

void ata_pio_init() {
    ata_pio_scandrives();
#if 0
    struct ata_bus_t bus0;
    bus0.io_port_base = 0x1f0;
    bus0.io_ctrl_base = 0x3f6;

    uint8_t *dest = pmm_alloc();
    ata_pio_read(&bus0, 0, 0, 1, dest);

    struct mbr_partition_t *partition_table = (struct mbr_partition_t*)(dest+0x1be);
    for (int i = 0; i < 4; i++) {
        kio_printf("Part. %d: %d -> %d (%x)\n", i, partition_table[i].lba_first,
            partition_table[i].lba_first+partition_table[i].sect_count,
            partition_table[i].type);
    }

    kio_printf("Reading sector: %d\n", partition_table[0].lba_first);
    uint8_t *part1_0 = pmm_alloc();
    ata_pio_read(&bus0, 0, 2048, 1, part1_0);
    struct fat16_bpb_t *bpb = (struct fat12_bpb_t*)(part1_0+0x0b);
    kio_printf("Rootdir entries: %d\n", bpb->rootdir_entries);
    kio_printf("Bytes per sector: %d\n", bpb->bytes_per_sect);
    kio_printf("Sectors per cluster: %d\n", bpb->sect_per_clust);

    uint16_t blocks_per_fat = (bpb->bytes_per_sect*bpb->sect_per_fat)/4096;
    kio_printf("Blocks per FAT: %d\n", blocks_per_fat);
    uint8_t *fat1 = pmm_allocs(blocks_per_fat);
    ata_pio_read(&bus0, 0, bpb->resvd_sects+2048, blocks_per_fat, fat1);
    kio_printf("FAT1 @ %x\n", (uint32_t)fat1);
    uint32_t rootdir_sect = bpb->resvd_sects + (bpb->sect_per_fat*bpb->number_of_fats);
    struct fat16_dir_entry_t *rootdir = pmm_alloc();
    ata_pio_read(&bus0, 0, rootdir_sect+2048, 1, rootdir);
    kio_printf("Root dir sector: %d\n", rootdir_sect);
    for (int i = 0; i < bpb->rootdir_entries; i++) {
        if (rootdir[i].filename[0] == 0x00) break;
        if ((uint8_t)rootdir[i].filename[0] == 0xe5) continue;
        if (rootdir[i].attr == 0x0f) continue;
        kio_puts_n(rootdir[i].filename, 8);
        kio_puts_n(rootdir[i].extension, 3);
        kio_printf("\nATTR: %x, Cluster: %d\n", rootdir[i].attr, rootdir[i].cluster_start);
        kio_updatecurs();
    }

    /* struct fat12_bpb_t *bpb = (struct fat12_bpb_t*)(dest + 0x0b);
    struct fat12_ebr_t *ebr = (struct fat12_ebr_t*)(dest + 0x0b + sizeof(struct fat12_bpb_t));
    kio_printf("\
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
        ebr->drive_number, ebr->sig, ebr->serial);

    uint16_t blocks_per_fat = (bpb->bytes_per_sect*bpb->sect_per_fat)/4096;
    kio_printf("Blocks per FAT: %d\n", blocks_per_fat);
    uint8_t *fat1 = pmm_allocs(blocks_per_fat);
    ata_pio_read(&bus0, 1, bpb->resvd_sects, blocks_per_fat, fat1);
    kio_printf("FAT1 @ %x\n", (uint32_t)fat1);
    uint32_t rootdir_sect = bpb->resvd_sects + (bpb->sect_per_fat*bpb->number_of_fats);
    struct fat12_dir_entry_t *rootdir = pmm_alloc();
    ata_pio_read(&bus0, 1, rootdir_sect, 1, rootdir);
    kio_printf("Root dir sector: %d\n", rootdir_sect);
    for (int i = 0; i < bpb->rootdir_entries; i++) {
        if (rootdir[i].filename[0] == 0x00) break;
        if ((uint8_t)rootdir[i].filename[0] == 0xe5) continue;
        kio_puts_n(rootdir[i].filename, 8);
        kio_putc('.');
        kio_puts_n(rootdir[i].extension, 3);
        kio_putc('\n');
        kio_updatecurs();
    } */
#endif
}

uint8_t ata_pio_read_status(struct ata_drive_t *drv) {
    uint8_t drvreg = 0xe0;
    drvreg |= drv->drive_n << 4;
    outb(drv->io_port_base+ATA_PIO_DRV, drvreg);

    // Create a delay to allow the drive to bus the status on the bus
    inb(drv->io_port_base + ATA_PIO_STAT);
    inb(drv->io_port_base + ATA_PIO_STAT);
    inb(drv->io_port_base + ATA_PIO_STAT);
    inb(drv->io_port_base + ATA_PIO_STAT);
    return inb(drv->io_port_base + ATA_PIO_STAT);
}

void ata_pio_wait_status_set(struct ata_drive_t *drv, uint8_t mask) {
    while (!(ata_pio_read_status(drv) & mask));
}

void ata_pio_wait_status_unset(struct ata_drive_t *drv, uint8_t mask) {
    while (ata_pio_read_status(drv) & mask);
}

void ata_pio_rd(struct ata_drive_t *drv, uint32_t lba, uint8_t n, void *dest) {
    uint8_t drv_reg = 0xe0;
    uint16_t *target = (uint16_t *)dest;

    // bit 4 of the drive register is the drive number (0 or 1)
    drv_reg |= drv->drive_n << 4; 
    // bits 0-3 of the drive register are 25-32 of the lba
    drv_reg |= (lba >> 24) & 0xf;

    ata_pio_wait_status_unset(drv, ATA_PIO_STATUS_BSY);
    kio_printf("[ATA] %x, %x\n", drv->io_port_base, drv->io_ctrl_base);

    // Tell the bus how many sectors to read
    outb(drv->io_port_base + ATA_PIO_SECT, n);

    // Tell the bus the address of the block
    outb(drv->io_port_base + ATA_PIO_LBAL, (uint8_t)lba);
    outb(drv->io_port_base + ATA_PIO_LBAM, (uint8_t)(lba >> 8));
    outb(drv->io_port_base + ATA_PIO_LBAH, (uint8_t)(lba >> 16));

    // Tell the bus which drive to use (plus the last 8 bits of the address)
    outb(drv->io_port_base + ATA_PIO_DRV, drv_reg);

    // Tell the drive to read the data
    outb(drv->io_port_base + ATA_PIO_CMND, ATA_CMD_READ);

    for (uint8_t i = 0; i < n; i++) {
        ata_pio_wait_status_unset(drv, ATA_PIO_STATUS_BSY);
        ata_pio_wait_status_set(drv, ATA_PIO_STATUS_DRQ);

        for (int j = 0; j < 256; j++, target++) {
            *target = inw(drv->io_port_base + ATA_PIO_DATA);
        }
    }
}

void ata_pio_wr(struct ata_drive_t *drv, uint32_t lba, uint8_t n, void *src) {
    uint8_t drv_reg = 0xe0;
    uint32_t *srcptr = (uint32_t *)src;

    // bit 4 of the drive register is the drive number (0 or 1)
    drv_reg |= drv->drive_n << 4; 
    // bits 0-3 of the drive register are 25-32 of the lba
    drv_reg |= (lba >> 24) & 0xf;

    ata_pio_wait_status_unset(drv, ATA_PIO_STATUS_BSY);

    // Tell the bus how many sectors we will write to it
    outb(drv->io_port_base + ATA_PIO_SECT, n);

    // Tell the bus which block to start writing at
    outb(drv->io_port_base + ATA_PIO_LBAL, (uint8_t)lba);
    outb(drv->io_port_base + ATA_PIO_LBAM, (uint8_t)(lba >> 8));
    outb(drv->io_port_base + ATA_PIO_LBAH, (uint8_t)(lba >> 16));

    // Tell the bus which drive to use (+ last 8 bits of the block address)
    outb(drv->io_port_base + ATA_PIO_DRV, drv_reg);

    // Tell the bus we will now write to it
    outb(drv->io_port_base + ATA_PIO_CMND, ATA_CMD_WRITE);

    for (uint8_t i = 0; i < n; i++) {
        ata_pio_wait_status_unset(drv, ATA_PIO_STATUS_BSY);
        ata_pio_wait_status_set(drv, ATA_PIO_STATUS_DRQ);

        for (int i = 0; i < 256; i++, srcptr++) {
            outw(drv->io_port_base+ATA_PIO_DATA, *srcptr);
        }
    }
}

int ata_pio_identify(struct ata_drive_t *drv) {
    ata_pio_wait_status_unset(drv, ATA_PIO_STATUS_BSY);

    outb(drv->io_port_base + ATA_PIO_DRV, 
        drv->drive_n == 0 ?
            0xa0 : 
            0xb0);

    outb(drv->io_port_base + ATA_PIO_SECT, 0);
    outb(drv->io_port_base + ATA_PIO_LBAL, 0);
    outb(drv->io_port_base + ATA_PIO_LBAM, 0);
    outb(drv->io_port_base + ATA_PIO_LBAH, 0);

    outb(drv->io_port_base + ATA_PIO_CMND, ATA_CMD_IDENT);

    uint8_t stat = inb(drv->io_port_base + ATA_PIO_STAT);

    if (!stat) return 0;

    ata_pio_wait_status_unset(drv, ATA_PIO_STATUS_BSY);
    while (!(ata_pio_read_status(drv) & (ATA_PIO_STATUS_DRQ | ATA_PIO_STATUS_ERR)));

    if (ata_pio_read_status(drv) & ATA_PIO_STATUS_ERR) return 0;

    //TODO: Extract the information from the identify command's returned data
    for (int i = 0; i < 256; i++) {
        inw(drv->io_port_base + ATA_PIO_DATA);
    }

    return 1;
}

void ata_pio_virtfs_rdsect(uint32_t lba, uint8_t count, void *dest, void *param) {
    struct ata_drive_t *drive_info = (struct ata_drive_t *)param;
    ata_pio_rd(drive_info, lba, count, dest);
}

void ata_pio_virtfs_wrsect(uint32_t lba, uint8_t count, void *src, void *param) {
    struct ata_drive_t *drive_info = (struct ata_drive_t *)param;
    ata_pio_wr(drive_info, lba, count, src);
}

void ata_pio_scandrives() {
    struct ata_drive_t tmp_drv;

    kio_printf("[ATA] Scanning for drives...\n");

    // Check the primary ATA bus
    tmp_drv.io_port_base = 0x1f0;
    tmp_drv.io_ctrl_base = 0x3f6;
    // Check master and slave on the bus
    for (int i = 0; i <= 1; i++) {
        tmp_drv.drive_n = i;
        if (ata_pio_identify(&tmp_drv)) {
            kio_printf("[ATA] Found a drive (%d)\n", i);
            struct ata_drive_t *actual_drive = kmalloc(sizeof(struct ata_drive_t));
            actual_drive->drive_n = tmp_drv.drive_n;
            actual_drive->io_port_base = tmp_drv.io_port_base;
            actual_drive->io_ctrl_base = tmp_drv.io_ctrl_base;

            //TODO: Properly calculate these values
            actual_drive->bytes_per_sector = 512;
            actual_drive->first_sector = 0;
            actual_drive->sector_count = 100;

            int d = register_drive(get_drivetype_index("ATA"), get_filesystem_index("FAT16"), actual_drive);
            kio_printf("[ATA] New drive registered (%x:%d [%d])\n", 
                actual_drive->io_port_base, actual_drive->drive_n, d);
        } else {
            kio_printf("[ATA] No drive (%x:%d)\n", tmp_drv.io_port_base, i);
        }
    }
}