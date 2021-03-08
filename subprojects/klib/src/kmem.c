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

char *kstrchr(const char *s, char c) {
    for (; *s && *s != c; s++);
    if (!*s) return NULL;
    return (char*)s;
}

char *kstrrchr(const char *s, char c) {
    //TODO: Implement this
    return (char*)s;
}

size_t kstrlen(const char *s) {
    size_t ret = 0;

    for (; *(s++); ret++);
    return ret;
}

int kstrcmp(const char *s1, const char *s2) {
    for (; *s1 || *s2; s1++, s2++) {
        if (*s1 != *s2) {
            return *s1 - *s2;
        }
    }
    return 0;
}