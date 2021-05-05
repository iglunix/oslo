#include <stdint.h>
#include <stddef.h>
#include <dev/serial.h>
// #include "disk.h"
#include <dev/vga.h>
#include <dev/pci.h>

int x = 0;

char const *msg = "Hello  World!";

int main() {
	vga_t vga;
	if (vga_init())
	 	return 1;

	vga_puts("Welcome to OSLO!");

	pci_init();

	return 0;
}
