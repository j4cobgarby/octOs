#include "fs/fat16.h"
#include "fs/virtfs.h"
#include "kio.h"
#include "klib.h"

uint16_t fat16_read_fat_entry(uint8_t *fattable, uint32_t i) {
    
}

struct fat16_dir_entry_t *fat16_find_dir_entry(int fd) {
    struct drive_t the_drive = drivetable[ifdtable[fd].drive];
    char tmp[512];
    struct fat16_bpb_t *bpb = tmp + 0x0b;
    struct fat16_ebr_t *ebr = tmp + 0x0b + sizeof(struct fat16_bpb_t);

    kio_printf("Reading dir entry\n");
    drivetypetable[the_drive.type].rdsect(1024, 1, tmp, the_drive.drive_param);
    kio_printf("Sig %x %x\n", tmp[0x01fe], tmp[0x1ff]);
    kio_printf("\
    Bytes per sect: %d\n\
    #FATS: %d\n\
    #Sects: %d\n\
    #Large sects: %d\n", 
    bpb->bytes_per_sect, bpb->number_of_fats, 
    bpb->total_sects, bpb->largesect_count);

    kio_printf("\
    Drive #: %d\n\
    Sig: %d\n",
    ebr->drive_number,
    ebr->sig);

    kio_puts_n(ebr->label, 11);
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

    fd = set_ifd(0, drive_num, path_in_drive);

    ifdtable[fd].fsdat = kmalloc(sizeof(struct fat16_dir_entry_t));
    //((struct fat16_dir_entry_t*)ifdtable[fd].fsdat)->

    kio_printf("[FAT16] Opened file (%d): %d : %s\n", fd, drive_num, path_in_drive);
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
