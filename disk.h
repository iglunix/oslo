#ifndef _DISK_H_
#define _DISK_H_

#include <stdint.h>
#include <stddef.h>

typedef struct mbr_t mbr_t;

typedef struct disk_t disk_t;

uint8_t *disk_read(disk_t *self, size_t *count);

#endif
