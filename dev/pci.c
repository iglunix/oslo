#include <dev/vga.h>
#include <util/io.h>
#include "pci.h"

#define PCI_CFG_ADDR 0xcf8
#define PCI_CFG_DATA 0xcfc

#define PCI_CFG_REG(bus, slot, func, offset) \
	( \
		(1 << 31) \
		| (bus << 16) \
		| (slot << 11) \
		| (func << 8) \
		| offset \
	)

static char const *pci_class_names[] = {
	"Unclassified device",
	"Mass storage controller",
	"Network controller",
	"Display controller",
	"Multimedia controller",
	"Memory controller",
	"Bridge",
	"Communication controller",
	"Generic system peripheral",
	"Input device controller",
	"Docking station",
	"Processor",
	"Serial bus controller",
	"Wireless controller",
	"Intelligent controller",
	"Satellite communications controller",
	"Encryption controller",
	"Signal processing controller",
	"Processing accelerators",
	"Non-Essential Instrumentation",
	NULL,
};

static uint8_t pci_cfg_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	outl(PCI_CFG_ADDR, PCI_CFG_REG(bus, slot, func, offset));
	return inb(PCI_CFG_DATA + (offset & 3));
}


static uint16_t pci_cfg_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	outl(PCI_CFG_ADDR, PCI_CFG_REG(bus, slot, func, offset));
	return inw(PCI_CFG_DATA + (offset & 2));
}


static uint32_t pci_cfg_read_long(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	outl(PCI_CFG_ADDR, PCI_CFG_REG(bus, slot, func, offset));
	return inl(PCI_CFG_DATA + (offset & 1));
}

static uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot) {
	return pci_cfg_read_word(bus, slot, 0, 0);
}

static uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func) {
	return pci_cfg_read_word(bus, slot, func, 2);
}

static uint8_t pci_get_class(uint8_t bus, uint8_t slot, uint8_t func) {
	return pci_cfg_read_byte(bus, slot, func, 0x0b);
}

static void check_all_busses() {
	for (uint16_t bus = 0; bus < 256; bus++) {
		for (uint8_t slot = 0; slot < 8; slot++) {
			uint16_t id = pci_get_vendor_id(bus, slot);
			if (id != 0xffff) {
				for (uint8_t func = 0; func < 8; func++) {
					uint16_t dev_id = pci_get_device_id(bus, slot, func);
					if (dev_id != 0xffff) {
						vga_printf("%xh:%xhh.%xhh %xh:%xh %s\n",
							bus, slot, func,
							id, dev_id,
							pci_class_names[pci_get_class(bus, slot, func)]
						);
					}
				}
			}
		}
	}
	vga_puts("Enumerated all devices");
}

int pci_init() {
	check_all_busses();

	return 0;
}
