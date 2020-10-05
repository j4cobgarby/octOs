#include "kio.h"
#include "asm_procs.h"

const char* kio_hexdigits = "0123456789abcdef";

uint16_t kio_row = 0;
uint16_t kio_col = 0;

void kio_init(void) {
    // enable vga cursor
    outb(0x3d4, 0x0a);
    outb(0x3d5, (inb(0x3d5) & 0xc0) | 0);

    outb(0x3d4, 0x0b);
    outb(0x3d5, (inb(0x3d5) & 0xe0) | 15);
}

kio_cls(void) {
    for (uint8_t row = 0; row < KIO_ROWS; row++) {
        for (uint8_t col = 0; col < KIO_COLS; col++) {
            KIO_SETCHARAT(row, col, ' ');
            KIO_SETATTRAT(row, col, KIO_DEFAULT_ATTR);
        }
    }
}

void kio_puts(const char* s) {
    kio_puts_attr(s, KIO_DEFAULT_ATTR);
}

void kio_puts_attr(const char* s, const char attr) {
    for (; *s; s++) kio_putc_attr(*s, attr);
    kio_updatecurs();
}

void kio_putc(const char c) {
    kio_putc_attr(c, KIO_DEFAULT_ATTR);
}

void kio_putc_attr(const char c, const char a) {
    if (kio_col < KIO_COLS) {
        if (c == '\n') {
            kio_move(KIO_DIRECTION_DOWN);
            kio_col = 0;
        } else {
            KIO_SETCHAR(c);
            KIO_SETATTR(a);
            kio_move(KIO_DIRECTION_RIGHT);
        }
    } else {
        kio_move(KIO_DIRECTION_DOWN);
        kio_col = 0;
        KIO_SETCHAR(c);
        KIO_SETATTR(a);
    }
}

void kio_move(const uint8_t dir) {
    if (dir & KIO_DIRECTION_UP) {
        if (kio_row > 0) kio_row--;
    }
    if (dir & KIO_DIRECTION_DOWN) {
        if (kio_row < KIO_ROWS - 1) kio_row++;
    }
    if (dir & KIO_DIRECTION_LEFT) {
        if (kio_col <= 0) {
            kio_move(KIO_DIRECTION_UP);
            kio_col = 0;
        } else {
            kio_col--;
        }
    }
    if (dir & KIO_DIRECTION_RIGHT) {
        if (kio_col >= KIO_COLS - 1) {
            kio_move(KIO_DIRECTION_DOWN);
            kio_col = 0;
        } else {
            kio_col++;
        }
    }
}

void kio_puthex(uint32_t n) {
    kio_puts("0x");
    for (int i = 7; i >= 0; i--) {
        uint32_t mask = 0xf << (i*4);
        kio_putc(kio_hexdigits[(n & mask) >> (i*4)]);
    }
}

void kio_setcurspos(uint16_t col, uint16_t row) {
    uint16_t pos = row * KIO_COLS + col;
    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t)(pos & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t)((pos>>8) & 0xff));
}

void kio_updatecurs(void) {
    kio_setcurspos(kio_col, kio_row);
}

uint16_t kio_getcurspos(void) {
    uint16_t ret = 0;

    outb(0x3d4, 0x0f);
    ret |= inb(0x3d5);
    outb(0x3d4, 0x0e);
    ret |= ((uint16_t)inb(0x3d5)) << 8;

    return ret;
}