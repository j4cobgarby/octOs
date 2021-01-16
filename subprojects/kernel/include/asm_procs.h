#ifndef ASM_PROCS_H
#define ASM_PROCS_H

#include <stdint.h>

inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1"
                :
                : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0"
                : "=a"(val)
                : "Nd"(port));
    return val;
}

inline void outw(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1"
                :
                : "a"(val), "Nd"(port));
}
 
inline uint16_t inw(uint16_t port) {
    uint16_t val;
    asm volatile("inw %1, %0"
                : "=a"(val)
                : "Nd"(port));
    return val;
}

inline void outl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1"
                :
                : "a"(val), "Nd"(port));
}

inline uint32_t inl(uint16_t port) {
    uint32_t val;
    asm volatile("inl %1, %0"
                : "=a"(val)
                : "Nd"(port));
    return val;
}

#endif