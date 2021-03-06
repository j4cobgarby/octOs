#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stdint.h>

#define ATA_PIO_STATUS_ERR 0x01
#define ATA_PIO_STATUS_IDX 0x02
#define ATA_PIO_STATUS_COR 0x04
#define ATA_PIO_STATUS_DRQ 0x08
#define ATA_PIO_STATUS_SRV 0x10
#define ATA_PIO_STATUS_DFE 0x20
#define ATA_PIO_STATUS_RDY 0x40
#define ATA_PIO_STATUS_BSY 0x80

#define ATA_PIO_DATA    0x0
#define ATA_PIO_ERR     0x1
#define ATA_PIO_FEAT    0x1
#define ATA_PIO_SECT    0x2
#define ATA_PIO_LBAL    0x3
#define ATA_PIO_LBAM    0x4
#define ATA_PIO_LBAH    0x5
#define ATA_PIO_DRV     0x6
#define ATA_PIO_STAT    0x7
#define ATA_PIO_CMND    0x7

#define ATA_CMD_READ    0x20
#define ATA_CMD_WRITE   0x30
#define ATA_CMD_IDENT   0xec

struct ata_drive_t {
    uint16_t io_port_base;
    uint16_t io_ctrl_base;
    uint8_t drive_n; // 0 for master, 1 for slave
    // The first sector of the drive to read for, to deal with partitions
    uint32_t first_sector;
    // The total amount of sectors in the drive (or partition)
    uint32_t sector_count;
    // Amount of bytes in 1 sector
    uint16_t bytes_per_sector;
};

struct ata_bus_t {
    uint8_t active_drive;
    uint16_t io_port_base;
    uint16_t io_ctrl_base;
};

void ata_pio_init();

uint8_t ata_pio_read_status(struct ata_drive_t *drv);

// Wait until the status byte AND mask is non zero
void ata_pio_wait_status_set(struct ata_drive_t *drv, uint8_t mask);

// Wait until the status byte AND mask is zero
void ata_pio_wait_status_unset(struct ata_drive_t *drv, uint8_t mask);

void ata_pio_rd(struct ata_drive_t *drv, uint32_t lba, uint8_t n, void *dest);
void ata_pio_wr(struct ata_drive_t *drv, uint32_t lba, uint8_t n, void *src);

int ata_pio_identify(struct ata_drive_t *drv);
void ata_pio_scandrives();

// Functions for the virtual filesystem interface
void ata_pio_virtfs_rdsect(uint32_t lba, uint8_t count, void *dest, void *param);
void ata_pio_virtfs_wrsect(uint32_t lba, uint8_t count, void *src, void *param);


#endif