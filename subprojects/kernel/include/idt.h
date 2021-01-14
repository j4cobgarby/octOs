#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_TYPE_TASK32 0x5
#define IDT_TYPE_INTR16 0x6
#define IDT_TYPE_TRAP16 0x7
#define IDT_TYPE_INTR32 0xe
#define IDT_TYPE_TRAP32 0xf

#define IDT_VEC_DE 0
#define IDT_VEC_GP 13
#define IDT_VEC_PF 14

struct __attribute__((__packed__)) idt_entry_t  {
    uint16_t pointer_lo; // bits 0-15 of address of code
    uint16_t codeselector;
    uint8_t  unused; // set to 0
    uint8_t  type : 4;
    uint8_t  s : 1; // storage segment
    uint8_t  dpl : 2; // privelage level needed to call this
    uint8_t  pres : 1; // set to 1 for present
    uint16_t pointer_hi; // bits 16-31 of address
};

struct interrupt_frame_t {
    uint32_t eip;
    uint32_t ecs;
    uint32_t flags;
};

extern struct idt_entry_t _idt_start[];

void register_exception_handler(uint8_t index, uint32_t address);

void register_exceptions();

// Handler for div by zero fault
__attribute__ ((interrupt))
void exception_handler_DE(struct interrupt_frame_t *frame);

__attribute__ ((interrupt))
void exception_handler_GP(struct interrupt_frame_t *frame, 
    unsigned int errorcode);

__attribute__ ((interrupt))
void exception_handler_PF(struct interrupt_frame_t *frame, 
    unsigned int errorcode);

#endif