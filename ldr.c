#include <efi.h>
#include <efiutil.h>
#include <menu.h>
#include <config.h>

// About sub-menu
menu_entry *about_entries[] = {
	&(menu_entry) { menu_type_info, L"Welcome to YAUB (Yet Another UEFI Bootloader)!" },
	&(menu_entry) { menu_type_info, L"This program is licensed under the MIT license" },
	&(menu_entry) { menu_type_info, L"and the full source code is available at:" },
	&(menu_entry) { menu_type_info, L"https://github.com/kukrimate/yaub.git" },
	&(menu_entry) { menu_type_exit, L"Back..." }
};

menu_screen about_menu = {
	L"About YAUB",
	5,
	about_entries
};

// Main menu entries
#define NUM_MAIN_MENU_ENTRIES 2
menu_entry *main_menu_entries[] = {
	(menu_entry *) &(menu_entry_subscreen) { menu_type_subscreen, L"About YAUB", &about_menu },
	&(menu_entry) { menu_type_exit, L"Exit" }
};

menu_screen main_menu = {
	L"Select what OS to boot into",
	0,
	NULL
};

efi_device_path_protocol *
get_self_volume_dp()
{
	efi_status status;
	efi_device_path_protocol *dp;

	status = bs->handle_protocol(
		self_loaded_image->device_handle,
		&(efi_guid) EFI_DEVICE_PATH_PROTOCOL_GUID,
		(void **) &dp);
	if (EFI_ERROR(status)) {
		abort(L"Error locating self volume device path!", status);
	}

	return dp;
}

static
void
start_efi_image(efi_ch16 *path, efi_ch16 *flags)
{
	efi_status status;
	efi_in_key key;
	efi_handle child_image_handle;
	efi_device_path_protocol *image_dp;

	/* Draw a banner for the image */
	menu_clearscreen();
	menu_banner(L"Running child image");

	/* Get rid of util structures before starting a child image */
	fini_util();

	/* Start the image */
	image_dp = append_filepath_device_path(get_self_volume_dp(), path);
	status = bs->load_image(
		false,
		self_image_handle,
		image_dp,
		NULL,
		0,
		&child_image_handle);
	status = bs->start_image(child_image_handle, NULL, NULL);

done:
	free(image_dp);

	/* If the image returns re-init ourselves */
	init_util(self_image_handle, st);
	menu_init();

	/* Wait for a keypress before re-drawing the menu */
	menu_clearscreen();
	print(L"Started image returned! Press any key to continue!\r\n");
	menu_wait_for_key(&key);
}

efi_status
efiapi
efi_main(efi_handle image_handle, efi_system_table *system_table)
{
	menu_entry *selected;

	menu_entry **merged_entries;
	menu_entry_exec **boot_entries;
	efi_size boot_entries_size;

	init_util(image_handle, system_table);
	menu_init();

	// Read menu entries
	get_entries(&boot_entries_size, &boot_entries);

	// Merge pre-defined and dynamic entries
	merged_entries = malloc(sizeof(menu_entry *) * (boot_entries_size + NUM_MAIN_MENU_ENTRIES));
	memcpy(merged_entries, boot_entries, sizeof(menu_entry *) * boot_entries_size);
	memcpy((void *) (((efi_size) merged_entries) + sizeof(menu_entry *) * boot_entries_size), main_menu_entries, sizeof(menu_entry *) * NUM_MAIN_MENU_ENTRIES);

	main_menu.entry_count = boot_entries_size + NUM_MAIN_MENU_ENTRIES;
	main_menu.entries = merged_entries;

	for (;;) {
		selected = menu_run(&main_menu, 0);
		switch (selected->type) {
		case menu_type_exit:
			goto done;
		case menu_type_exec:
			start_efi_image(((menu_entry_exec *) selected)->path, ((menu_entry_exec *) selected)->flags);
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
