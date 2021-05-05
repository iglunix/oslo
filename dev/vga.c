#include <stdint.h>
#include "vga.h"

#define VGA_BUF ((uint16_t *) 0xb8000)
#define WIDTH 80
#define HEIGHT 25

static uint8_t row;
static uint8_t col;

int vga_init() {
	row = 0;
	col = 0;
	for (size_t i = 0; i < WIDTH; i++) {
		for (size_t j = 0; j < HEIGHT; j++) {
			VGA_BUF[i + j * WIDTH] = ' ' | 0x0000;
		}
	}
	return 0;
}

void vga_putchar(char c) {
	switch (c) {
		case '\n':
			col = 0;
			row++;
			break;

		default:
			VGA_BUF[col + row * WIDTH] = c | 0x0f00;
			col++;
			break;
	}
}

void vga_puts(char const *s) {
	for (;*s;s++) {
		vga_putchar(*s);
	}
	vga_putchar('\n');
}

void vga_pretty_byte(uint8_t v) {
	char map[] = {
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};
	vga_putchar(map[v >> 4]);
	vga_putchar(map[v & 0x0f]);
}

void vga_pretty_int(int32_t n) {
	int32_t rem = n;
	if (rem < 0) {
		rem *= -1;
		vga_putchar('-');
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
	for (i--;digits[i] >= '0' && digits[i] <= '9';i--) vga_putchar(digits[i]);
}

void vga_pretty_u32(uint32_t i) {
	vga_pretty_byte(i >> 24);
	vga_pretty_byte(i >> 16);
	vga_pretty_byte(i >> 8);
	vga_pretty_byte(i);
}

void vga_pretty_u16(uint16_t i) {
	vga_pretty_byte(i >> 8);
	vga_pretty_byte(i);
}
