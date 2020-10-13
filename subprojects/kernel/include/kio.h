// Kernel io

#ifndef KIO_H
#define KIO_H

#include <stdint.h>

#define KIO_VMEM 0xb8000
#define KIO_ROWS 25
#define KIO_COLS 80
#define KIO_DIRECTION_UP    0b0001
#define KIO_DIRECTION_DOWN  0b0010
#define KIO_DIRECTION_LEFT  0b0100
#define KIO_DIRECTION_RIGHT 0b1000
#define KIO_ATTR_DEFAULT    0x17
#define KIO_ATTR_ERROR      0xcf
#define KIO_ATTR_WARNING    0xef
#define KIO_SCROLL_UP       1
#define KIO_SCROLL_DOWN     0

#define KIO_ERRORMSG(msg)   kio_puts_attr("\n* ERROR *", KIO_ATTR_ERROR); \
                            kio_putc(' '); kio_puts(msg);

#define KIO_ERRORWARNING(msg)   kio_puts_attr("\n* WARN *", KIO_ATTR_WARNING); \
                                kio_putc(' '); kio_puts(msg);

#define KIO_SETCHARAT(row, col, c) *(char*)(KIO_VMEM + 2 * (KIO_COLS * row + col)) = c;
#define KIO_SETATTRAT(row, col, a) *(char*)(1 + KIO_VMEM + 2 * (KIO_COLS * row + col)) = a;
#define KIO_SETCHAR(c) *(char*)(KIO_VMEM + 2 * (KIO_COLS * kio_row + kio_col)) = c;
#define KIO_SETATTR(a) *(char*)(1 + KIO_VMEM + 2 * (KIO_COLS * kio_row + kio_col)) = a;

extern uint16_t kio_row;
extern uint16_t kio_col;

extern const char* kio_hexdigits;

void kio_init(void);
void kio_cls(void); // Clear screen
void kio_scroll(int dir); // KIO_SCROLL_UP or KIO_SCROLL_DOWN
void kio_puts(const char* s); // Print null-terminated string s to screen
void kio_puts_attr(const char* s, const char attr); // puts with attribute
void kio_putc(const char c); // Print char c to screen
void kio_putc_attr(const char c, const char attr); // putc with attribute
void kio_puthex(uint32_t n); // Print n to screen, represented as hex
void kio_putbin(uint32_t n); // Print n to screen, represented as binary
void kio_putdec(uint32_t n); // Print n to screen, represented as decimal
void kio_move(const uint8_t dir); // Move the cursor. dir is 0 or more KIO_DIRECTION's or'd together.
void kio_setcurspos(uint16_t col, uint16_t row); // Set the visual cursor's position
void kio_updatecurs(void);
uint16_t kio_getcurspos(void);

#endif