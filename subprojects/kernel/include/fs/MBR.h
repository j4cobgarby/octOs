#ifndef FS_MBR_H
#define FS_MBR_H

#include <stdint.h>

// A partiton table entry in the MBR.
// There can be up to 4 of these, at addresses:
// 0x1be, 0x1ce, 0x1de, 0x1ee
// in sector 0 of the disk.
// If the type of every one of these partitions is 0, then
// the disk isn't partitioned.
struct __attribute__((__packed__)) mbr_partition_t {
    uint8_t status;
    uint8_t chs_first[3];
    uint8_t type;
    uint8_t chs_last[3];
    uint32_t lba_first;
    uint32_t sect_count;
};

#endif