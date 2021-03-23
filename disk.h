#ifndef _DISK_H_
#define _DISK_H_

#include <stdint.h>
#include <stddef.h>
#include "vga.h"

typedef struct mbr_t mbr_t;

typedef struct disk_t disk_t;

uint8_t *disk_read(disk_t *self, size_t *count);

#pragma pack(push)
#pragma pack(1)

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

#pragma pack(pop)


typedef enum fat_type_t {
	UNKNOWN = 0,
	FAT_12,
	FAT_16,
	FAT_32,
} fat_type_t;

typedef struct fat_t {
	fat_bs_t const *buf;
	fat_type_t type;
	uint32_t total_sectors;
	uint32_t fat_size;
	uint16_t root_dir_sectors;
	uint32_t first_data_sector;
	uint16_t first_fat_sector;
	uint32_t data_sectors;
	uint32_t total_clusters;
} fat_t;

/*
 * offset is 0x800 if starting at sector 4
 */

fat_t fat_new(size_t offset);

/*
 * Does a FAT dump on the screen
 * start: what offset to start reading from
 * count: how many bytes to dump
 */

void fat_dump(fat_t const *self, vga_t *, size_t start, size_t count);

/*
 * prints the OEM ID in the FAT BPB
 */

void fat_oem(fat_t const *self, vga_t *);

/*
 * prints files in the root directory
 */

void fat_ls(fat_t const *self, vga_t *);

/*
 * returns the size of each sector in bytes
 */

uint16_t fat_sector_size(fat_t const *self);

/*
 * returns the size of each cluster in sectors
 */

uint8_t fat_cluster_size(fat_t const *self);

/*
 * returns the number of reserved sectors
 */

uint16_t fat_reserved_count(fat_t const *self);

/*
 * returns the number of FATs
 */

uint8_t fat_table_count(fat_t const *self);

/*
 * returns the FAT size
 */

uint32_t fat_size(fat_t const *self);

/*
 * returns the number of directory entries
 */

uint16_t fat_dirent_count(fat_t const *self);

/*
 * FAT tbl lookup
 */

void fat_lookup(fat_t const *self, vga_t *vga);

#endif
