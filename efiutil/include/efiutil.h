/*
 * UEFI dependent helper functions
 */
#ifndef EFIUTIL_H
#define EFIUTIL_H

/*
 * efiutil provides some standard library support
 */
#include <stdarg.h>
#include <string.h>

/*
 * Globals for accessing UEFI
 */
extern efi_handle self_image_handle;
extern efi_system_table *st;
extern efi_boot_services *bs;
extern efi_runtime_services *rt;

/*
 * Must be called before using the globals above
 */
void efi_init(efi_handle image_handle, efi_system_table *system_table);

/*
 * Like printf, but uses UEFI strings
 */
void efi_print(efi_ch16 *format, ...);

/*
 * Print error_msg, then exit with status
 */
void efi_abort(efi_ch16 *error_msg, efi_status status);

/*
 * Assert that condition is true, print error_msg and abort if it fails
 */
void efi_assert(efi_bool condition, efi_ch16 *error_msg);

/*
 * Allocate an n byte memory region
 */
void *efi_alloc(efi_size n);

/*
 * Free the memory region pointed to by ptr
 */
void efi_free(void *ptr);

/*
 * Resize a memory region from oldsize to newsize
 */
void *efi_realloc(void *oldptr, efi_size oldsize, efi_size newsize);

/*
 * Compare two EFI strings for equality
 * The return value works similar to strcmp
 */
efi_ssize efi_strcmp(efi_ch16 *str1, efi_ch16 *str2);

/*
 * Determine the length of an EFI string pointed to by str
 */
efi_size efi_strlen(efi_ch16 *str);

/*
 * Determine how many a bytes an EFI string takes to store
 *  including the null-terminator
 */
efi_size efi_strsize(efi_ch16 *str);

/*
 * Allocate a buffer to store the device paths pointed first
 * and second and merge them
 */
efi_device_path_protocol *merge_device_paths(efi_device_path_protocol *first,
    efi_device_path_protocol *second);

/*
 * Generate a file path device path from a string file path and merge it
 * with the device path pointed to by base
 */
efi_device_path_protocol *append_filepath_device_path(
    efi_device_path_protocol *base, efi_ch16 *file_path);

/*
 * Locate all EFI handles that support the specified protocol
 */
efi_status locate_all_handles(efi_guid *protocol, efi_size *num_handles,
	efi_handle **out_buffer);

/*
 * Locate the first instance of a protocol
 */
efi_status locate_protocol(efi_guid *protocol, void **iface);

/*
 * Get the file info struct for file
 */
efi_status get_file_info(efi_file_protocol *file, efi_file_info **file_info);

#endif
