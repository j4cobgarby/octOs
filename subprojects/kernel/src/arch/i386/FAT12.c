#include "fs/FAT12.h"

uint16_t fat12_read_fat_entry(uint8_t *fattable, uint32_t i) {
    uint32_t fat_index = i + (i/2);
    uint16_t entry = *(uint16_t*)&fattable[fat_index];

    if (i & 0x0001) {
        entry >>= 4;
    } else {
        entry &= 0xfff;
    }

    return entry;
}