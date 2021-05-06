#include "vga.h"
#include <stdarg.h>

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

void vga_pretty_int(vga_t *self, int32_t n) {
	int32_t rem = n;
	if (rem < 0) {
		rem *= -1;
		vga_putc(self, '-');
	}
	char digits[11];
	size_t i = 0;
	for (;rem > 0;i++) {
		digits[i] =  '0' + rem % 10;
		if (rem < 10) {
			rem = 0;
		} else {
			rem /= 10;
		}
	}
	for (i--;digits[i] >= '0' && digits[i] <= '9';i--) vga_putc(self, digits[i]);
}

void vga_pretty_hex(vga_t *self, uint32_t i) {
	vga_pretty_byte(self, i >> 24);
	vga_pretty_byte(self, i >> 16);
	vga_pretty_byte(self, i >> 8);
	vga_pretty_byte(self, i);
}

void vga_printf(vga_t *self, char const *s, ...) {
	va_list va;
	va_start(va, s);
	for (;*s;s++) {
		switch (*s) {
			case '%':
				s++;
				switch (*s) {
					case 'i':
					case 'd':
						;
						int i = va_arg(va, int);
						vga_pretty_int(self, i);
						break;
					case 's':
						;
						char const *str = va_arg(va, char const *);
						for (;*str;str++) vga_putc(self, *str);
						break;
					case 'X':
					case 'x':
						;
						uint32_t h = va_arg(va, uint32_t);
						vga_pretty_hex(self, h);
					default:
						break;
				}
				break;

			default:
				vga_putc(self, *s);
				break;
		}
	}
	va_end(va);
}
