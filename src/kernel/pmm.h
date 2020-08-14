#ifndef PMM_H
#define PMM_H

#include <stdint.h>

#define PMM_BLOCKSIZE 4096

extern void _kernel_start(void);
extern void _kernel_end(void);

void pmm_init(uint32_t* mboot_info, uint8_t* bitmap);

#endif