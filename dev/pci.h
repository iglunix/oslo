#ifndef pci_h_INCLUDED
#define pci_h_INCLUDED

#include <stdint.h>

typedef struct pci_dev_t {
	uint16_t bus;
	uint8_t dev;
	uint8_t func;
} pci_dev_t;

int pci_init();

#endif // pci_h_INCLUDED

