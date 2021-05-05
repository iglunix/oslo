#include <stdbool.h>
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

struct fat_t fat_new(size_t offset) {
	fat_t ret = {
		/* - 0x200 because 0x8000 is start of second sector */
		.buf = (fat_bs_t *) ((uint8_t *) 0x8000 + offset - 0x200)
	};
    ret.total_sectors = (ret.buf->total_sectors == 0)? ret.buf->total_sectors_32 : ret.buf->total_sectors;
	ret.fat_size = (ret.buf->tbl_size == 0)? ret.buf->ext.fat32.tbl_size : ret.buf->tbl_size;
	ret.root_dir_sectors = ((ret.buf->root_ent_count * 32) + (ret.buf->sector_size - 1)) / ret.buf->sector_size;
    ret.first_data_sector = ret.buf->reserved_sector_count + (ret.buf->tbl_count * ret.fat_size) + ret.root_dir_sectors;
	ret.first_fat_sector = ret.buf->reserved_sector_count;
	ret.data_sectors = ret.buf->total_sectors - (ret.buf->reserved_sector_count + (ret.buf->tbl_count * ret.fat_size) + ret.root_dir_sectors);
	ret.total_clusters = ret.data_sectors / ret.buf->sector_size;

	if (ret.total_clusters < 4085) {
		ret.type = FAT_12;
	} else if (ret.total_clusters < 65525) {
		ret.type = FAT_16;
	} else if (ret.total_clusters < 268435445) {
		ret.type = FAT_32;
	} else {
		ret.type = UNKNOWN; /*exfat*/
	}

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

/*void fat_lookup(fat_t const *self, vga_t *vga) {
//	uint8_t FAT_table[self->buf->sector_size];
	uint32_t fat_offset;
	uint32_t active_cluster = 0;
	switch (self->type) {
		case FAT_12:
			fat_offset = active_cluster + (active_cluster / 2);
			break;
		case FAT_16:
			fat_offset = active_cluster * 2;
			break;
		case FAT_32:
			fat_offset = active_cluster * 4;
			break;
		case UNKNOWN:
			vga_puts(vga, "Error: Unknown FAT Type");
			break;
	}
	uint32_t first_fat_sector = 0x08;
	uint32_t fat_sector = first_fat_sector + (fat_offset / self->buf->sector_size);
	uint32_t ent_offset = fat_offset % self->buf->sector_size;

	vga_pretty_byte(vga, ent_offset>>24);
	vga_pretty_byte(vga, ent_offset>>16);
	vga_pretty_byte(vga, ent_offset>>8);
	vga_pretty_byte(vga, ent_offset);
}*/

/*
 * TODO:
 */

typedef union fat_dir_t {
	uint8_t raw[32];
	struct {

	} data;
	struct {

	} long_name;
} __attribute__((packed)) fat_dir_t;

void fat_find(fat_t const *self, uint8_t **oslocfg, uint8_t **vmlinuz, uint8_t **initrd);

void fat_ls(fat_t const *self, vga_t *vga) {
	uint16_t reserved = self->buf->reserved_sector_count;
	uint8_t table_count = self->buf->tbl_count;
	uint32_t fat_tbl_size = self->buf->tbl_size;
	size_t root_offset = reserved + table_count * fat_tbl_size;

	uint32_t first_root_dir_sector = self->first_data_sector - self->root_dir_sectors;

	uint8_t *root_dir_buf = (uint8_t *) self->buf + root_offset * 512;

	vga_printf(vga, "fat_ls: %h", self->buf + root_offset * 512);
	vga_putc(vga, '\n');

	bool has_long_name = false;
	char long_name[32];

	for (size_t i = 0; root_dir_buf[i * 32]; i++) {
		/* test for unused */
		if (root_dir_buf[i * 32] != 0xe5) {
			/* check for long file names */
			if (root_dir_buf[i * 32 + 11] == 0x0f) {
				has_long_name = true;
				for (size_t j = 0; j < 5; j++) {
					uint16_t *char_buf = (uint16_t *)(root_dir_buf + i * 32 + 1);
					long_name[j] = char_buf[j];
				}
				for (size_t j = 0; j < 6; j++) {
					long_name[j + 5] = root_dir_buf[i * 32 + 14 + j * 2];
				}
				for (size_t j = 0; j < 2; j++) {
					long_name[j + 11] = root_dir_buf[i * 32 + 28 + j * 2];
				}
			} else if (has_long_name) {
				vga_printf(vga, "  %s\n", long_name);
				has_long_name = false;
			} else {
				vga_printf(vga, "  ");
				for (size_t j = 0; j < 11; j++)
					vga_putc(vga, root_dir_buf[i * 32 + j]);
				vga_putc(vga, '\n');
			}
			/* check if not volume id */
			if (!(root_dir_buf[i * 32 + 11] & 0x08)) {
				size_t offset = (self->first_data_sector + (root_dir_buf[i * 32 + 26] - 2) * self->buf->cluster_size) * 512;
				uint8_t *ptr = (uint8_t *)self->buf + offset;
				vga_printf(vga, "data: %h\n", offset);
				vga_puts(vga, (char const *)ptr);
			}
		}
	}
}

#define real_seg(x) ((uint16_t)(((int)x & 0xffff0) >> 4))
#define real_off(x) ((uint16_t)(((int)x & 0x0000f) >> 0))

void int13h_disk_read(vga_t *vga, uint8_t *dest) {
	vga_printf(vga, "%h:", (uint32_t) real_seg(0xf000));
	vga_printf(vga, "%h\n", (uint32_t) real_off(0xf000));

	asm(
		"push %esp\n"

		"ljmp $0x08,$b16 \n"
	".code16\n"
	"b16: \n"

		"mov $0x10,%ax\n"
		"mov %ax,%ds\n"
		"mov %ax,%es\n"

		"mov %cr0,%eax\n"
		"and $0xfe,%al\n"
		"mov %eax,%cr0\n"
	".code32\n"

		"ljmp $0x00,$cs0 \n"
	".code16\n"
	"cs0:"

		"xor %ax,%ax\n"
		"mov %ax,%es\n"
		"mov $0xf000,%ebx\n"
		"mov $0x0080,%edx\n"

		"movb $0x02,%ah\n"
		"movb $0x01,%al\n"
		"movb $0x00,%ch\n"
		"movb $0x02,%cl\n"
		"movb $0x00,%dh\n"
		"movb $0x80,%dl\n"
		"mov $0x0080,%dx\n"

		"sti \n"
		"int $0x13\n"
		"cli \n"
		"hlt \n"

		"mov %cr0,%eax\n"
		"or $0x1,%al\n"
		"mov %eax,%cr0\n"
	".code32\n"
		"ljmp $0x08,$b32 \n"
	"b32:"

		"mov $0x10,%ax\n"
		"mov %ax,%ds\n"
		"mov %ax,%es\n"
		"mov %ax,%fs\n"
		"mov %ax,%gs\n"
		"mov %ax,%ss\n"

		"jmp asm_end \n"

	"asm_end:"
		"pop %esp \n"
	);

	vga_puts(vga, "Read some disk");
}
