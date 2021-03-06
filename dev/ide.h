#ifndef ide_h_INCLUDED
#define ide_h_INCLUDED

#include <dev/pci.h>

/*
 * Status
 */

/* busy */
#define ATA_SR_BSY \
	0x80

/* ready */
#define ATA_SR_DRDY \
	0x40

/* write fault */
#define ATA_SR_DF \
	0x20

/* seek complete */
#define ATA_SR_DSC \
	0x10

/* request ready */
#define ATA_SR_DRQ \
	0x08

/* corrected data */
#define ATA_SR_CORR \
	0x04

/* index */
#define ATA_SR_IDX \
	0x02

/* error */
#define ATA_SR_ERR \
	0x01

/*
 * Registers
 */

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

/*
 * Commands
 */

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200


typedef struct ide_chan_t {
	uint8_t index;
	uint16_t base;
	uint16_t ctrl;
	uint16_t bmide;
	uint8_t nint;
} ide_chan_t;

int ide_init(pci_dev_t const *, ide_chan_t channels[2]);
void ide_read_sectors(ide_chan_t const *channel, uint8_t drive, uint16_t *dest, uint32_t lba, uint8_t sector_count);


#endif // ide_h_INCLUDED
