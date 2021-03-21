#ifndef vga_h_INCLUDED
#define vga_h_INCLUDED

#include <stdint.h>
#include <stddef.h>

typedef struct vga_t {
	size_t line;
	size_t col;
	uint16_t *buf;
} vga_t;

vga_t vga_init();
void vga_putc(vga_t *, char);
void vga_puts(vga_t *, char const *);
void vga_pretty_byte(vga_t *, uint8_t);

#endif // vga_h_INCLUDED

