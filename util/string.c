#include <string.h>
#include <stdint.h>

void *memset(void *s, int c, size_t n) {
	for (;n>0;n--) {
		((uint8_t *) s)[n] = c;
	}
	return s;
}
