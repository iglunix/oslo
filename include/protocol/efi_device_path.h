/*
 * EFI device path protocol
 */

#ifndef EFI_DEVICE_PATH_H
#define EFI_DEVICE_PATH_H

#define EFI_DEVICE_PATH_PROTOCOL_GUID \
  { 0x09576e91, 0x6d3f, 0x11d2, 0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, \
    0x72, 0x3b }

#define EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID \
  { 0xbc62157e, 0x3e33, 0x4fec, 0x99, 0x20, 0x2d, 0x3b, 0x36, 0xd7, \
    0x50, 0xdf }

typedef struct efi_device_path_protocol efi_device_path_protocol;
struct efi_device_path_protocol {
    efi_u8  type;
    efi_u8  sub_type;
    efi_u8  length[2];
};

//
// Hardware Device Path
//

#define HARDWARE_DEVICE_PATH 0x01

/* PCI Device Path Sub-type */
#define HARDWARE_PCI_DP 0x01

typedef struct {
    efi_device_path_protocol    header;
    efi_u8                      function;
    efi_u8                      device;
} pci_device_path;

/* PCCARD Device Path Sub-type */
#define HARDWARE_PCCARD_DP 0x02

typedef struct {
    efi_device_path_protocol    header;
    efi_u8                      function;
} pccard_device_path;

/* Memory Mapped Device Path Sub-type */
#define HARDWARE_MMAP_DP 0x03

typedef struct {
    efi_device_path_protocol    header;
    efi_memory_type             memory_type;
    efi_physical_address        start_address;
    efi_physical_address        end_address;
} mmap_device_path;

/* Vendor-specific Device Path Sub-type */
#define HARDWARE_VENDOR_DP 0x04

typedef struct {
    efi_device_path_protocol    header;
    efi_guid                    vendor_guid;
    // n bytes of vendor specific data
} vendor_device_path;

/* Controller device Path Sub-type */
#define HARDWARE_CONTROLLER_DP 0x05

typedef struct {
    efi_device_path_protocol    header;
    efi_u32                     controller_number;
} controller_device_path;

/*
 * NOTE: ACPI and Messaging device path structures will be added as necessary
 *   to avoid bloating up the headers with unused structures
 */

//
// ACPI Device Path
//

#define ACPI_DEVICE_PATH 0x02

//
// Messaging Device Path
//

#define MESSAGING_DEVICE_PATH 0x03

//
// Media Device Path
//

#define MEDIA_DEVICE_PATH 0x04

/* Hard Drive Device Path Sub-type */
#define MEDIA_HARDDRIVE_DP 0x01

typedef struct {
    efi_device_path_protocol    header;
    efi_u32                     partition_number;
    efi_u64                     partition_start;
    efi_u64                     partition_size;
    efi_u8                      signature[16];
    efi_u8                      mbr_type;
    efi_u8                      signature_type;
} harddrive_device_path;

#define MBR_TYPE_PCAT                       0x01
#define MBR_TYPE_EFI_PARTITION_TABLE_HEADER 0x02

#define NO_DISK_SIGNATURE   0x00
#define SIGNATURE_TYPE_MBR  0x01
#define SIGNATURE_TYPE_GUID 0x02

/* CD-ROM Device Path Sub-type */
#define MEDIA_CDROM_DP 0x02

typedef struct {
    efi_device_path_protocol    header;
    efi_u32                     boot_entry;
    efi_u64                     partition_start;
    efi_u64                     partition_size;
} cdrom_device_path;

/* Vendor-specific Device Path Sub-type */
#define MEDIA_VENDOR_DP 0x03

/* File Path Device Path Sub-type */
#define MEDIA_FILEPATH_DP 0x04

typedef struct {
    efi_device_path_protocol    header;
    efi_ch16                    path_name[1];
} filepath_device_path;

/* Media Protocol Device Path Sub-type */
#define MEDIA_PROTOCOL_DP 0x05

typedef struct {
    efi_device_path_protocol    header;
    efi_guid                    protocol;
} media_protocol_dp;

//
// End of Device Path
//

#define END_DEVICE_PATH_TYPE             0x7f

#define END_ENTIRE_DEVICE_PATH_SUBTYPE   0xff
#define END_INSTANCE_DEVICE_PATH_SUBTYPE 0x01

#endif
