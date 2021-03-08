// Kernel io

#ifndef KIO_H
#define KIO_H

#include <stdint.h>
#include <stdarg.h>

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#define VGA_COL_BLACK       0x0
#define VGA_COL_BLUE        0x1
#define VGA_COL_GREEN       0x2
#define VGA_COL_CYAN        0x3
#define VGA_COL_RED         0x4
#define VGA_COL_MAGENTA     0x5
#define VGA_COL_BROWN       0x6
#define VGA_COL_LIGHTGREY   0x7
#define VGA_COL_DARKGREY    0x8
#define VGA_COL_LIGHTBLUE   0x9
#define VGA_COL_LIGHTGREEN  0xa
#define VGA_COL_LIGHTCYAN   0xb
#define VGA_COL_LIGHTRED    0xc
#define VGA_COL_LIGHTMAGENTA 0xd
#define VGA_COL_LIGHTBROWN  0xe
#define VGA_COL_WHITE       0xf

#define KIO_VMEM 0xb8000
#define KIO_ROWS 25
#define KIO_COLS 80
#define KIO_DIRECTION_UP    0b0001
#define KIO_DIRECTION_DOWN  0b0010
#define KIO_DIRECTION_LEFT  0b0100
#define KIO_DIRECTION_RIGHT 0b1000
#define KIO_ATTR_DEFAULT    VGA_COL_BLACK << 4 | VGA_COL_WHITE
#define KIO_ATTR_ERROR      VGA_COL_BLACK << 4 | VGA_COL_RED
#define KIO_ATTR_WARNING    VGA_COL_BLACK << 4 | VGA_COL_LIGHTBROWN
#define KIO_ATTR_SUBSYS     VGA_COL_BLACK << 4 | VGA_COL_MAGENTA
#define KIO_SCROLL_UP       1
#define KIO_SCROLL_DOWN     0

#define KIO_ERRORMSG(msg)   kio_puts_attr("[ERROR]", KIO_ATTR_ERROR); \
                            kio_putc(' '); kio_puts(msg);

#define KIO_ERRORWARNING(msg)   kio_puts_attr("[WARNING]", KIO_ATTR_WARNING); \
                                kio_putc(' '); kio_puts(msg);

#define KIO_SUBSYSTEM_MSG(subsys, msg)  kio_puts_attr(subsys, KIO_ATTR_SUBSYS); \
                                        kio_printf(" %s", msg);

#define KIO_SETCHARAT(row, col, c) *(char*)(KIO_VMEM + 2 * (KIO_COLS * row + col)) = c;
#define KIO_SETATTRAT(row, col, a) *(char*)(1 + KIO_VMEM + 2 * (KIO_COLS * row + col)) = a;
#define KIO_SETCHAR(c) *(char*)(KIO_VMEM + 2 * (KIO_COLS * kio_row + kio_col)) = c;
#define KIO_SETATTR(a) *(char*)(1 + KIO_VMEM + 2 * (KIO_COLS * kio_row + kio_col)) = a;

#define KIO_PUTDEC_MAX_DIGITS 10

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
void kio_putdec(int32_t n);
// Print a formatted string
void kio_printf(char *fmt, ...);
// Move the cursor. dir is 0 or more KIO_DIRECTION's or'd together.
void kio_move(const uint8_t dir); 
// Set the visual cursor's position
void kio_setcurspos(uint16_t col, uint16_t row); 
void kio_updatecurs(void);
uint16_t kio_getcurspos(void);

#endif