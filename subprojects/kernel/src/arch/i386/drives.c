#include "fs/virtfs.h"
#include "klib.h"
#include "pmm.h"
#include "ata_pio.h"
#include "kio.h"

struct drive_t drivetable[DRIVETABLE_SIZE];
struct drivetype_t drivetypetable[DRIVETYPETABLE_SIZE];
struct filesystemdescriptor_t fsdtable[FSDTABLE_SIZE];

int register_drivetype(unsigned int bytespersector, 
        void (*rdsect)(uint32_t, uint8_t, void*, void*), 
        void (*wrsect)(uint32_t, uint8_t, void*, void*),
        char name[DRIVETYPE_NAME_LEN]) {
    static int id = 0;

    drivetypetable[id].bytespersector = bytespersector;
    drivetypetable[id].rdsect = rdsect;
    drivetypetable[id].wrsect = wrsect;
    kmemcpy(drivetypetable[id].name, name, kstrlen(name));
    drivetypetable->name[DRIVETYPE_NAME_LEN-1] = 0;

    return id++;
}

int register_drive(int drivetype, int fstype, void *param) {
    static int id = 0;

    drivetable[id].drive_param = param;
    drivetable[id].fs = fstype;
    drivetable[id].type = drivetype;
    fsdtable[fstype].mount(id);

    return id++;
}

void drivetypes_init() {
    register_drivetype(512, &ata_pio_virtfs_rdsect, &ata_pio_virtfs_wrsect, "ATA");
    ata_pio_init();

    kio_printf("[VFS] Initialised drive types.\n");
}

int get_drivetype_index(const char *name) {
    for (int i = 0; i < DRIVETYPETABLE_SIZE; i++) {
        if (kstrcmp(name, drivetypetable[i].name) == 0) {
            return i;
        }
    }
    
    return -1;
}