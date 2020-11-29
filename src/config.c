/*
 * config.c
 *
 *  Created on: Jan 28, 2017
 *      Author: kmate
 */
#include <efi.h>
#include <efiutil.h>
#include <khelper.h>
#include "menu.h"
#include "config.h"

/* Convert an ascii string to an EFI string */
static efi_ch16 *ascii_str_to_efi(char *ascii_str, efi_size len)
{
	efi_ch16 *str;
	efi_size index;

	str = efi_alloc((len + 1) * sizeof(efi_ch16));
	for (index = 0; index < len; ++index) {
		str[index] = (efi_ch16) ascii_str[index];
	}
	str[++index] = L'\0';

	return str;
}

static efi_size get_file_size(efi_file_protocol *file)
{
	efi_status	status;
	efi_file_info	*file_info;
	efi_size	file_size;

	status = get_file_info(file, &file_info);
	if (EFI_ERROR(status))
		efi_abort(L"Error getting file info!\n", status);

	file_size = file_info->file_size;
	efi_free(file_info);
	return file_size;
}

static efi_bool starts_with(char *haystack, char *needle)
{
	efi_size needle_len, index;

	needle_len = strlen(needle);
	for (index = 0; index < needle_len; ++index) {
		if (haystack[index] != needle[index]) {
			return false;
		}
	}

	return true;
}

static char *get_next_line(char *start)
{
	for (;*start != '\n'; ++start) {
		if (*start == 0) {
			return NULL;
		}
	}
	return ++start;
}

static efi_status
locate_self_volume(efi_simple_file_system_protocol **self_volume)
{
	efi_status status;
	efi_loaded_image_protocol *self_loaded_image;

	status = bs->handle_protocol(
		self_image_handle,
		&(efi_guid) EFI_LOADED_IMAGE_PROTOCOL_GUID,
		(void **) &self_loaded_image);
	if (EFI_ERROR(status))
		return status;

	status = bs->handle_protocol(
		self_loaded_image->device_handle,
		&(efi_guid) EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID,
		(void **) self_volume);
	return status;
}

static void
sanity_check_entry(menu_entry *entry)
{
	if (!entry->text || !entry->path)
		efi_abort(
			L"Entries file is invalid! Please check!\n",
			EFI_INVALID_PARAMETER);
}

void add_boot_entries(menu_screen **menu)
{
	efi_status status;

	efi_simple_file_system_protocol *self_volume;
	efi_file_protocol *root_dir, *entries_file;
	efi_size entries_contents_size;
	char *entries_contents;

	char *next_line, *current_line;
	efi_size current_line_length;

	efi_bool have_entry;
	menu_entry entry;

	status = locate_self_volume(&self_volume);
	if (EFI_ERROR(status))
		efi_abort(L"Failed to locate boot volume!\n", status);


	/* Open root directory */
	status = self_volume->open_volume(self_volume, &root_dir);
	if (EFI_ERROR(status))
		efi_abort(L"Failed to open boot volume root directory!\n",
			status);

	/* Open the configuration file */
	status = root_dir->open(root_dir, &entries_file,
				 ENTRIES_FILE_NAME, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status))
		efi_abort(L"Error opening entries file!\n", status);

	/* Read the whole file */
	entries_contents_size = get_file_size(entries_file);
	/* +1 to leave space for the null-terminator */
	entries_contents = efi_alloc(entries_contents_size + 1);
	status = entries_file->read(entries_file, &entries_contents_size, entries_contents);
	if (EFI_ERROR(status))
		efi_abort(L"Error reading boot entry description file!\n", status);
	/* Null-terminate the string */
	entries_contents[entries_contents_size] = 0;

	/* Close the configuration file */
	status = entries_file->close(entries_file);
	if (EFI_ERROR(status))
		efi_abort(L"Error closing entries file!\n", status);

	/* Close root directory */
	status = root_dir->close(root_dir);
	if (EFI_ERROR(status))
		efi_abort(L"Failed to close boot volume root directory!\n",
			status);

	/* Prepare */
	have_entry = 0;

	/* Iterate through the contents */
	current_line = entries_contents;
	while (next_line = get_next_line(current_line)) {
		current_line_length = next_line - current_line;

		/* Parse line */

		if (current_line[0] == ';' || current_line[0] == '\n') { /* Ignore empty line or comment */
			goto next;
		}

		if (current_line[0] == '[') { /* New entry starts here */
			if (have_entry) {
				sanity_check_entry(&entry);
				menu_add_entries(menu, &entry, 1);
			} else {
				have_entry = 1;
			}

			/* Initialize new entry */
			entry.type = menu_type_exec;
			entry.text = NULL;
			entry.path = NULL;
			entry.flags = NULL;

			goto next;
		}

		if (!have_entry) { /* Properties must be part of an entry */
			efi_abort(
				L"Entries file is invalid! Please check!\n",
				EFI_INVALID_PARAMETER);
		}

		if (starts_with(current_line, "name="))
			entry.text = ascii_str_to_efi(current_line + 5, current_line_length - 6);
		if (starts_with(current_line, "path="))
			entry.path = ascii_str_to_efi(current_line + 5, current_line_length - 6);
		if (starts_with(current_line, "flags="))
			entry.flags = ascii_str_to_efi(current_line + 6, current_line_length - 7);
next:
		/* Go to the next line */
		current_line = next_line;
	}

	/* Add final entry */
	if (have_entry) {
		sanity_check_entry(&entry);
		menu_add_entries(menu, &entry, 1);
	}

	efi_free(entries_contents);
}
