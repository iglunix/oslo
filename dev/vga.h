#ifndef vga_h_INCLUDED
#define vga_h_INCLUDED

#include <stdint.h>
#include <stddef.h>

typedef struct vga_t {
	size_t row;
	size_t col;
} vga_t;

int vga_init();
void vga_clear();
void vga_putchar(char);
void vga_puts(char const *);
void vga_pretty_u8(uint8_t);
void vga_pretty_u32(uint32_t);
void vga_pretty_u16(uint16_t);
void vga_printf(char const *fmt, ...) __attribute__((format (printf, 1, 2)));

#endif // vga_h_INCLUDED

