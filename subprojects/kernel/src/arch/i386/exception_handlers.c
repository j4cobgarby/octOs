#include "idt.h"
#include "kio.h"

__attribute__ ((interrupt))
void exception_handler_DE(struct interrupt_frame *frame) {
    asm("xchg %bx, %bx");
}