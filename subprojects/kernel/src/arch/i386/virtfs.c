#include "fs/virtfs.h"
#include "fs/fat16.h"
#include "kio.h"
#include "klib.h"

struct intern_fd_t ifdtable[IFDTABLE_SIZE];

int register_filesystem(struct filesystemdescriptor_t fs) {
    int index = -1;
    // Find an empty fsd
    for (int i = 0; i < FSDTABLE_SIZE; i++) {
        if (fsdtable[i].name[0] == 0) {
            index = i;
            break;
        }
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
}

int set_ifd(uint8_t attr, int drive, char *path) {
    int id;

    if (kstrlen(path) > PATH_MAX_LEN) return -1;
    if (drive < 0) return -1;

    // Find a suitable fd
    id = get_free_ifd();
    if (id >= 0) {
        struct intern_fd_t *ifd = &(ifdtable[id]);
        ifd->attr = attr;
        ifd->drive = drive;
        kmemcpy(ifd->path, path, kstrlen(path) + 1); // Copy string and null term.
    }

    return id;
}

int del_ifd(int ifd) {
    if (ifd < 0 || ifd >= IFDTABLE_SIZE) return -1;
    ifdtable[ifd].pres = 0;
    return ifd;
}

int get_free_ifd() {
    for (int i = 0; i < IFDTABLE_SIZE; i++) {
        if (!ifdtable[i].pres) {
            return i;
        }
    }
    return -1;
}

char *vfs_parse_path(int *drivenum, const char *path) {
    char *colon = kstrchr(path, ':');
    if (!colon) return NULL;
    *colon = '\0';
    // TODO: Return NULL if the number string isn't a valid number
    *drivenum = katoi(path);
    *colon = ':';
    return colon + 1;
}

//TODO: Currently, the vfs_open and similar functions pass the path in the format drvn:path_in_drive, but it would be tidier and slightly
//      more efficient to pass the path as just path_in_drive so that the filesystem code doesn't have to parse the path again. To do this,
//      the filesystem functions would need to all take an additional parameter for the drive number.

int vfs_open(const char *path, int flags) {
    int drvn;
    vfs_parse_path(&drvn, path);
    struct drive_t drv = drivetable[drvn];
    struct filesystemdescriptor_t fs = fsdtable[drv.fs];
    return fs.open(path, flags);
}

int vfs_close(int fd) {
    return fsdtable[drivetable[ifdtable[fd].drive].fs].close(fd);
}

int vfs_read(int fd, void *dest, uint32_t start, uint32_t n) {
    struct intern_fd_t ifd = ifdtable[fd];
    struct drive_t drv = drivetable[ifd.drive];
    struct filesystemdescriptor_t fs = fsdtable[drv.fs];

    return fs.read(fd, dest, start, n);
}

int vfs_write(int fd, void *src, uint32_t start, uint32_t n) {
    return fsdtable[drivetable[ifdtable[fd].drive].fs].write(fd, src, start, n);
}

int vfs_mkdir(const char *path, int flags) {
    int drvn;
    vfs_parse_path(&drvn, path);
    struct drive_t drv = drivetable[drvn];
    struct filesystemdescriptor_t fs = fsdtable[drv.fs];
    return fs.mkdir(path, flags);
}

int vfs_getpath(int fd, char *path) {
    return fsdtable[drivetable[ifdtable[fd].drive].fs].getpath(fd, path);
}

int vfs_rmdir(const char *path) {
    int drvn;
    vfs_parse_path(&drvn, path);
    struct drive_t drv = drivetable[drvn];
    struct filesystemdescriptor_t fs = fsdtable[drv.fs];
    return fs.rmdir(path);
}

int vfs_rmfile(const char *path) {
    int drvn;
    vfs_parse_path(&drvn, path);
    struct drive_t drv = drivetable[drvn];
    struct filesystemdescriptor_t fs = fsdtable[drv.fs];
    return fs.rmfile(path);
}