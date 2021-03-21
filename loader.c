#include <stdint.h>
#include <stddef.h>
#include "disk.h"
#include "vga.h"

int main() {
	vga_t vga = vga_init();
	vga_puts(&vga, "Welcome to OSLO!");
	vga_puts(&vga, "OSLO is WIP");
	fat_t boot_part = fat_new(0x1000);
	fat_oem(&boot_part, &vga);
	fat_ls(&boot_part, &vga);
	vga_putc(&vga, '\n');
	vga_pretty_byte(&vga, boot_part.type);

//	fat_dump(&boot_part, &vga, 0, 0x200);
/*	fat_ls(&boot_part, &vga);
	uint16_t sector_size = fat_sector_size(&boot_part);
	if (sector_size != 0x0200) {
		vga_puts(&vga, "Invalid Sector Size!");
	}
	uint16_t dirent_count = fat_dirent_count(&boot_part);
	vga_puts(&vga, "Dirent Count: ");
	vga_putc(&vga, '0' + dirent_count);*/

//	fs_fat_t fat;
//	fs_fat_load(&fat, 0, 1, 0, 0x80);
	return 0;
}

extern void _start() {
	main();
}
