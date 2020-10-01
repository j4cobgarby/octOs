#ifndef ATA_PIO_H
#define ATA_PIO_H

#include "asm_procs.h"
#include <stdint.h>

/* ATA Base IO port ofsets */
#define ATA_DATA        0
#define ATA_ERR         1
#define ATA_FEATURES    1
#define ATA_SCOUNT      2
#define ATA_LBALOW      3
#define ATA_LBAMID      4
#define ATA_LBAHI       5
#define ATA_DRIVEHEAD   6
#define ATA_STATUS      7
#define ATA_COMMAND     7

/* ATA Control IO port offsets */
#define ATA_ALTSTATUS   0
#define ATA_DEVCTRL     0
#define ATA_DRIVEADDR   1

/* Stores information about */
typedef struct {
    uint16_t base_addr; /* The IO address for most of the ATA registers for this drive */
    uint16_t control_addr;
} ata_drive_t;

/* Initialises ATA stuff. */
void ata_start();

/* Returns 1 if drive exists, 0 if it doesn't. Also sets the pointer arguments to the right values. */
int ata_identify_drive(uint16_t base_addr, uint16_t control_addr, int *lba48, 
    uint64_t *lba48_sects, uint32_t *lba28_sects);

#endif