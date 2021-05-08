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

// #define PCI_DEBUG

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

static uint8_t pci_cfg_read_u8(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	outl(PCI_CFG_ADDR, PCI_CFG_REG(bus, slot, func, offset));
	return inb(PCI_CFG_DATA + (offset & 3));
}


static uint16_t pci_cfg_read_u16(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	outl(PCI_CFG_ADDR, PCI_CFG_REG(bus, slot, func, offset));
	return inw(PCI_CFG_DATA + (offset & 2));
}


static uint32_t pci_cfg_read_u32(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	outl(PCI_CFG_ADDR, PCI_CFG_REG(bus, slot, func, offset));
	return inl(PCI_CFG_DATA + (offset & 1));
}

static uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot) {
	return pci_cfg_read_u16(bus, slot, 0, 0);
}

static uint16_t pci_get_device_id(uint8_t bus, uint8_t slot, uint8_t func) {
	return pci_cfg_read_u16(bus, slot, func, 2);
}

static uint8_t pci_get_class(uint8_t bus, uint8_t slot, uint8_t func) {
	return pci_cfg_read_u8(bus, slot, func, 0x0b);
}

static uint8_t pci_get_sub_class(uint8_t bus, uint8_t slot, uint8_t func) {
	return pci_cfg_read_u8(bus, slot, func, 0x0a);
}

static void pci_get_bar(uint8_t bus, uint8_t slot, uint8_t func, uint32_t *bar) {
	bar[0] = pci_cfg_read_u32(bus, slot, func, 0x10 + 4 * 0);
	bar[1] = pci_cfg_read_u32(bus, slot, func, 0x10 + 4 * 1);
	bar[2] = pci_cfg_read_u32(bus, slot, func, 0x10 + 4 * 2);
	bar[3] = pci_cfg_read_u32(bus, slot, func, 0x10 + 4 * 3);
	bar[4] = pci_cfg_read_u32(bus, slot, func, 0x10 + 4 * 4);
	bar[5] = pci_cfg_read_u32(bus, slot, func, 0x10 + 4 * 5);
}

static void check_all_busses(pci_dev_t *dev) {
	for (uint16_t bus = 0; bus < 256; bus++) {
		for (uint8_t slot = 0; slot < 32; slot++) {
			uint16_t id = pci_get_vendor_id(bus, slot);
			if (id != 0xffff) {
				for (uint8_t func = 0; func < 8; func++) {
					uint16_t dev_id = pci_get_device_id(bus, slot, func);
					if (dev_id != 0xffff) {
						uint8_t class = pci_get_class(bus, slot, func);
#ifdef PCI_DEBUG
						vga_printf("%xh:%xhh.%xhh %xh:%xh %xhh:%xhh:%s\n",
							bus, slot, func,
							id, dev_id,
							class, pci_get_sub_class(bus, slot, func),
							pci_class_names[class]
						);
#endif
						if (class == 1) {
							dev->bus = bus;
							dev->slot = slot;
							dev->func = func;
							pci_get_bar(bus, slot, func, dev->bar);
						}
					}
				}
			}
		}
	}
	vga_puts("Enumerated all devices");
}

int pci_init(pci_dev_t *dev) {
	check_all_busses(dev);
	return 0;
}
