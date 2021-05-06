#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
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
	if (row >= 80) return;
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

void vga_pretty_u8(uint8_t v) {
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
	vga_pretty_u8(i >> 24);
	vga_pretty_u8(i >> 16);
	vga_pretty_u8(i >> 8);
	vga_pretty_u8(i);
}

void vga_pretty_u16(uint16_t i) {
	vga_pretty_u8(i >> 8);
	vga_pretty_u8(i);
}

typedef enum printf_len_mod_t {
	/* uint8_t */
	PFL_HH = 0,
	/* uint16_t */
	PFL_H = 1,
	/* uint32_t */
	PFL_L = 2,
	/* uint64_t */
	PFL_LL = 3,
} printf_len_mod_t;

static printf_len_mod_t printf_parse_len_mod_t(char const **s) {
	switch (**s) {
		case 'l':
			++*s;
			if (**s == 'l') {
				++*s;
				return PFL_LL;
			} else {
				return PFL_L;
			}

		case 'h':
			++*s;
			if (**s == 'h') {
				++*s;
				return PFL_HH;
			} else {
				return PFL_H;
			}

		default:
			return PFL_L;
	}
};

void vga_printf(char const *s, ...) {
	va_list va;
	va_start(va, s);
	for (;*s;) {
		switch (*s) {
			case '%':
				s++;
				switch (*s) {
					case 'i':
					case 'd':
						;
						int i = va_arg(va, int);
						vga_pretty_int(i);
						s++;
						break;
					case 's':
						;
						char const *str = va_arg(va, char const *);
						for (;*str;str++) vga_putchar(*str);
						s++;
						break;
					case 'X':
					case 'x':
						;
						uint32_t h = va_arg(va, uint32_t);
						s++;
						switch (printf_parse_len_mod_t(&s)) {
							case PFL_HH:
								vga_pretty_u8(h);
								break;
							case PFL_H:
								vga_pretty_u16(h);
								break;
							case PFL_L:
								vga_pretty_u32(h);
								break;
							case PFL_LL:
								break;
						}
					default:
						break;
				}
				break;

			default:
				vga_putchar(*s);
				s++;
				break;
		}
	}
	va_end(va);
}
