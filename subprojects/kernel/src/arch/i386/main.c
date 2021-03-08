#include "kio.h"
#include "klib.h"
#include "ata_pio.h"
#include "fs/virtfs.h"
#include "fs/fat16.h"

void kmain() {
    drivetypes_init();
    fat16_init();

    int ata_id = get_drivetype_index("ATA");
    kio_printf("ATA type id = %d\n", ata_id);
/*
    struct ata_drive_t drv;
    drv.drive_n = 0;
    drv.io_port_base = 0x1f0;
    drv.io_ctrl_base = 0x3f6;
    drv.bytes_per_sector = 512;
    drv.first_sector = 0;
    drv.sector_count = 100;
    
    int drvnum = register_drive(0, 0, &drv, "ATA0:0/0"); // ATA bus 0, drive 0, partition 0 (whole disk)
    fat16_open("0:/user/jacob/myfile.txt", 0);
*/
}