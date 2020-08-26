/*
 * config.c
 *
 *  Created on: Jan 28, 2017
 *      Author: kmate
 */
#include <efi.h>
#include <efiutil.h>
#include <menu.h>
#include <config.h>

/* Convert an ascii string to an EFI string */
static efi_ch16 *ascii_str_to_efi(char *ascii_str, efi_size len)
{
	efi_ch16 *str;
	efi_size index;

	str = malloc((len + 1) * sizeof(efi_ch16));
	for (index = 0; index < len; ++index) {
		str[index] = (efi_ch16) ascii_str[index];
	}
	str[++index] = L'\0';

	return str;
}

static efi_size get_file_size(efi_file_protocol *file)
{
	efi_status status;
	efi_file_info *file_info;
	efi_size file_info_size;
	efi_size file_size;

	/* Don't play the buffer growing dance just say the buffer is 0 bytes and allocate it afterwards then re-call */
	file_info = NULL;
	file_info_size = 0;
retry:
	status = file->get_info(file, &(efi_guid) EFI_FILE_INFO_ID, &file_info_size, file_info);
	if (status == EFI_BUFFER_TOO_SMALL) {
		file_info = malloc(file_info_size);
		goto retry;
	}
	if (EFI_ERROR(status)) {
		abort(L"Error getting file info!\r\n", status);
	}
	file_size = file_info->file_size;
	free(file_info);
	return file_size;
}

static efi_bool starts_with(char *haystack, char *needle)
{
	efi_size needle_len, index;

	needle_len = ascii_strlen(needle);
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

void get_entries(efi_size *returned_entries, menu_entry_exec ***entries)
{
	efi_status status;

	efi_file_protocol *entries_file;
	efi_size entries_contents_size;
	char *entries_contents;

	char *next_line, *current_line;
	efi_size current_line_length;

	/* Open the configuration file */
	status = self_root_dir->open(self_root_dir, &entries_file, ENTRIES_FILE_NAME, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status)) {
		abort(L"Error opening entries file!\r\n", status);
	}

	/* Read the whole file */
	entries_contents_size = get_file_size(entries_file);

	/* +1 to leave space for the null-terminator */
	entries_contents = malloc(entries_contents_size + 1);

	status = entries_file->read(entries_file, &entries_contents_size, entries_contents);
	if (EFI_ERROR(status)) {
		abort(L"Error reading boot entry description file!\r\n", status);
	}

	/* Null-terminate the string */
	entries_contents[entries_contents_size] = 0;

	/* Close the configuration file */
	status = entries_file->close(entries_file);
	if (EFI_ERROR(status)) {
		abort(L"Error closing entries file!\r\n", status);
	}

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
			*entries = realloc(*entries, (*returned_entries) * sizeof(menu_entry_exec *),
					(++(*returned_entries)) * sizeof(menu_entry_exec *));

			/* Fill the type field of the current entry */
			(*entries)[(*returned_entries) - 1] = malloc(sizeof(menu_entry_exec));
			(*entries)[(*returned_entries) - 1]->base.type = menu_type_exec;

			goto next;
		}

		if (*returned_entries < 1) { /* If we reach here without good entries something is bad */
			abort(L"Entries file is invalid! Please check!\r\n", EFI_INVALID_PARAMETER);
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

	free(entries_contents);

	/* Check if all entries have the required fields */
	for (efi_size index = 0; index < *returned_entries; ++index) {
		if ((*entries)[index]->base.text == NULL ||
				(*entries)[index]->path == NULL) {
			abort(L"Entries file is invalid! Please check!\r\n", EFI_INVALID_PARAMETER);
		}
	}
}
