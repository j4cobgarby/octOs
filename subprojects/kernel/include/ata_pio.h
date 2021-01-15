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

#define ATA_PIO_BUS0 0x1f0
#define ATA_PIO_BUS0_STAT 0x3f6

void ata_pio_init();

// Wait until the status byte AND mask is non zero
void ata_pio_wait_status_set(uint8_t mask);

// Wait until the status byte AND mask is zero
void ata_pio_wait_status_unset(uint8_t mask);

void ata_pio_read_sectors(uint8_t sectors, uint32_t target, uint32_t lba);
void ata_pio_write_sectors(uint8_t sectors, uint32_t source, uint32_t lba);

#endif