#include <stdint.h>
#include <stddef.h>
#include <dev/serial.h>
#include <dev/vga.h>
#include <dev/pci.h>
/* TODO: AHCI */
#include <dev/ide.h>

int main() {
	if (vga_init())
	 	return 1;

	vga_puts("Welcome to OSLO!");

	pci_dev_t dev;
	pci_init(&dev);

	ide_chan_t channels[2];

	ide_init(&dev, channels);

	uint16_t sec[256];

	ide_read_sectors(channels+0, 0, sec, 0x0, 1);

	for (uint8_t i = 0; i < 32; i++) {
		for (uint8_t j = 0; j < 8; j++) {
			vga_pretty_u16(sec[i * 8 + j]);
			vga_putchar(' ');
		}
		vga_putchar('\n');
	}

	return 0;
}
