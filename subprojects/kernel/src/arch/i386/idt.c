#include "idt.h"

#include "kio.h"

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
    register_exception_handler(0, (uint32_t)&exception_handler_DE);

    kio_putdec(10 / 0);
}