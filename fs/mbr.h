#ifndef mbr_h_INCLUDED
#define mbr_h_INCLUDED

#include <stdint.h>

void mbr_dump_parts(uint8_t const *bsect);
uint8_t mbr_get_part_sector(uint8_t const *bsect, uint8_t index);

#endif // mbr_h_INCLUDED
