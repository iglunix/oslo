/*
 * config.c
 *
 *  Created on: Jan 28, 2017
 *      Author: kmate
 */
#include <efi.h>
#include <util.h>
#include <menu.h>
#include <config.h>

/* Convert an ascii string to an EFI string */
static efi_char16_t *ascii_str_to_efi(char *ascii_str)
{
	efi_char16_t *str;
	size_t index;

	str = malloc((ascii_strlen(ascii_str) + 1) * sizeof(efi_char16_t));
	for (index = 0; ascii_str[index] != '\0'; ++index) {
		str[index] = (efi_char16_t) ascii_str[index];
	}
	str[++index] = L'\0';

	return str;
}

static size_t read_line(efi_file_protocol_t *file, char *buffer)
{
	efi_status_t status;
	size_t i;
	uintn_t read_len;

	// FIXME: this code is vulnerable to a buffer overflow attack
	for (i = 0;; ++i) {
		read_len = 1;
		status = file->read(file, &read_len, (char *)(((uintn_t) buffer) + i));
		if (buffer[i] == '\n') {
			if (i == 0) { // Ignore empty lines
				--i;
				continue;
			}
			buffer[i] = '\0';
			goto done;
		}

		if (read_len == 0) {
			buffer[i + 1] = '\0';
			goto done;
		}
	}
done:
	return i;
}

efi_bool_t ascii_str_starts_with(char *haystack, char *needle)
{
	size_t needle_len;

	needle_len = ascii_strlen(needle);
	for (size_t i = 0; i < needle_len; ++i) {
		if (haystack[i] != needle[i]) {
			return false;
		}
	}

	return true;
}

char *ascii_str_split_at(char *str, char delim)
{
	size_t str_len;

	str_len = ascii_strlen(str);
	for (size_t i = 0; i < str_len; ++i) {
		if (str[i] == delim) {
			return (char *) (((uintn_t) str) + i + 1);
		}
	}

	return NULL;
}

efi_device_path_protocol_t *find_self_volume_dp()
{
	efi_status_t status;
	efi_device_path_protocol_t *self_volume_dp;

	status = bs->handle_protocol(self_loaded_image->device_handle, &(efi_guid_t) EFI_DEVICE_PATH_PROTOCOL_GUID, (void **) &self_volume_dp);
	if (EFI_ERROR(status)) {
		abort(L"Error locating self volume device path!\r\n", status);
	}

	return self_volume_dp;
}

void get_entries(uintn_t *buffer_size, menu_entry_exec_t ***buffer)
{
	efi_status_t status;
	efi_file_protocol_t *entries_file;
	size_t count;
	char line[CONFIG_FILE_MAX_LINE_LEN];
	efi_device_path_protocol_t *self_volume_dp;

	self_volume_dp = find_self_volume_dp();

	status = self_root_dir->open(self_root_dir, &entries_file, ENTRIES_FILE_NAME, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(status)) {
		abort(L"Error opening entries file!\r\n", status);
	}

	/* Count the entries */
	count = 0;
	while (read_line(entries_file, line)) {
		if (line[0] == ';') { // Ignore comments
			continue;
		}

		if (line[0] == '[') { // New entry
			++count;
			continue;
		}
	}

	/* Allocate memory for the entries */
	*buffer = malloc(count * sizeof(menu_entry_t *));

	/* Read again */
	entries_file->set_position(entries_file, 0);
	count = 0;
	while (read_line(entries_file, line)) {
		if (line[0] == ';') { // Ignore comments
			continue;
		}

		if (line[0] == '[') { // New entry
			++count;
			(*buffer)[count - 1] = malloc(sizeof(menu_entry_exec_t));
			(*buffer)[count - 1]->base.type = menu_entry_exec;
			continue;
		}

		if (ascii_str_starts_with(line, "name")) {
			(*buffer)[count - 1]->base.text = ascii_str_to_efi(ascii_str_split_at(line, '='));
		}

		if (ascii_str_starts_with(line, "path")) {
			(*buffer)[count - 1]->path = append_filepath_device_path(self_volume_dp, ascii_str_to_efi(ascii_str_split_at(line, '=')));
		}

		if (ascii_str_starts_with(line, "flags")) {
			(*buffer)[count - 1]->flags = ascii_str_to_efi(ascii_str_split_at(line, '='));
		}
	}

	*buffer_size = count;

	status = entries_file->close(entries_file);
	if (EFI_ERROR(status)) {
		abort(L"Error closing entries file!\r\n", status);
	}
}
