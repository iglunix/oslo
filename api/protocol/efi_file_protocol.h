#ifndef _EFI_FILE_PROTOCOL_H
#define _EFI_FILE_PROTOCOL_H

typedef struct _efi_efi_file_protocol efi_file_protocol_t;
struct _efi_efi_file_protocol {
	uint64_t revision;
	efi_status_t (efi_func *open)   (efi_file_protocol_t* self, efi_file_protocol_t **new_handle,
			efi_char16_t *file_name, uint64_t open_mode, uint64_t attributes);
	efi_status_t (efi_func *close)  (efi_file_protocol_t* self);
	efi_status_t (efi_func *delete) (efi_file_protocol_t* self);
	efi_status_t (efi_func *read)   (efi_file_protocol_t* self, uintn_t *buffer_size, void *buffer);
	efi_status_t (efi_func *write)  (efi_file_protocol_t* self, uintn_t *buffer_size, void *buffer);

	efi_status_t (efi_func *get_position)  (efi_file_protocol_t* self, uint64_t *position);
	efi_status_t (efi_func *set_position)  (efi_file_protocol_t* self, uint64_t position);

	efi_status_t (efi_func *get_info)  (efi_file_protocol_t* self, efi_guid_t *information_type,
			uintn_t *buffer_size, void *buffer);
	efi_status_t (efi_func *set_info)  (efi_file_protocol_t* self, efi_guid_t *information_type,
			uintn_t buffer_size, void *buffer);

	efi_status_t (efi_func *flush)  (efi_file_protocol_t* self);
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
  { 0x09576e92, 0x6d3f, 0x11d2, 0x8e39, 0x00, 0xa0, \
    0xc9, 0x69, 0x72, 0x3b }

typedef struct {
	uint64_t     size;
	uint64_t     file_size;
	uint64_t     physical_size;
	efi_time_t   creation_time;
	efi_time_t   last_access_time;
	efi_time_t   modification_time;
	uint64_t     attribute;
	efi_char16_t file_name[1];
} efi_file_info_t;

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
	efi_char16_t volume_label[1];
} efi_file_system_volume_label_t;

#endif
