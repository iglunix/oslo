#include <efi.h>
#include <efiutil.h>
#include "menu.h"
#include "config.h"

// About sub-menu
menu_screen about_menu = {
	.title = L"About YAUB",
	.timeout = -1,
	.selected_entry = 4,
	.entry_count = 5,
	.entries = {
		{ menu_type_info, L"Welcome to YAUB (Built for: " BUILD_ARCH ")!" },
		{ menu_type_info, L"This program is licensed under the ISC license" },
		{ menu_type_info, L"and the source code is available at:" },
		{ menu_type_info, L"https://github.com/kukrimate/yaub" },
		{ menu_type_exit, L"Back..." },
	}
};

// Main menu fixed entries
menu_entry main_menu_fixed[] = {
	{
		.type = menu_type_subscreen,
		.text = L"About YAUB",
		.subscreen = &about_menu
	},
	{
		.type = menu_type_exit,
		.text = L"Exit"
	}
};

static efi_device_path_protocol *get_self_volume_dp()
{
	efi_status status;
	efi_loaded_image_protocol *self_loaded_image;
	efi_device_path_protocol *dp;

	status = bs->handle_protocol(self_image_handle,
		&(efi_guid) EFI_LOADED_IMAGE_PROTOCOL_GUID,
		(void **) &self_loaded_image);
	if (EFI_ERROR(status))
		goto err;

	status = bs->handle_protocol(self_loaded_image->device_handle,
		&(efi_guid) EFI_DEVICE_PATH_PROTOCOL_GUID,
		(void **) &dp);
	if (EFI_ERROR(status))
		goto err;

	return dp;
err:
	efi_abort(L"Error locating self volume device path!", status);
}

static void start_efi_image(efi_ch16 *path, efi_ch16 *flags)
{
	efi_status status;
	efi_in_key key;
	efi_handle child_image_handle;
	efi_device_path_protocol *image_dp;

	// De-init menu before running image
	menu_clearscreen();
	menu_fini();

	// Start the image
	image_dp = append_filepath_device_path(get_self_volume_dp(), path);
	status = bs->load_image(false, self_image_handle, image_dp, NULL, 0,
		&child_image_handle);
	status = bs->start_image(child_image_handle, NULL, NULL);

done:
	efi_free(image_dp);

	// Re-init menu on image exit
	menu_init();
	menu_clearscreen();

	// Wait for keypress before returning
	efi_print(L"Application exited with status: %p!\n"
			  L"Press any key to continue!\n", status);
	menu_wait_for_key(&key);
}

efi_status efiapi efi_main(efi_handle image_handle, efi_system_table *system_table)
{
	menu_screen *main_menu;
	efi_size entry_idx;
	menu_entry *selected;

	efi_init(image_handle, system_table);

	menu_init();
	menu_clearscreen();

	// Setup main menu
	main_menu = efi_alloc(sizeof(menu_screen));
	main_menu->title = L"Select boot option";
	main_menu->timeout = -1;
	main_menu->selected_entry = 0;
	main_menu->entry_count = 0;

	// Add boot entries to menu
	add_boot_entries(&main_menu);
	// Add fixed entries
	menu_add_entries(&main_menu, main_menu_fixed, ARRAY_SIZE(main_menu_fixed));

	for (;;) {
		selected = menu_run(main_menu);
		switch (selected->type) {
		case menu_type_exit:
			goto done;
		case menu_type_exec:
			start_efi_image(selected->path, selected->flags);
			break;
		default:
			break;
		}
	}

done:
	efi_free(main_menu);
	menu_clearscreen();
	menu_fini();
	return EFI_SUCCESS;
}
