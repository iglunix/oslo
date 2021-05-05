#include <stdint.h>

typedef enum port_t {
	COM1 = 0x3F8,
	COM2 = 0x2F8,
	COM3 = 0x3E8,
	COM4 = 0x2E8
} port_t;

int serial_init(port_t port);
uint8_t serial_rx(port_t port);
void serial_tx(port_t port, uint8_t);
