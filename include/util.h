#ifndef _UTIL_H
#define _UTIL_H

// globals
extern efi_handle_t self_image_handle;
extern efi_loaded_image_protocol_t *self_loaded_image;
extern efi_simple_file_system_protocol_t *self_volume;
extern efi_file_protocol_t *self_root_dir;
extern efi_system_table_t *st;
extern efi_boot_services_t *bs;

/* Initialize and deinitialize util structures */
void init_util(efi_handle_t image_handle, efi_system_table_t *system_table);
void fini_util();

/* Exit with status after printing error_msg */
void abort(efi_char16_t *error_msg, efi_status_t status);

/* Print a native width unsigned integer as a hexadecimal string */
void puthex(uintn_t number);

/* Zero a len sized memory region pointed to by buffer */
void bzero(void *buffer, size_t len);

/* Copy n bytes from src to dest */
void memcpy(void *dest, void *src, size_t n);

/* Allocate a size sized memory region */
void *malloc(size_t size);

/* Free the memory region pointed to by buffer */
void free(void *buffer);

/* Allocate a newsize sized memory region and copy over oldsize bytes
 * from oldptr and free the region pointed to by oldptr
 * if oldptr is null just allocate the new region and return */
void *realloc(void *oldptr, size_t oldsize, size_t newsize);

/* Determine the length of an EFI string pointed to by str */
size_t strlen(efi_char16_t *str);

/* Determine how many a bytes an EFI string takes to store
 *  including the null-terminator */
size_t strsize(efi_char16_t *str);

/* Determine the length of an ASCII string pointed to by str */
size_t ascii_strlen(char *str);

/* Allocate a buffer to store the device paths pointed first and second and merge them */
efi_device_path_protocol_t *merge_device_paths(efi_device_path_protocol_t *first, efi_device_path_protocol_t *second);

/* Generate a file path device path from a string file path and merge it
 * with the device path pointed to by base */
efi_device_path_protocol_t *append_filepath_device_path(efi_device_path_protocol_t *base, efi_char16_t *file_path);

/* Locate all EFI handles that support the specified protocol */
efi_status_t locate_all_handles(efi_guid_t *protocol, uintn_t *num_handles, efi_handle_t **out_buffer);

#endif
