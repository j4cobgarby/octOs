#include "kio.h"
#include "klib.h"
#include "ata_pio.h"
#include "fs/virtfs.h"
#include "fs/fat16.h"

void kmain() {
    drivetypes_init();
    fat16_init();

    int ata_id = get_drivetype_index("ATA");
    // kio_printf("ATA type id = %d\n", ata_id);

    struct ata_drive_t *drv0 = drivetable[0].drive_param;
    // kio_printf("io: 0x%x, ctrl: 0x%x\n", drv0->io_port_base, drv0->io_ctrl_base);

    uint16_t dest[512];
    //ata_pio_virtfs_rdsect(1, 1, dest, &drivetable[0]);
    ata_pio_rd(drv0, 0, 1, &dest[0]);
    kio_printf("Sect 0: \n");
    for (int i = 0; i < 128; i++) {
        uint32_t word = ((uint32_t*)dest)[i];
        kio_puthex(word);
        kio_putc(' ');
        if ((i+1) % 8 == 0) {
            kio_putc('\n');
        }
    }
    kio_putc('\n');
    
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