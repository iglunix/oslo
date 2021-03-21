#include "disk.h"

struct mbr_t {
	struct {
		uint8_t status;
		/* Head Sector Track */
		uint8_t hst[3];
		uint8_t type;
		uint8_t end_hst[3];
	} entries[4];
};

struct mbr_t *cast_mbr(uint8_t *mbr) {

}


struct fat_t fat_new(size_t offset) {
	fat_t ret = {
		/* - 0x200 because 0x1000 is start of second sector */
		.buf = (uint8_t *) 0x1000 + offset - 0x200
	};
	return ret;
}


void fat_oem(fat_t *self, vga_t *vga) {
	char str[9];
	for (size_t i = 0; i < 8; i++) {
		str[i] = (self->buf + 0x03)[i];
	}
	str[8] = '\0';
	vga_puts(vga, str);
}
void fat_ls(fat_t *self, vga_t *vga) {
}
