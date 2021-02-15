#include "fs/virtfs.h"
#include "fs/fat16.h"

int register_filesystem(struct filesystemdescriptor_t fs) {
    static int index = 0;

    if (index >= FSDTABLE_SIZE) {
        return -1;
    } else {
        fsdtable[index] = fs;
    }

    return index++;
}

void vfs_init() {
    fat16_init();
}