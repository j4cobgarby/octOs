#include "idt.h"
#include "kio.h"

__attribute__ ((interrupt))
void exception_handler_DE(struct interrupt_frame_t *frame) {
#ifdef KERNEL_DEBUG
    kio_printf("Div by zero at %x :(\n", frame->eip);
#endif

    while (1) {
        asm("hlt");
    };
}

__attribute__ ((interrupt))
void exception_handler_GP(struct interrupt_frame_t *frame, 
        unsigned long errorcode) {
#ifdef KERNEL_DEBUG
    kio_printf("#GP Exceotion at %x (error %x)\n", frame->eip, errorcode);
#endif
}

__attribute__ ((interrupt))
void exception_handler_PF(struct interrupt_frame_t *frame, 
        unsigned long errorcode) {
#ifdef KERNEL_DEBUG
    kio_printf("Page fault encountered at %x (error %x)\n", frame->eip,
        errorcode);
#endif

    // TODO: Make this swap the page into memory from a swapfile
    // on a disk, once the disk driver works.

    while (1) {
        asm("hlt");
    }
}