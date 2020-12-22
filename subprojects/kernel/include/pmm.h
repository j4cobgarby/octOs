#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PMM_BLOCKSIZE 4096
#define ADDR_BLOCK(addr) (addr >> 12) // Get block from address

uint32_t pmm_blocks_used;
uint32_t pmm_blocks_max;

extern void _kernel_start(void);
extern void _kernel_end(void);

void pmm_init(uint32_t* mboot_info);
void pmm_set(uint32_t block);
void pmm_sets(uint32_t startblock, uint32_t amount);
void pmm_unset(uint32_t block);
void pmm_unsets(uint32_t startblock, uint32_t amount);
uint8_t pmm_isset(uint32_t block);
void *pmm_alloc();
void pmm_free(void *);

#endif