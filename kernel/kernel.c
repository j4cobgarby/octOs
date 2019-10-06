#include "kernel.h"

#include <stdint.h>
#include <stddef.h>

void kernel_main() {
    uint16_t* vbuff = (uint16_t*) 0xb8000;
    vbuff[0] = 0xabcd;
    vbuff[1] = 0xefff;

    while (1) {}
}
