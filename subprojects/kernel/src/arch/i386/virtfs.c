#include "fs/virtfs.h"
#include "fs/fat16.h"
#include "klib.h"

struct intern_fd_t ifdtable[IFDTABLE_SIZE];

int register_filesystem(struct filesystemdescriptor_t fs) {
    int index = -1;
    // Find an empty fsd
    for (int i = 0; i < FSDTABLE_SIZE; i++) {
        if (fsdtable[i].name[0] == 0) index = i;
    }

    if (index >= 0) {
        fsdtable[index] = fs;
    }

    return index;
}

int get_filesystem_index(const char *name) {
    for (int i = 0; i < FSDTABLE_SIZE; i++) {
        // Check if `name` is the same as `fsdtable[i].name`.
        if (kstrcmp(name, fsdtable[i].name) == 0) {
            return i;
        }
    }
    
    return -1;
}

void vfs_init() {
    // Clear the internal fd table
    kmemset(ifdtable, 0, IFDTABLE_SIZE * sizeof(struct intern_fd_t));
    kmemset(fsdtable, 0, FSDTABLE_SIZE * sizeof(struct filesystemdescriptor_t));

    fat16_init();
}

int set_ifd(uint8_t attr, int drive, char *path) {
    int id;

    if (kstrlen(path) > PATH_MAX_LEN) return -1;
    if (drive < 0) return -1;

    // Find a suitable fd
    for (id = 0; (id < IFDTABLE_SIZE) && (ifdtable[id].attr & IFD_ATTR_PRESENT); id++);

    struct intern_fd_t *ifd = &(ifdtable[id]);
    ifd->attr = attr;
    ifd->drive = drive;
    kmemcpy(ifd->path, path, kstrlen(path) + 1); // Copy string and null term.

    return id;
}

int del_ifd(int ifd) {
    if (ifd < 0 || ifd >= IFDTABLE_SIZE) return -1;
    ifdtable[ifd].attr = 0;
    return ifd;
}