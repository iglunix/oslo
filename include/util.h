#ifndef __UTIL_H
#define __UTIL_H

// globals
extern efi_handle_t self_image_handle;
extern efi_loaded_image_protocol_t *self_loaded_image;
extern efi_system_table_t *st;
extern efi_boot_services_t *bs;

// Utility funcs
void init_util(efi_handle_t image_handle, efi_system_table_t *system_table);
void abort(efi_char16_t *error_msg);
void bzero(void *buffer, size_t len);
void *malloc(size_t size);
void free(void *buffer);

#endif
