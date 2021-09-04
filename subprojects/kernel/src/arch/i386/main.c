#include "fs/memfs.h"
#include "kio.h"
#include "klib.h"
#include "ata_pio.h"
#include "fs/virtfs.h"
#include "fs/fat16.h"
#include "fs/memfs.h"

void kmain() {
    vfs_init();
    fat16_init();
    memfs_init();
    drivetypes_init();

    struct memfs_node_t *n1 = kmalloc(sizeof(struct memfs_node_t));

    n1->node_type = MEMFS_NODETYPE_FILE;
    kmemcpy(n1->name, "myfile.txt", 11);
    n1->u.file.length = 512;
    n1->u.file.data = kmalloc(512);
    kmemcpy(n1->u.file.data, "Hello, world!", 14);

    memfs_append_node_to_dir(memfs_root_node, n1);

    int drv = register_drive(-1, get_filesystem_index("MEMFS"), NULL);
    kio_printf("Drive ind: %d\n", drv);
    int myfile = vfs_open("1:/myfile.txt", FILE_WRITABLE | FILE_READABLE);
    kio_printf("File d: %d\n", myfile);
    char *buffer = kmalloc(20);
    vfs_write(myfile, "Written", 0, 7);
    vfs_read(myfile, buffer, 0, 13);
    buffer[13] = '\0';
    kio_printf("Data: %s\n", buffer);

    

    // int myfile = memfs_open("0:/myfile.txt", FILE_READABLE);
    // char *buffer = kmalloc(20);
    // kio_printf("Read: %d\n", memfs_read(myfile, buffer, 1, 4));
    // buffer[4] = '\0';
    // kio_printf("Data: %s\n", buffer);

    // int ata_id = get_drivetype_index("ATA");
}