// Kernel io

#ifndef KIO_H
#define KIO_H

#include <stdint.h>
#include <stdarg.h>

#define KIO_VMEM 0xb8000
#define KIO_ROWS 25
#define KIO_COLS 80
#define KIO_DIRECTION_UP    0b0001
#define KIO_DIRECTION_DOWN  0b0010
#define KIO_DIRECTION_LEFT  0b0100
#define KIO_DIRECTION_RIGHT 0b1000
#define KIO_ATTR_DEFAULT    0x0f
#define KIO_ATTR_ERROR      0x04
#define KIO_ATTR_WARNING    0xee
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

// Initialise all the stuff needed for kernel io to work
void kio_init(void);
// Clear screen
void kio_cls(void); 
// KIO_SCROLL_UP or KIO_SCROLL_DOWN
void kio_scroll(int dir); 
// Print null-terminated string s to screen
void kio_puts(const char* s); 
// puts with attribute
void kio_puts_attr(const char* s, const char attr); 
// These are the same their counterparts without the '_n', but
// only printing a certain amount of characters.
void kio_puts_n(const char *s, uint32_t n);
void kio_puts_n_attr(const char *s, uint32_t n, const char attr);
// Print char c to screen
void kio_putc(const char c); 
// putc with attribute
void kio_putc_attr(const char c, const char attr); 
// Print n to screen, represented as hex
void kio_puthex(uint32_t n); 
// Print n to screen, represented as binary
void kio_putbin(uint32_t n); 
// Print n to screen as binary, only bits between lobit and hibit inclusive
void kio_putbin_bounds(uint32_t n, uint32_t lobit, uint32_t hibit);
// Print n to screen, represented as decimal
void kio_putdec(uint32_t n);
// Print a formatted string
void kio_printf(char *fmt, ...);
// Move the cursor. dir is 0 or more KIO_DIRECTION's or'd together.
void kio_move(const uint8_t dir); 
// Set the visual cursor's position
void kio_setcurspos(uint16_t col, uint16_t row); 
void kio_updatecurs(void);
uint16_t kio_getcurspos(void);

#endif