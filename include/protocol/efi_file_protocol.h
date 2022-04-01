/*
 * EFI file protocol
 */
#ifndef EFI_FILE_PROTOCOL_H
#define EFI_FILE_PROTOCOL_H

typedef struct efi_file_protocol efi_file_protocol;
struct efi_file_protocol {
	efi_u64	revision;
	efi_status (efiapi *open)   (efi_file_protocol* self, efi_file_protocol **new_handle,
			efi_ch16 *file_name, efi_u64 open_mode, efi_u64 attributes);
	efi_status (efiapi *close)  (efi_file_protocol* self);
	efi_status (efiapi *delete) (efi_file_protocol* self);
	efi_status (efiapi *read)   (efi_file_protocol* self, efi_size *buffer_size, void *buffer);
	efi_status (efiapi *write)  (efi_file_protocol* self, efi_size *buffer_size, void *buffer);

	efi_status (efiapi *get_position)  (efi_file_protocol* self, efi_u64 *position);
	efi_status (efiapi *set_position)  (efi_file_protocol* self, efi_u64 position);

	efi_status (efiapi *get_info)  (efi_file_protocol* self, efi_guid *information_type,
			efi_size *buffer_size, void *buffer);
	efi_status (efiapi *set_info)  (efi_file_protocol* self, efi_guid *information_type,
			efi_size buffer_size, void *buffer);

	efi_status (efiapi *flush)  (efi_file_protocol* self);
};

// Open modes
#define EFI_FILE_MODE_READ   0x0000000000000001
#define EFI_FILE_MODE_WRITE  0x0000000000000002
#define EFI_FILE_MODE_CREATE 0x8000000000000000

// Attributes
#define EFI_FILE_READ_ONLY  0x0000000000000001
#define EFI_FILE_HIDDEN     0x0000000000000002
#define EFI_FILE_SYSTEM     0x0000000000000004
#define EFI_FILE_RESERVED   0x0000000000000008
#define EFI_FILE_DIRECTORY  0x0000000000000010
#define EFI_FILE_ARCHIVE    0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037

// EFI file info
#define EFI_FILE_INFO_ID \
  { 0x09576e92, 0x6d3f, 0x11d2, 0x8e, 0x39, 0x00, 0xa0, \
    0xc9, 0x69, 0x72, 0x3b }

typedef struct {
	efi_u64		size;
	efi_u64		file_size;
	efi_u64		physical_size;
	efi_time	creation_time;
	efi_time	last_access_time;
	efi_time	modification_time;
	efi_u64		attribute;
	efi_ch16	file_name[1];
} efi_file_info;

// File info attribute bits
#define EFI_FILE_READ_ONLY  0x0000000000000001
#define EFI_FILE_HIDDEN     0x0000000000000002
#define EFI_FILE_SYSTEM     0x0000000000000004
#define EFI_FILE_RESERVED   0x0000000000000008
#define EFI_FILE_DIRECTORY  0x0000000000000010
#define EFI_FILE_ARCHIVE    0x0000000000000020
#define EFI_FILE_VALID_ATTR 0x0000000000000037

// EFI file system volume label info
#define EFI_FILE_SYSTEM_VOLUME_LABEL_ID \
  { 0xDB47D7D3, 0xFE81, 0x11d3, 0x9A35, 0x00, 0x90, 0x27, 0x3F, 0xC1, \
    0x4D }

typedef struct {
	efi_ch16	volume_label[1];
} efi_file_system_volume_label;

#endif
