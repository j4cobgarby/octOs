#include <stdint.h>

#define PSTATE_BLOCKED 0
#define PSTATE_RUNNABLE 1
#define PSTATE_RUNNING 2

typedef struct {
    // General purpose regs
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    // Memory segment regs
    uint16_t cs;
    uint16_t ss;
    uint16_t ds;
    uint16_t ss;

    uint32_t eip;
    uint32_t esp;
    uint32_t ebp;

    // Process info
    uint8_t state; // PSTATE_xxxx
    uint16_t pid;
    uint16_t parent_pid;
} ptable_entry_t;