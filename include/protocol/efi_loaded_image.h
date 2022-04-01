/*
 * EFI loaded image protocol
 */

#ifndef EFI_LOADED_IMAGE_H
#define EFI_LOADED_IMAGE_H

#define EFI_LOADED_IMAGE_PROTOCOL_GUID \
  { 0x5B1B31A1, 0x9562, 0x11d2, 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, \
    0x72, 0x3B }

typedef struct efi_loaded_image_protocol efi_loaded_image_protocol;
struct efi_loaded_image_protocol {
	efi_u32				rev;
	efi_handle			parent;
	efi_system_table		*system_table;

	efi_handle			device_handle;
	efi_device_path_protocol	file_path;
	void				*reserved;

	efi_u32				load_options_size;
	void				*load_options;

	void				*image_base;
	efi_u64				image_size;
	efi_memory_type			image_code_type;
	efi_memory_type			image_data_type;
	efi_status	(efiapi *unload) (efi_handle image_handle);
};

#endif
