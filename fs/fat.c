#include <stdbool.h>
#include <dev/vga.h>
#include "fat.h"


typedef struct fat_bs_ext_32_t {
	uint32_t tbl_size;
} __attribute__((packed)) fat_bs_ext_32_t;

typedef struct fat_bs_ext_16_t {
	int8_t v;
} __attribute__((packed)) fat_bs_ext_16_t;

typedef struct fat_bs_t {
	uint8_t boot_jmp[3];
	uint8_t oem[8];
	uint16_t sector_size;
	uint8_t cluster_size;
	uint16_t reserved_sector_count;
	uint8_t tbl_count;
	uint16_t root_ent_count;
	uint16_t total_sectors; /* use total_sectors_32 for FAT32 */
	uint8_t media_type;
	uint16_t tbl_size; /* use ext->fat32.tbl_size for FAT32 */
	uint16_t track_size;
	uint16_t head_size_count;
	uint32_t hidden_sector_count;
	uint32_t total_sectors_32;

	union {
		uint8_t pad[54];
		fat_bs_ext_16_t fat16;
		fat_bs_ext_32_t fat32;
	} __attribute__((packed)) ext;
} __attribute__((packed)) fat_bs_t;


void fat_dump_info(uint8_t const *sect) {
	fat_bs_t const *bs = (fat_bs_t const *) sect;
	vga_printf("OEM: ");
	for (uint8_t i = 0; i < 8; i++) {
		vga_putchar(bs->oem[i]);
	}
	vga_puts("");
}

void fat_ls(disk_read_fn_t disk_read_fn, uint32_t start) {
	union {
		uint8_t bytes[512];
		fat_bs_t fat_bs;
	} __attribute__((packed)) start_sect;

	disk_read_fn(start_sect.bytes, start);

	uint32_t root_dir_sectors = ((start_sect.fat_bs.root_ent_count * 32) + (start_sect.fat_bs.sector_size - 1)) / start_sect.fat_bs.sector_size;
	uint32_t first_data_sector = start_sect.fat_bs.reserved_sector_count + (start_sect.fat_bs.tbl_count * start_sect.fat_bs.tbl_size) + root_dir_sectors;

	uint8_t root_dir_buf[512];
	disk_read_fn(root_dir_buf, first_data_sector);

	vga_puts("fat ls");

	bool has_long_name = false;
	char long_name[32];

	for (uint32_t i = 0; root_dir_buf[i*32]; i++) {
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
				vga_printf("-> %s\n", long_name);
				has_long_name = false;
			} else {
				vga_printf("  ");
				for (size_t j = 0; j < 11; j++)
					vga_putchar(root_dir_buf[i * 32 + j]);
				vga_putchar('\n');
			}
			/* check if not volume id */
			if (!(root_dir_buf[i * 32 + 11] & 0x08)) {
				size_t offset = (first_data_sector + ((root_dir_buf[i * 32 + 20] << 16 | root_dir_buf[i * 32 + 26]) - 2) * start_sect.fat_bs.cluster_size);
			 	uint8_t ptr[512];
				disk_read_fn(ptr, offset + start);
			 	vga_printf("data: %x\n", offset + start);
			 	vga_puts((char const *)ptr);
			}

		}
	}
}
