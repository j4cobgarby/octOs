#ifndef FS_MEMFS_H
#define FS_MEMFS_H

#include <stdint.h>
#include "kio.h"
#include "fs/virtfs.h"

#define MEMFS_NODETYPE_DIR 0
#define MEMFS_NODETYPE_FILE 1

#define MEMFS_NAMELEN 16

struct memfs_node_t {
    int node_type;
    char name[MEMFS_NAMELEN];
    struct memfs_node_t *next_node;
    struct memfs_node_t *prev_node;
    struct memfs_node_t *parent;

    union {
        struct {
            struct memfs_node_t *first_in_dir;
        } dir;

        struct {
            char *data;
            int length;
        } file;
    } u;
};

extern struct memfs_node_t *memfs_root_node;

int memfs_open(const char *path, int flags);
int memfs_close(int fd);
int memfs_read(int fd, void *dest, uint32_t start, uint32_t n);
int memfs_write(int fd, void *src, uint32_t start, uint32_t n);
int memfs_mkdir(const char *path, int flags);
int memfs_getpath(int fd, char *path);
int memfs_rmdir(const char *path);
int memfs_rmfile(const char *path);

void memfs_init();
struct memfs_node_t *memfs_getnode(const char *path);
void memfs_append_node_to_dir(struct memfs_node_t *dir, struct memfs_node_t *node);
void memfs_remove_node(struct memfs_node_t *node);

#endif