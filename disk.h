#ifndef _DISK_H_
#define _DISK_H_

#include <stdint.h>
#include <stddef.h>
#include "vga.h"

typedef struct mbr_t mbr_t;

typedef struct disk_t disk_t;

uint8_t *disk_read(disk_t *self, size_t *count);

typedef struct fat_t fat_t;

struct fat_t {
	uint8_t *buf;
};

/*
 * offset is 0x800 if starting at sector 4
 */

fat_t fat_new(size_t offset);

void fat_oem(fat_t *self, vga_t *);
void fat_ls(fat_t *self, vga_t *);


#endif
