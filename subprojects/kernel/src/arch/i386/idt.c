#include "idt.h"

#include "kio.h"

#define IDT_DE 0
#define IDT_DB 1
#define IDT_NMI 2
#define IDT_BP 3
#define IDT_OF 4
#define IDT_BR 5
#define IDT_UD 6
#define IDT_NM 7
#define IDT_DF 8

void register_exception_handler(uint8_t index, uint32_t address) {
    _idt_start[index].unused = 0;
    _idt_start[index].codeselector = 0x8; // kernel code selector
    _idt_start[index].type = IDT_TYPE_TRAP32;
    _idt_start[index].s = 0;
    _idt_start[index].dpl = 0;
    _idt_start[index].pres = 1;
    _idt_start[index].pointer_lo = (uint16_t)address;
    _idt_start[index].pointer_hi = (uint16_t)(address >> 16);
}

void register_exceptions() {
    register_exception_handler(IDT_VEC_DE, (uint32_t)&exception_handler_DE);
    register_exception_handler(IDT_VEC_GP, (uint32_t)&exception_handler_GP);
    register_exception_handler(IDT_VEC_PF, (uint32_t)&exception_handler_GP);
}