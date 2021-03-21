#include "vga.h"

size_t VGA_WIDTH = 80;
size_t VGA_HEIGHT = 25;
uint16_t *vga_buf = (uint16_t *) 0xb8000;

vga_t vga_init() {
	for (size_t i = 0; i < VGA_WIDTH; i++) {
		for (size_t j = 0; j < VGA_HEIGHT; j++) {
			vga_buf[j * VGA_WIDTH + i] = 0;
		}
	}
	vga_t ret = {
		.line = 0,
		.col = 0,
		.buf = (uint16_t *) 0xb8000,
	};
	return ret;
}

void vga_putc(vga_t *self, char c) {
	if (c == '\n') {
		self->line++;
		self->col = 0;
	} else {
		self->buf[self->col + self->line * VGA_WIDTH] = 0x0300 | c;
		self->col++;
	}
}

void vga_puts(vga_t *self, char const *s) {
	for (;*s;s++) {
		vga_putc(self, *s);
	}
	vga_putc(self, '\n');
}

void vga_pretty_byte(vga_t *self, uint8_t v) {
	char map[] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};
	vga_putc(self, map[v >> 4]);
	vga_putc(self, map[v & 0x0f]);
}
