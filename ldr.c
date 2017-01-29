#include <efi.h>
#include <util.h>
#include <menu.h>
#include <config.h>

// About sub-menu
menu_entry_t *about_entries[] = {
	&(menu_entry_t) { menu_entry_info, L"Welcome to YAUB (Yet Another UEFI Bootloader)!" },
	&(menu_entry_t) { menu_entry_info, L"This program is licensed under the MIT license" },
	&(menu_entry_t) { menu_entry_info, L"and the full source code is available at:" },
	&(menu_entry_t) { menu_entry_info, L"https://github.com/kukrimate/yaub.git" },
	&(menu_entry_t) { menu_entry_exit, L"Back..." }
};

menu_screen_t about_menu = {
	L"About YAUB",
	5,
	about_entries
};

// Main menu entries
#define NUM_MAIN_MENU_ENTRIES 2
menu_entry_t *main_menu_entries[] = {
	(menu_entry_t *) &(menu_entry_subscreen_t) { menu_entry_subscreen, L"About YAUB", &about_menu },
	&(menu_entry_t)           { menu_entry_exit, L"Exit" }
};

menu_screen_t main_menu = {
	L"Select what OS to boot into",
	0,
	NULL
};

efi_device_path_protocol_t *get_self_volume_dp()
{
	efi_status_t status;
	efi_device_path_protocol_t *dp;

	status = bs->handle_protocol(self_loaded_image->device_handle, &(efi_guid_t) EFI_DEVICE_PATH_PROTOCOL_GUID, (void **) &dp);
	if (EFI_ERROR(status)) {
		abort(L"Error locating self volume device path!", status);
	}

	return dp;
}

static void start_efi_image(efi_char16_t *path, efi_char16_t *flags)
{
	efi_status_t status;
	efi_in_key_t key;
	efi_handle_t child_image_handle;
	efi_device_path_protocol_t *image_dp;

	/* Draw a banner for the image */
	menu_clearscreen();
	menu_banner(L"Running child image");

	/* Get rid of util structures before starting a child image */
	fini_util();

	/* Start the image */
	image_dp = append_filepath_device_path(get_self_volume_dp(), path);
	status = bs->load_image(false, self_image_handle, image_dp, NULL, 0, &child_image_handle);
	status = bs->start_image(child_image_handle, NULL, NULL);

done:
	free(image_dp);

	/* If the image returns re-init ourselves */
	init_util(self_image_handle, st);
	menu_init();

	/* Wait for a keypress before re-drawing the menu */
	menu_clearscreen();
	st->con_out->output_string(st->con_out, L"Started image returned! Press any key to continue!\r\n");
	menu_wait_for_key(&key);
}

efi_status_t efi_func efi_main(efi_handle_t image_handle, efi_system_table_t *system_table)
{
	menu_entry_t *selected;

	menu_entry_t **merged_entries;
	menu_entry_exec_t **boot_entries;
	size_t boot_entries_size;

	init_util(image_handle, system_table);
	menu_init();

	// Read menu entries
	get_entries(&boot_entries_size, &boot_entries);

	// Merge pre-defined and dynamic entries
	merged_entries = malloc(sizeof(menu_entry_t *) * (boot_entries_size + NUM_MAIN_MENU_ENTRIES));
	memcpy(merged_entries, boot_entries, sizeof(menu_entry_t *) * boot_entries_size);
	memcpy((void *) (((uintn_t) merged_entries) + sizeof(menu_entry_t *) * boot_entries_size), main_menu_entries, sizeof(menu_entry_t *) * NUM_MAIN_MENU_ENTRIES);

	main_menu.entry_count = boot_entries_size + NUM_MAIN_MENU_ENTRIES;
	main_menu.entries = merged_entries;

	for (;;) {
		selected = menu_run(&main_menu, 0);
		switch (selected->type) {
		case menu_entry_exit:
			goto done;
		case menu_entry_exec:
			start_efi_image(((menu_entry_exec_t *) selected)->path, ((menu_entry_exec_t *) selected)->flags);
			break;
		default:
			break;
		}
	}

done:
	menu_clearscreen();
	fini_util();
	return EFI_SUCCESS;
}
