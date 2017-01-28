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
		default:
			break;
		}
	}

done:
	menu_clearscreen();
	fini_util();
	return EFI_SUCCESS;
}
