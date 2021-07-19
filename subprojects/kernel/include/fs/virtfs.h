#ifndef FS_VIRTFS_H
#define FS_VIRTFS_H

#include <stdint.h>

#include "klib.h"

#define DRIVETABLE_SIZE         32
#define DRIVETYPETABLE_SIZE     32
#define DRIVETYPE_NAME_LEN      16
#define FSDTABLE_SIZE           8
#define PATH_MAX_LEN            248
#define FSD_NAME_LEN            10
#define IFDTABLE_SIZE           128

#define FILE_DIRECTORY  0x01
#define FILE_WRITABLE   0x02
#define FILE_READABLE   0x04

// dn refers to the drive number (0 is invalid)
struct filesystemdescriptor_t {
    // `name` must be a C-string (null-terminated)
    char name[FSD_NAME_LEN]; // if name[0] == 0, this fsd is not in use.

    int (*open)(const char *path, int flags); // Return fd
    int (*close)(int fd);

    // Read/write AT A POSITION, not using the unix style seek/read
    int (*read)(int fd, void *dest, uint32_t start, uint32_t n);
    int (*write)(int fd, void *src, uint32_t start, uint32_t n);

    int (*mkdir)(const char *path, int flags);
    int (*getpath)(int fd, char *path);

    int (*rmdir)(const char *path);
    int (*rmfile)(const char *path);
};

#define DRIVE_ATTR_PRESENT  0x01
#define DRIVE_ATTR_WRITABLE 0x02

struct drive_t {
    uint8_t attr;
    int     type; // The type of drive
    int     fs; // The type of filesystem present on the drive

    void    *drive_param; // Parameter passed to the drive's functions
};

// This is the sort of file descriptor that a process would know about, so that
// each process's file descriptors can start at 0. Each process has its own
// fd table, maintained by the kernel, which contains lots of this struct.

struct fd_t {
    short int pres;
    uint8_t attr;
    int intern_fd;
};

struct intern_fd_t {
    short int pres;
    uint8_t attr;
    int drive;
    char path[PATH_MAX_LEN]; // The path to the file within the drive
    void *fsdat; // Data which makes sense to the filesystem
};

struct drivetype_t {
    char name[DRIVETYPE_NAME_LEN];
    unsigned int bytespersector;
    void (*rdsect)(uint32_t lba, uint8_t count, void *dest, void *param);
    void (*wrsect)(uint32_t lba, uint8_t count, void *src, void *param);
};

extern struct drive_t drivetable[DRIVETABLE_SIZE];
extern struct drivetype_t drivetypetable[DRIVETYPETABLE_SIZE];
extern struct filesystemdescriptor_t fsdtable[FSDTABLE_SIZE];
extern struct intern_fd_t ifdtable[IFDTABLE_SIZE];

int register_drivetype(unsigned int blocksize, 
    void (*rdsect)(uint32_t, uint8_t, void*, void*), 
    void (*wrsect)(uint32_t, uint8_t, void*, void*),
    char name[DRIVETYPE_NAME_LEN]);

int register_drive(int drivetype, int fstype, void *param);

int register_filesystem(struct filesystemdescriptor_t fs);

// Finds the index of a fsd with a given name in fsdtable
// Returns the index if it exists, otherwise returns -1
int get_filesystem_index(const char *name);
int get_drivetype_index(const char *name);

int set_ifd(uint8_t attr, int drive, char path[PATH_MAX_LEN]);
int del_ifd(int ifd);
void vfs_init();

// Extracts the drive number from a path, and returns the pointer to the path after the drive number specifier
char *vfs_parse_path(int *drivenum, const char *path);
void drivetypes_init();
int get_free_ifd();

int vfs_open(const char *path, int flags);
int vfs_close(int fd);

int vfs_read(int fd, void *dest, uint32_t start, uint32_t n);
int vfs_write(int fd, void *src, uint32_t start, uint32_t n);

int vfs_mkdir(const char *path, int flags);
int vfs_getpath(int fd, char *path);

int vfs_rmdir(const char *path);
int vfs_rmfile(const char *path);

#endif