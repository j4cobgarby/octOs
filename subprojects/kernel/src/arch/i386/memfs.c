#include "fs/memfs.h"
#include "fs/virtfs.h"
#include "klib.h"

struct memfs_node_t *memfs_root_node;

void memfs_init() {
    struct filesystemdescriptor_t fsd;
    fsd.open = &memfs_open;
    fsd.close = &memfs_close;
    fsd.read = &memfs_read;
    fsd.write = &memfs_write;
    fsd.mkdir = &memfs_mkdir;
    fsd.getpath = &memfs_getpath;
    fsd.rmdir = &memfs_rmdir;
    fsd.rmfile = &memfs_rmfile;
    kmemcpy(fsd.name, "MEMFS", 6);

    register_filesystem(fsd);

    memfs_root_node = kmalloc(sizeof(struct memfs_node_t));
    memfs_root_node->node_type = MEMFS_NODETYPE_DIR;
    kmemcpy(memfs_root_node->name, "ROOTDIR", 8);
    memfs_root_node->u.dir.first_in_dir = NULL;

    kio_printf("[MEMFS] Initialised.\n");
}

int memfs_open(const char *path, int flags) {
    int new_ifd = get_free_ifd();
    int drivenum;

    if (new_ifd >= 0) {
        path = vfs_parse_path(&drivenum, path);

        // Trying to open a file which doesn't exist
        struct memfs_node_t *node = memfs_getnode(path);
        if (!node) return -1;
        if (node->node_type != MEMFS_NODETYPE_FILE) return -1;

        ifdtable[new_ifd].fsdat = node;
        ifdtable[new_ifd].pres = 1;
        ifdtable[new_ifd].attr = flags;
        ifdtable[new_ifd].drive = drivenum;
        kmemcpy(ifdtable[new_ifd].path, (char*)path, kstrlen(path));
    }
    return new_ifd;
}

int memfs_close(int fd) {
    if (!ifdtable[fd].pres) {
        return -1;
    } else {
        ifdtable[fd].pres = 0;
        return 0;
    }
}

int memfs_read(int fd, void *dest, uint32_t start, uint32_t n) {
    if (!ifdtable[fd].pres) return -1;
    if (!(ifdtable[fd].attr & FILE_READABLE)) return -1;

    struct intern_fd_t ifd = ifdtable[fd];
    struct memfs_node_t *fnode = ifd.fsdat;
    char *data = fnode->u.file.data;

    if (start+n < (uint32_t)fnode->u.file.length) {
        kmemcpy(dest, data+start, n);
        return n;
    }

    return -1;
}

int memfs_write(int fd, void *src, uint32_t start, uint32_t n) {
    if (!ifdtable[fd].pres) return -1;
    if (!(ifdtable[fd].attr & FILE_WRITABLE)) return -1;

    struct intern_fd_t ifd = ifdtable[fd];
    struct memfs_node_t *fnode = ifd.fsdat;
    char *data = fnode->u.file.data;

    if (start+n < (uint32_t)fnode->u.file.length) {
        kmemcpy(data+start, src, n);
        return n;
    }
}

int memfs_mkdir(const char *path, int flags) {

}

int memfs_getpath(int fd, char *path) {

}

int memfs_rmdir(const char *path) {

}

int memfs_rmfile(const char *path) {

}


struct memfs_node_t *memfs_getnode(const char *path) {
    // path is like "/dir1/dir2/dir3/file.txt"

    struct memfs_node_t *current_node = memfs_root_node;

    // skip slashes before path names
    //while (path[0] == '/') path++;

    const char *next_slash;
    // While there is another slash in the string...
    while ((next_slash = kstrchr(path, '/'))) {
        // If trying to get files inside another file (e.g. /myfile.txt/otherthing), that's not allowed
        if (current_node->node_type == MEMFS_NODETYPE_FILE) return NULL;

        size_t sect_len = next_slash - path;

        if (sect_len > 0) { // a//b/c == a/b/c
            struct memfs_node_t *node_in_dir = current_node->u.dir.first_in_dir;
            int found = 0;

            // Find the node in the current directory of the target name
            while (node_in_dir) {
                if (kstrlen(node_in_dir->name) == sect_len && kstrncmp(path, node_in_dir->name, sect_len) == 0) {
                    current_node = node_in_dir;
                    found = 1;
                    break;
                }
                node_in_dir = node_in_dir->next_node;
            }

            if (!found) return NULL;
        }

        path = next_slash + 1;
    }

    if (current_node->node_type == MEMFS_NODETYPE_FILE) {
        // If at this point the current node is a file, that means that the full path was something like
        // "xxx/xxx/xxx/FILE/xxx", which is of course not valid.
        return NULL;
    }

    // Now at this point, we've parsed up to the last slash, so take the rest of the path string as the name of the
    // last node
    struct memfs_node_t *node_checking = current_node->u.dir.first_in_dir;
    size_t pathlen = kstrlen(path);
    if (pathlen > 0) {
        int found = 0;
        while (node_checking) {
            if (kstrlen(node_checking->name) == pathlen && kstrncmp(path, node_checking->name, pathlen) == 0) {
                current_node = node_checking;
                found = 1;
                break;
            }
            node_checking = node_checking->next_node;
        }
        if (!found) return NULL;
    }

    return current_node;
}

void memfs_append_node_to_dir(struct memfs_node_t *dir, struct memfs_node_t *node) {
    struct memfs_node_t *last_in_dir = dir->u.dir.first_in_dir;

    if (last_in_dir == NULL) {
        dir->u.dir.first_in_dir = node;
        node->prev_node = NULL;
        node->next_node = NULL;
        node->parent = dir;
    } else {
        while (last_in_dir->next_node) last_in_dir = last_in_dir->next_node;

        last_in_dir->next_node = node;
        node->prev_node = last_in_dir;
        node->next_node = NULL;
        node->parent = dir;
    }
}

void memfs_remove_node(struct memfs_node_t *node) {
    if (node->parent == NULL) {
        return;
    }

    if (node->prev_node == NULL && node->next_node == NULL) {
        node->parent->u.dir.first_in_dir = NULL;
    } else {
        if (node->prev_node) {
            node->prev_node->next_node = node->next_node;
        }
        if (node->prev_node) {
            node->next_node->prev_node = node->prev_node;
        }
    }
}
