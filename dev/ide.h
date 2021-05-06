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

int ide_init(pci_dev_t const *);

#endif // ide_h_INCLUDED
