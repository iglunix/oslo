#include <efi.h>
#include <util.h>
#include <menu.h>

// About submenu
menu_entry_t *about_entries[] = {
	&(menu_entry_t) { menu_entry_info, L"Welcome to YAUB (Yet Another Uefi Bootloader)!" },
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
menu_entry_subscreen_t main_menu_about = {
	menu_entry_subscreen,
	L"About YAUB",
	&about_menu
};

menu_entry_t main_menu_exit = {
	menu_entry_exit,
	L"Exit"
};

menu_entry_t *main_menu_entries[] = {
	(menu_entry_t *) &main_menu_about,
	&main_menu_exit
};

menu_screen_t main_menu = {
	L"Select a boot device",
	2,
	main_menu_entries
};

efi_status_t efi_func efi_main(efi_handle_t image_handle, efi_system_table_t *system_table)
{
	menu_entry_t *selected;

	init_util(image_handle, system_table);
	menu_init();

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
	return EFI_SUCCESS;
}
