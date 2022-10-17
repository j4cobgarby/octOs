#include "fs/fat16.h"
#include "fs/virtfs.h"
#include "kio.h"
#include "klib.h"

#define MAX_MOUNTS 32

struct fat16_mount_t mounts[MAX_MOUNTS];

uint16_t fat16_read_fat_entry(uint16_t *fattable, uint32_t i) {
    
}

struct fat16_mount_t *find_mount(int drvn) {
    for (int i = 0; i < MAX_MOUNTS; i++) {
        if (mounts[i].present && mounts[i].drvn == drvn) {
            return &(mounts[i]);
        }
    }
    return NULL;
}

/* Loads the entire FAT of a given drive into fattable. 
    This allocates memory for fattable. Make sure not to
    allocate anything for it before calling.
*/
int load_fat(uint8_t drv, uint16_t *fattable) {
    struct fat16_mount_t *mntparams = find_mount(drv);
    if (!mntparams) {
        kio_printf("[FAT16] Drive %d doesn't exist or isn't FAT16.\n", drv);
        return -1;
    }
    uint16_t fat_sector = mntparams->bpb.resvd_sects;
    uint16_t fat_sectors = mntparams->bpb.sect_per_fat;
    
}

struct fat16_dir_entry_t *fat16_find_dir_entry(int fd) {

}

void fat16_init() {
    struct filesystemdescriptor_t fsd;
    fsd.open = &fat16_open;
    fsd.close = &fat16_close;
    fsd.read = &fat16_read;
    fsd.write = &fat16_write;
    fsd.mkdir = &fat16_mkdir;
    fsd.getpath = &fat16_getpath;
    fsd.rmdir = &fat16_rmdir;
    fsd.rmfile = &fat16_rmfile;
    fsd.mount = &fat16_mount;
    kmemcpy(fsd.name, "FAT16", 6); // Copy the name (and null terminator)
    int n = register_filesystem(fsd);

    kio_printf("[FAT16] Initialised: %d\n", n);
}

/*
`path` will be of the form drive:path_in_drive, so it's known which drive the
file is on. The function will look into the drive table and find out which
drive type is present on that drive.
*/
int fat16_open(const char *path, int flags) {
    char *path_in_drive = kstrchr(path, ':');
    int drive_num;
    int fd;

    if (path_in_drive == NULL) {
        kio_printf("Path was invalid format.\n");
        return -1;
    }

    (path_in_drive++)[0] = 0;
    drive_num = katoi(path);

    kio_printf("[FAT16] Opening file within drive %d\n", drive_num);
    struct fat16_mount_t *mntparams = find_mount(drive_num);
    if (!mntparams) {
        kio_printf("[FAT16] Drive %d doesn't exist or isn't FAT16.\n", drive_num);
        return -1;
    }

    fd = set_ifd(0, drive_num, path_in_drive);

    ifdtable[fd].fsdat = kmalloc(sizeof(struct fat16_dir_entry_t));

    kio_printf("[FAT16] Opened file (fd/drvn/path): %d/%d/%s\n", fd, drive_num, path_in_drive);
    return 0;
}

int fat16_close(int fd) {
    kio_printf("Closed file %d\n", fd);
    return del_ifd(fd);
}

int fat16_read(int fd, void *dest, uint32_t start, uint32_t n) {
    kio_printf("[FAT16] Reading file %d\n", fd);
    fat16_find_dir_entry(fd);

    // 1) Determine which physical drive type this file is on
    // 2) Starting at the root directory, 

    return 0;
}

int fat16_write(int fd, void *src, uint32_t start, uint32_t n) {
    kio_printf("Writing to file %d\n", fd);
    return 0;
}

int fat16_mkdir(const char *path, int flags) {
    kio_printf("Making new directory %s\n", path);
    return 0;
}

int fat16_getpath(int fd, char *path) {
    kio_printf("Getting the path to file %d\n");
    return 0;
}

int fat16_rmdir(const char *path) {
    kio_printf("Removing directory at %s\n", path);
    return 0;
}

int fat16_rmfile(const char *path) {
    kio_printf("Removing file %s\n", path);
    return 0;
}

/* To mount a drive with this filesystem, we read the BPB and EBR
    data structures which are in the first sector of the drive.
    These store useful data about the device, which are needed by
    the rest of the filesystem.
*/
int fat16_mount(int drive) {
    int i;
    for (i = 0; i < MAX_MOUNTS && mounts[i].present; i++);
    if (i >= MAX_MOUNTS) {
        kio_printf("[FAT16] Cannot mount drive %d because there are too many FAT16 drives mounted.\n", drive);
        return -1;
    }
    kio_printf("[FAT16] Chosen mount number %d\n", i);

    kio_printf("[FAT16] Mounting drive %d with FAT16.\n", drive);

    struct fat16_mount_t mnt;
    mnt.present = 1;
    mnt.drvn = drive;
    struct drive_t the_drive = drivetable[drive];
    char *tmp = kmalloc(drivetypetable[the_drive.type].bytespersector);

    // This 1024 is the beginning sector to read from. This is currently 1024 because
    // of how the disk I'm testing with is partitioned.
    // TODO: Make the vfs system handle partitioned disks properly.
    drivetypetable[the_drive.type].rdsect(1024, 1, tmp, the_drive.drive_param);
    

    kmemcpy(&(mnt.bpb), tmp + 0x0b, sizeof(struct fat16_bpb_t));
    kmemcpy(&(mnt.ebr), tmp + 0x0b + sizeof(struct fat16_bpb_t), sizeof(struct fat16_ebr_t));
    mounts[i] = mnt;

    kfree(tmp);

    return 0;
}