#ifndef FS_FAT12_H
#define FS_FAT12_H

#include <stdint.h>

#define FAT12_ATTR_READONLY     0x01
#define FAT12_ATTR_HIDDEN       0x02
#define FAT12_ATTR_SYSTEM       0x04
#define FAT12_ATTR_VOLUMELABEL  0x08
#define FAT12_ATTR_SUBDIRECTORY 0x10
#define FAT12_ATTR_ARCHIVE      0X20

// This structure will be present starting at address 0x0b in the first
// sector of a fat12 disk
struct __attribute__((__packed__)) fat12_bpb_t {
    uint16_t    bytes_per_sect;
    uint8_t     sect_per_clust;
    uint16_t    resvd_sects;
    uint8_t     number_of_fats;
    uint16_t    rootdir_entries;
    uint16_t    total_sects;
    uint8_t     media_desc;
    uint16_t    sect_per_fat;
    uint16_t    sect_per_track;
    uint16_t    number_heads;
    uint32_t    number_hidden_sects;
    uint32_t    largesect_count;
};

struct __attribute__((__packed__)) fat12_ebr_t {
    uint8_t     drive_number;
    uint8_t     resvd;
    uint8_t     sig; // Either 0x28 or 0x29
    uint32_t    serial;
    char        label[11];
    char        ident[8];
};

struct __attribute__((__packed__)) fat12_time_t {
    uint8_t secs : 5;
    uint8_t mins : 6;
    uint8_t hour : 5;
};

struct __attribute__((__packed__)) fat12_date_t {
    uint8_t day   : 5;
    uint8_t month : 4;
    uint8_t year  : 7;
};

struct __attribute__((__packed__)) fat12_dir_entry_t {
    char filename[8];
    char extension[3];
    uint8_t attr;
    uint16_t resv;
    struct fat12_time_t time_created;
    struct fat12_date_t date_created;
    struct fat12_date_t date_accessed;
    uint16_t resv2;
    struct fat12_time_t time_written;
    struct fat12_date_t  date_written;
    uint16_t cluster_start;
    uint32_t bytes;
};

uint16_t fat12_read_fat_entry(uint8_t *fattable, uint32_t i);

#endif