
/* A specific volume on a drive. This could be an entire drive
    or a partition on a drive.
    A volume has an associated filesystem.
*/
struct vfs_volume_t {
    struct vfs_drive_t *drive;
    
}

/* An instance of a filesystem
*/
struct vfs_fs_t {

}

/* A type of filesystem, like FAT16 or ext4.
*/
struct vfs_fstype_t {
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
}

/* A specific drive that can be attached to the computer.
    A drive can contain multiple volumes, each with different
    filesystems.
*/
struct vfs_drive_t {
    
}

/* A medium is a type of drive attached to the computer.
    Typical examples of this might be ATA or optical drives. 
*/
struct vfs_medium_t {

}