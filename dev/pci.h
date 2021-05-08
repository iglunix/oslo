#ifndef pci_h_INCLUDED
#define pci_h_INCLUDED

#include <stdint.h>

typedef struct pci_dev_t {
	uint16_t bus;
	uint8_t slot;
	uint8_t func;
	uint32_t bar[6];
} pci_dev_t;

/*
 * Enumerates PCI devices and stores a storage device in `dev`
 */
int pci_init(pci_dev_t *dev);

#endif // pci_h_INCLUDED

