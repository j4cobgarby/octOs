#ifndef FS_VIRTFS_H
#define FS_VIRTFS_H

#include <stdint.h>

#define DRIVETABLE_SIZE         32
#define DRIVETYPETABLE_SIZE     32
#define FSDTABLE_SIZE           8
#define PATH_MAX_LEN            248
#define FSD_NAME_LEN            10

// dn refers to the drive number (0 is invalid)
struct filesystemdescriptor_t {
    char name[FSD_NAME_LEN];

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
    char    name[16]; // The name of the drive
};

// This is the sort of file descriptor that a process would know about, so that
// each process's file descriptors can start at 0. Each process has its own
// fd table, maintained by the kernel, which contains lots of this struct.
#define FD_ATTR_PRESENT     0x01

struct fd_t {
    uint8_t attr;
    int intern_fd;
};

#define IFD_ATTR_PRESENT    0x01

struct intern_fd_t {
    uint8_t attr;
    int drive;
    char path[PATH_MAX_LEN]; // The path to the file within the drive
};

struct drivetype_t {
    unsigned int blocksize;
    void (*rdsect)(uint32_t lba, uint8_t count, void *dest, void *param);
    void (*wrsect)(uint32_t lba, uint8_t count, void *src, void *param);
};

extern struct drive_t drivetable[DRIVETABLE_SIZE];
extern struct drivetype_t drivetypetable[DRIVETYPETABLE_SIZE];
extern struct filesystemdescriptor_t fsdtable[FSDTABLE_SIZE];

int register_drivetype(unsigned int blocksize, 
    void (*rdsect)(uint32_t, uint8_t, void*, void*), 
    void (*wrsect)(uint32_t, uint8_t, void*, void*));

int register_drive(int drivetype, int fstype, void *param,
    char name[16]);

int register_filesystem(struct filesystemdescriptor_t fs);

void vfs_init();
void drivetypes_init();

#endif