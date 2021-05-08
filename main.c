#include <stdint.h>
#include <stddef.h>
#include <dev/serial.h>
#include <dev/vga.h>
#include <dev/pci.h>
/* TODO: AHCI */
#include <dev/ide.h>
#include <fs/mbr.h>
#include <fs/fat.h>

ide_chan_t channels[2];

static void disk_read(uint8_t *dest, uint32_t lba) {
	ide_read_sectors(channels+0, 0, (uint16_t *) dest, lba, 1);
}

int main() {
	if (vga_init())
	 	return 1;

	vga_puts("Welcome to OSLO!");

	pci_dev_t dev;
	pci_init(&dev);

	ide_init(&dev, channels);

	uint8_t sec[512];
	disk_read(sec, 4 * 8);

	fat_dump_info(sec);

	fat_ls(&disk_read, 4 * 8);

	return 0;
}
