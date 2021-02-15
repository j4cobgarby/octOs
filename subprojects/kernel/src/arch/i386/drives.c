#include "fs/virtfs.h"
#include "klib.h"
#include "pmm.h"
#include "ata_pio.h"
#include "kio.h"

struct drive_t drivetable[DRIVETABLE_SIZE];
struct drivetype_t drivetypetable[DRIVETYPETABLE_SIZE];
struct filesystemdescriptor_t fsdtable[FSDTABLE_SIZE];

int register_drivetype(unsigned int blocksize, 
        void (*rdsect)(uint32_t, uint8_t, void*, void*), 
        void (*wrsect)(uint32_t, uint8_t, void*, void*)) {
    static int id = 0;

    drivetypetable[id].blocksize = blocksize;
    drivetypetable[id].rdsect = rdsect;
    drivetypetable[id].wrsect = wrsect;

    return id++;
}

int register_drive(int drivetype, int fstype, void *param,
        char name[16]) {
    static int id = 0;

    drivetable[id].drive_param = param;
    kmemcpy(drivetable[id].name, name, 16);
    drivetable[id].fs = fstype;
    drivetable[id].type = drivetype;

    return id++;
}

void drivetypes_init() {
    int atapio_id = register_drivetype(512, &ata_pio_virtfs_rdsect, &ata_pio_virtfs_wrsect);
    ata_pio_init();

    kio_printf("Initialised drive types.\n");
    kio_printf("ATA PIO id = %d\n", atapio_id);
}