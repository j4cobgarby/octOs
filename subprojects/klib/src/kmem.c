#include "klib.h"

void *kmemset(void *ptr, char c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char*)ptr)[i] = c;
    }

    return ptr;
}

void *kmemcpy(void *dest, void *src, size_t n) {
    for (size_t i = 0; i < n; i++) {
        ((char*)dest)[i] = ((char*)src)[i];
    }

    return dest;
}