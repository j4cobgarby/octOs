#include "kio.h"
#include "ata_pio.h"
#include "fs/virtfs.h"
#include "fs/fat16.h"

void kmain() {
    drivetypes_init();
    fat16_init();
}