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

#define ATA_PIO_PORT_BUS0_BASE 0x1f0
#define ATA_PIO_PORT_BUS1_BASE 0x170

#define ATA_PIO_PORT_BUS0_STAT 0x3f6
#define ATA_PIO_PORT_BUS1_STAT 0x376


void ata_pio_init();

// Wait until the status byte AND mask is non zero
void ata_pio_wait_status_set(uint8_t mask);

// Wait until the status byte AND mask is zero
void ata_pio_wait_status_unset(uint8_t mask);

void ata_pio_read_sectors(uint8_t sectors, uint32_t target, uint32_t lba);
void ata_pio_write_sectors(uint8_t sectors, uint32_t source, uint32_t lba);

#endif