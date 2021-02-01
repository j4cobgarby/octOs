#ifndef FS_VIRTFS_H
#define FS_VIRTFS_H

#include <stdint.h>

// dn refers to the drive number (0 is invalid)
struct virtfs_t {
    int (*open)(const char *fname, int flags, int dn); // Return fd
    int (*close)(int fd);

    // Read/write AT A POSITION, not using the unix style seek/read
    int (*read)(int fd, void *dest, uint32_t start, uint32_t n);
    int (*write)(int fd, void *src, uint32_t start, uint32_t n);

    int (*mkdir)(const char *dname, int flags);
    int (*getpath)(int fd, char *path);
};

#define DRIVE_ATTR_PRESENT  0x01
#define DRIVE_ATTR_WRITABLE 0x02

struct drive_t {
    uint16_t    phy_sect_size;
    uint16_t    log_sect_size;
    uint8_t     type;
    uint8_t     fs;
    uint8_t     attr;
    char        name[16];
};

#endif