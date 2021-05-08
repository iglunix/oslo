#include <dev/vga.h>
#include <util/io.h>
#include <stdbool.h>
#include "ide.h"

#define IDE_PORT_PRIMARY 0x1f0
#define IDE_PORT_SECONDARY 0x170

// typedef enum drv_type_t {
// 	ATA_NA,
// 	ATA_ATAPI,
// 	ATA_PATA,
// 	ATA_SATA
// } drv_type_t;

// static char const *drv_type_names[] = {
// 	"Unknown",
// 	"ATAPI",
// 	"PATA",
// 	"SATA"
// };

static void ide_write(ide_chan_t const *channel, uint8_t reg, uint8_t data) {
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->nint);
	}

	if (reg < 0x08) {
		outb(channel->base + reg - 0x00, data);
	} else if (reg < 0x0c) {
		outb(channel->base + reg - 0x06, data);
	} else if (reg < 0x0e) {
		outb(channel->ctrl + reg - 0x0a, data);
	} else if (reg < 0x16) {
		outb(channel->bmide + reg - 0x0e, data);
	}

	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, channel->nint);
	}
}

static uint8_t ide_read(ide_chan_t const *channel, uint8_t reg) {
	uint8_t ret = 0;
	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, 0x80 | channel->nint);
	}

	if (reg < 0x08) {
		ret = inb(channel->base + reg - 0x00);
	} else if (reg < 0x0c) {
		ret = inb(channel->base + reg - 0x06);
	} else if (reg < 0x0e) {
		ret = inb(channel->ctrl + reg - 0x0a);
	} else if (reg < 0x16) {
		ret = inb(channel->bmide + reg - 0x0e);
	}

	if (reg > 0x07 && reg < 0x0C) {
		ide_write(channel, ATA_REG_CONTROL, channel->nint);
	}
	return ret;
}

void ide_delay(ide_chan_t const *channel) {
	ide_read(channel, ATA_REG_ALTSTATUS);
	ide_read(channel, ATA_REG_ALTSTATUS);
	ide_read(channel, ATA_REG_ALTSTATUS);
	ide_read(channel, ATA_REG_ALTSTATUS);
}

void ide_wait_bsy(ide_chan_t const *channel) {
	while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY);
}

void ide_wait_drdy(ide_chan_t const *channel) {
	while (!(ide_read(channel, ATA_REG_STATUS) & ATA_SR_DRDY));
}

// static void ide_read_io(ide_chan_t const *channel, uint16_t *buf, size_t size) {
// 	for (size_t read = 0; read < (size >> 1); read++) {
// 		buf[read] = inw(channel->base + ATA_REG_DATA + read);
// 	}
// }

// static uint8_t ide_poll(ide_chan_t const *channel) {
// 	for (uint8_t i = 0; i < 4; i++) ide_read(channel, ATA_REG_ALTSTATUS);

// 	uint8_t ret;
// 	for (uint8_t attempts = 0; attempts < 16; attempts++) {
// 		ret = ide_read(channel, ATA_REG_STATUS);
// 		if (!(ret & ATA_SR_BSY)) {
// 			break;
// 		}
// 	}
// 	return ret;
// }

// void ide_print_err(ide_chan_t const *channel, uint8_t err) {
// 	// TODO
// }

// static void ide_identify(ide_chan_t const *channel, uint8_t drive) {
// 	vga_printf("Trying drive %xhh:%xhh\n", channel->index, drive);

// 	ide_write(channel, ATA_REG_HDDEVSEL, 0xa0 | (drive << 4));

// 	ide_delay(channel);

// 	ide_write(channel, ATA_REG_SECCOUNT0, 0);
// 	ide_write(channel, ATA_REG_LBA0, 0);
// 	ide_write(channel, ATA_REG_LBA1, 0);
// 	ide_write(channel, ATA_REG_LBA2, 0);

// 	ide_delay(channel);

// 	ide_write(channel, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);

