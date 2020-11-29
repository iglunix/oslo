#include <efi.h>
#include <efiutil.h>
#include <khelper.h>
#include "menu.h"

// Menu colors
#define DEFAULT_COLOR  EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
#define SELECTED_COLOR EFI_BLACK     | EFI_BACKGROUND_LIGHTGRAY

// Banner height
#define BANNER_HEIGHT 3

// Menu variables
efi_size cols, rows;

static void menu_entries(menu_screen *screen)
{
	// Draw the entries
	for (efi_size i = 0; i < screen->entry_count; ++i) {
		if (i == screen->selected_entry) {
			st->con_out->set_attr(st->con_out, SELECTED_COLOR);
		} else {
			st->con_out->set_attr(st->con_out, DEFAULT_COLOR);
		}

		st->con_out->set_cursor_pos(st->con_out, 0, i + 3);
		st->con_out->output_string(st->con_out, screen->entries[i].text);
	}
}

void menu_add_entries(menu_screen **menu, menu_entry *entry, efi_size cnt)
{
	efi_size oldsize;

	oldsize = sizeof(menu_screen) +
		sizeof(menu_entry) * (*menu)->entry_count;
	*menu = efi_realloc(*menu, oldsize, oldsize + sizeof(menu_entry) * cnt);
	memcpy((*menu)->entries + (*menu)->entry_count,
		entry, sizeof(menu_entry) * cnt);
	(*menu)->entry_count += cnt;
}

// Wait for a key press
void menu_wait_for_key(efi_in_key *key)
{
	efi_size index;

	/* Wait for a key press */
	bs->wait_for_event(1, &st->con_in->wait_for_key, &index);
	/* Read the key */
	st->con_in->read_key(st->con_in, key);
}

void menu_init()
{
	st->con_out->query_mode(st->con_out, st->con_out->mode->mode, &cols, &rows);
	st->con_out->enable_cursor(st->con_out, false);
}

void menu_clearscreen()
{
	st->con_out->set_attr(st->con_out, DEFAULT_COLOR);
	st->con_out->clear_screen(st->con_out);
}
void menu_banner(efi_ch16 *banner_text)
{
	// Clear the screen
	menu_clearscreen();
	// Draw banner
	st->con_out->set_attr(st->con_out, SELECTED_COLOR);
	for (efi_size i = 0; i < BANNER_HEIGHT; ++i) {
		st->con_out->set_cursor_pos(st->con_out, 0, i);
		for (efi_size j = 0; j < cols; ++j) {
			st->con_out->output_string(st->con_out, L" ");
		}
	}
	st->con_out->set_cursor_pos(st->con_out, 0, BANNER_HEIGHT / 2);
	st->con_out->output_string(st->con_out, banner_text);
	st->con_out->set_attr(st->con_out, DEFAULT_COLOR);
}

menu_entry *
menu_run(menu_screen *screen)
{
	efi_in_key key;
	menu_entry *submenu_entry;

	// Sanity check
	if (screen->entry_count < 1) {
		return NULL;
	}

	menu_banner(screen->title);
	menu_entries(screen);

	for (;;) {
		// Read user input
		menu_wait_for_key(&key);

		// First try to take action based on the scancode
		switch (key.scan) {

		case 0x01: // Arrow up
			// convert the index to a signed and check out of bounds
			if (!screen->selected_entry) {
				continue;
			}
			--screen->selected_entry;
			menu_entries(screen);
			break;

		case 0x02: // Arrow down
			if (screen->selected_entry == screen->entry_count - 1) {
				continue;
			}
			++screen->selected_entry;
			menu_entries(screen);
			break;

		}

		// Than try the character code
		if (key.c == L'\n' || key.c == L'\r' || key.c == L' ') {
			switch (screen->entries[screen->selected_entry].type) {

			case menu_type_subscreen: // Display submenu on submenu entries
				submenu_entry =
					menu_run(screen->entries[screen->selected_entry].subscreen);

				switch (submenu_entry->type) {

				case menu_type_exit: // Continue on if the submenu was exited
					break;

				default: /* Return the entry if something was choosen
						 from the submenu (except exit and info entries) */
					return submenu_entry;

				}

				menu_banner(screen->title);
				menu_entries(screen);
				break;

			case menu_type_info: // Do nothing on info entries
				break;

			default: // Return the choosen entry
				return screen->entries + screen->selected_entry;

			} // entry type switch END

		} // character code if END

	} // for loop END
}
