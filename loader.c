#include <stdint.h>
#include <stddef.h>
#include "disk.h"
#include "vga.h"

int main() {
	vga_t vga = vga_init();
	vga_puts(&vga, "Welcome to OSLO!");
	vga_puts(&vga, "OSLO is WIP");
	fat_t boot_part = fat_new(0x800);
	fat_oem(&boot_part, &vga);
	fat_ls(&boot_part, &vga);

//	fs_fat_t fat;
//	fs_fat_load(&fat, 0, 1, 0, 0x80);
	return 0;
}

extern void _start() {
	main();
}
