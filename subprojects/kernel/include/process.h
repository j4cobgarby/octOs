#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define PROCTABLE_SIZE 64

enum proc_state_t {
    proc_blocked, proc_runnable, proc_running
};

struct proc_t {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;

    uint16_t cs;
    uint16_t ds;
    uint16_t ss;

    uint32_t eip;
    uint32_t esp;
    uint32_t ebp;

    enum proc_state_t state;
    uint16_t id;
    uint16_t parent;
    uint8_t priority;
};

struct proc_t proctable[PROCTABLE_SIZE];

void create_process(uint16_t id, uint16_t parent, uint8_t priority);
void switch_process(uint16_t id);

#endif