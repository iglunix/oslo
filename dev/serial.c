#include <stdbool.h>
#include <util/io.h>
#include "serial.h"

int serial_init(port_t port) {
	outb(port + 1, 0x00);
	outb(port + 3, 0x80);
	outb(port + 0, 0x03);
	outb(port + 1, 0x00);
	outb(port + 3, 0x03);
	outb(port + 2, 0xC7);
	outb(port + 4, 0x0B);
	outb(port + 4, 0x1E);
	outb(port + 0, 0xAE);
	if (inb(port + 0) != 0xAE) {
		return 1;
	}

	outb(port + 4, 0x0F);

	return 0;
}

static inline bool rx_ready(port_t port) {
	return inb(port + 5) & 0x01;
}

static inline bool tx_ready(port_t port) {
	return inb(port + 5) & 0x20;
}

uint8_t serial_rx(port_t port) {
	while (!rx_ready(port));
	return inb(port);
}

void serial_tx(port_t port, uint8_t v) {
	// while (!tx_ready(port));
	// outb(port, v);
}
