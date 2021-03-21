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
	return 0;
}


struct fat_t fat_new(size_t offset) {
	fat_t ret = {
		/* - 0x200 because 0x1000 is start of second sector */
		.buf = (fat_bs_t *) ((uint8_t *) 0x1000 + offset - 0x200)
	};
	return ret;
}

void fat_dump(fat_t const *self, vga_t *vga, size_t start, size_t count) {
	for (size_t i = start; i < count; i++) {
		if ((i - start) % 16 == 0) {
			vga_putc(vga, '\n');
		} else {
			if ((i - start) % 4 == 0) {
				vga_putc(vga, ' ');
			}
			vga_putc(vga, ' ');
		}
		vga_pretty_byte(vga, ((uint8_t *)self->buf)[i]);
	}
}

void fat_oem(fat_t const *self, vga_t *vga) {
	for (size_t i = 0; i < 8; i++) {
		vga_putc(vga, self->buf->oem[i]);
//		vga_pretty_byte(vga, self->buf->oem[i]);
	}
	vga_putc(vga, '\n');
}

void fat_ls(fat_t const *self, vga_t *vga) {
	uint16_t reserved = self->buf->reserved_sector_count;
	uint8_t table_count = self->buf->tbl_count;
	uint32_t fat_tbl_size = self->buf->tbl_size;
	size_t root_offset = reserved + table_count * fat_tbl_size;

	vga_pretty_byte(vga, root_offset>>24);
	vga_pretty_byte(vga, root_offset>>16);
	vga_pretty_byte(vga, root_offset>>8);
	vga_pretty_byte(vga, root_offset);
	
//	size_t root_offset = fat_boot->reserved_sector_count + (fat_boot->table_count * fat_size);
}

/*

uint16_t fat_sector_size(fat_t const *self) {
	uint16_t ret = 0;
	ret = self->buf[0x0B];
	ret |= self->buf[0x0B + 1] << 8;
	return ret;
}

uint8_t fat_cluster_size(fat_t const *self) {
	return self->buf[0x0D];
}

uint16_t fat_reserved_count(fat_t const *self) {
	uint16_t ret = 0;
	ret = self->buf[0x0E];
	ret |= self->buf[0x0E + 1] << 8;
	return ret;
}

uint16_t fat_dirent_count(fat_t const *self) {
	uint16_t ret = 0;
	ret = self->buf[0x11];
	ret |= self->buf[0x11 + 1] << 8;
	return ret;
}

uint8_t fat_table_count(fat_t const *self) {
	return self->buf[0x10];
}

uint32_t fat_size(fat_t const *self) {
	return (*(uint16_t *)(self->buf + 0x13) == 0)? *(uint32_t *)(self->buf + 0x20) : *(uint16_t *)(self->buf + 0x13);
}


*/
