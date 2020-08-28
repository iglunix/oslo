/*
 * config.c
 *
 *  Created on: Jan 28, 2017
 *      Author: kmate
 */
#include <efi.h>
#include <efiutil.h>
#include <khelper.h>
#include <menu.h>
#include <config.h>

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

static
efi_status
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

void get_entries(efi_size *returned_entries, menu_entry_exec ***entries)
{
	efi_status status;

	efi_simple_file_system_protocol *self_volume;
	efi_file_protocol *root_dir, *entries_file;
	efi_size entries_contents_size;
	char *entries_contents;

	char *next_line, *current_line;
	efi_size current_line_length;

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
	*entries = NULL;
	*returned_entries = 0;

	/* Iterate through the contents */
	current_line = entries_contents;
	while (next_line = get_next_line(current_line)) {
		current_line_length = next_line - current_line;

		/* Parse line */

		if (current_line[0] == ';' || current_line[0] == '\n') { /* Ignore empty line or comment */
			goto next;
		}

		if (current_line[0] == '[') { /* New entry starts here */
			*entries = efi_realloc(*entries, (*returned_entries) * sizeof(menu_entry_exec *),
					(++(*returned_entries)) * sizeof(menu_entry_exec *));

			/* Fill the type field of the current entry */
			(*entries)[(*returned_entries) - 1] = efi_alloc(sizeof(menu_entry_exec));
			(*entries)[(*returned_entries) - 1]->base.type = menu_type_exec;

			goto next;
		}

		if (*returned_entries < 1) { /* If we reach here without good entries something is bad */
			efi_abort(L"Entries file is invalid! Please check!\n", EFI_INVALID_PARAMETER);
		}

		if (starts_with(current_line, "name="))
			(*entries)[(*returned_entries) - 1]->base.text = ascii_str_to_efi(current_line + 5, current_line_length - 6);
		if (starts_with(current_line, "path="))
			(*entries)[(*returned_entries) - 1]->path = ascii_str_to_efi(current_line + 5, current_line_length - 6);
		if (starts_with(current_line, "flags="))
			(*entries)[(*returned_entries) - 1]->flags = ascii_str_to_efi(current_line + 6, current_line_length - 7);
next:
		/* Go to the next line */
		current_line = next_line;
	}

	efi_free(entries_contents);

	/* Check if all entries have the required fields */
	for (efi_size index = 0; index < *returned_entries; ++index) {
		if ((*entries)[index]->base.text == NULL || (*entries)[index]->path == NULL)
			efi_abort(L"Entries file is invalid! Please check!\n", EFI_INVALID_PARAMETER);
	}
}