// 	if (ide_read(channel, ATA_REG_STATUS) == 0) {
// 		vga_printf("Drive %xhh:%xhh failed\n", channel->index, drive);
// 		return;
// 	}

// 	bool err = false;

// 	while (true) {
// 		uint8_t status = ide_read(channel, ATA_REG_STATUS);
// 		if ((status & ATA_SR_ERR)) {
// 			vga_printf("Drive %xhh:%xhh error\n", channel->index, drive);
// 			err = true;
// 			return;
// 		}
// 		if (!(status & ATA_SR_BSY)) {
// 			break;
// 		}
// 	}

// 	drv_type_t type = ATA_NA;

// 	uint8_t cl = ide_read(channel, ATA_REG_LBA1);
// 	uint8_t ch = ide_read(channel, ATA_REG_LBA2);

// 	vga_printf("%xhh:%xhh\n", cl, ch);


// 	if (cl == 0x14 && ch == 0xeb) {
// 		type = ATA_ATAPI;
// 	} else if (cl == 0x69 && ch == 0x96) {
// 		type = ATA_ATAPI;
// 	} else if (cl == 0 && ch == 0) {
// 		type = ATA_PATA;
// 	} else if (cl == 0x3c && ch == 0xc3) {
// 		type = ATA_SATA;
// 	}

// 	while (true) {
// 		uint8_t status = ide_read(channel, ATA_REG_STATUS);
// 		if ((status & ATA_SR_ERR)) {
// 			vga_printf("Drive %xhh:%xhh error\n", channel->index, drive);
// 			err=true;
// 			return;
// 		}
// 		if (status & ATA_SR_DRQ) {
// 			break;
// 		}
// 	}

// 	ide_write(channel, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);

// 	/* delay */
// 	ide_delay(channel);

// 	vga_puts(drv_type_names[type]);

// 	uint8_t buf[2048];

// 	ide_read_io(channel, (uint16_t *) buf, 512);

// 	vga_pretty_u8(buf[ATA_IDENT_MODEL + 1]);
// 	vga_putchar('\n');

// }

void ide_read_sectors(ide_chan_t const *channel, uint8_t drive, uint16_t *dest, uint32_t lba, uint8_t sector_count) {
	ide_wait_bsy(channel);
	ide_write(channel, ATA_REG_HDDEVSEL, 0xe0 | (drive << 4));

	ide_write(channel, ATA_REG_SECCOUNT0, sector_count);
	ide_write(channel, ATA_REG_LBA0, (uint8_t) (lba >> 0));
	ide_write(channel, ATA_REG_LBA1, (uint8_t) (lba >> 8));
	ide_write(channel, ATA_REG_LBA2, (uint8_t) (lba >> 16));
	ide_write(channel, ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	for (uint8_t i = 0; i < sector_count; i++) {
		ide_wait_bsy(channel);
		ide_wait_drdy(channel);

		for (uint16_t j = 0; j < 256; j++) {
			dest[i * 256 + j] = inw(channel->base + ATA_REG_DATA);
		}
	}
}


int ide_init(pci_dev_t const *dev, ide_chan_t channels[2]) {
	vga_puts("IDE init");

	channels[0].index = 0;
	channels[1].index = 1;

	channels[0].base = (dev->bar[0] & 0xfffffffc) + 0x1f0 * (!dev->bar[0]);
	channels[0].ctrl = (dev->bar[1] & 0xfffffffc) + 0x3f6 * (!dev->bar[1]);
	channels[1].base = (dev->bar[2] & 0xfffffffc) + 0x170 * (!dev->bar[2]);
	channels[1].ctrl = (dev->bar[3] & 0xfffffffc) + 0x376 * (!dev->bar[3]);
	channels[0].bmide = (dev->bar[4] & 0xfffffffc) + 0;
	channels[1].bmide = (dev->bar[4] & 0xfffffffc) + 8;

	return 0;
}
