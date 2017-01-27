#ifndef _UTIL_H
#define _UTIL_H

// globals
extern efi_handle_t self_image_handle;
extern efi_loaded_image_protocol_t *self_loaded_image;
extern efi_system_table_t *st;
extern efi_boot_services_t *bs;

// Utility funcs
void init_util(efi_handle_t image_handle, efi_system_table_t *system_table);
void puthex(uintn_t number);
void abort(efi_char16_t *error_msg, efi_status_t status);
void bzero(void *buffer, size_t len);
void *malloc(size_t size);
void free(void *buffer);

// String utils
size_t strlen(efi_char16_t *str);

/* Device path utils */
efi_device_path_protocol_t *merge_device_paths(efi_device_path_protocol_t *first, efi_device_path_protocol_t *second);
efi_device_path_protocol_t *append_filepath_device_path(efi_device_path_protocol_t *base, efi_char16_t *file_path);

/* Locate all handles that support the specified protocol */
efi_status_t locate_all_handles(efi_guid_t *protocol, uintn_t *num_handles, efi_handle_t **out_buffer);

#endif
