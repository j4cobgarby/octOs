#ifndef FS_VIRTFS_H
#define FS_VIRTFS_H

#include <stdint.h>

#define DRIVETABLE_SIZE 128
#define PATH_MAX_LEN    248

// dn refers to the drive number (0 is invalid)
struct virtfs_t {
    char name[8];

    int (*open)(const char *fname, int flags); // Return fd
    int (*close)(int fd);

    // Read/write AT A POSITION, not using the unix style seek/read
    int (*read)(int fd, void *dest, uint32_t start, uint32_t n);
    int (*write)(int fd, void *src, uint32_t start, uint32_t n);

    int (*mkdir)(const char *dname, int flags);
    int (*getpath)(int fd, char *path);

    int (*rmdir)(const char *dname);
    int (*rmfile)(int fd);
};

#define DRIVE_ATTR_PRESENT  0x01
#define DRIVE_ATTR_WRITABLE 0x02

struct drive_t {
    uint8_t attr;
    uint16_t phy_sect_size;
    uint16_t log_sect_size;
    int     type; // The type of drive, for the filesystem to know how to read blocks
    int     fs; // The type of filesystem present on the drive
    void    *type_param; // Parameter passed to the filesystem's functions
    char    name[16]; // The name of the drive
};

// This is the sort of file descriptor that a process would know about, so that
// each process's file descriptors can start at 0. Each process has its own
// fd table, maintained by the kernel, which contains lots of this struct.
#define FD_ATTR_PRESENT     0x01

#define FD_TABLE_LEN    

struct fd_t {
    uint8_t attr;
    int intern_fd;
};

#define IFD_ATTR_PRESENT    0x01

struct intern_fd_t {
    uint8_t attr;
    int drive;
    char path[PATH_MAX_LEN]; // The path to the file (excluding the drive identifier)
};

struct drivetype_t {
    char name[12];
    void (*rdsect)(uint32_t bladdr, uint8_t count, void *dest);
    void (*wrsect)(uint32_t bladdr, uint8_t count, void *src);
};

extern struct drive_t drivetable[DRIVETABLE_SIZE];

#endif