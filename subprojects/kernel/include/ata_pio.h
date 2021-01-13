#ifndef ATA_PIO_H
#define ATA_PIO_H

#include <stdint.h>

#define ATA_PIO_STATUS_ERR 0x00
#define ATA_PIO_STATUS_IDX 0x01
#define ATA_PIO_STATUS_COR 0x02
#define ATA_PIO_STATUS_DRQ 0x04
#define ATA_PIO_STATUS_SRV 0x08
#define ATA_PIO_STATUS_DFE 0x10
#define ATA_PIO_STATUS_RDY 0x20
#define ATA_PIO_STATUS_BSY 0x40

void ata_pio_init();
void ata_pio_wait_status_set(uint8_t mask);
void ata_pio_wait_status_unset(uint8_t mask);
void ata_pio_read_sectors(uint8_t count, uint32_t target, uint32_t lba);
void ata_pio_write_sectors(uint8_t count, uint32_t source, uint32_t lba);

#endif