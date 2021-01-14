#include "idt.h"
#include "kio.h"

__attribute__ ((interrupt))
void exception_handler_DE(struct interrupt_frame *frame) {
    kio_printf("Div by zero at %x :(\n", frame->eip);

    while(1) {
        asm("hlt");
    };
}