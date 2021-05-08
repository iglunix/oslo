#ifndef fat_h_INCLUDED
#define fat_h_INCLUDED

#include <stdint.h>

typedef void (*disk_read_fn_t)(uint8_t *dest, uint32_t lba);

void fat_dump_info(uint8_t const *sect);

void fat_ls(disk_read_fn_t, uint32_t start);

#endif // fat_h_INCLUDED
