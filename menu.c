#include <efi.h>
#include <util.h>
#include <menu.h>

// Menu colors
#define DEFAULT_COLOR  EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
#define SELECTED_COLOR EFI_BLACK     | EFI_BACKGROUND_LIGHTGRAY

// Banner height
#define BANNER_HEIGHT 3

// Menu variables
size_t cols, rows;

static void menu_entries(menu_screen_t *screen, size_t selected_entry)
{
	// Draw the entries
	for (size_t i = 0; i < screen->entry_count; ++i) {
		if (i == selected_entry) {
			st->con_out->set_attr(st->con_out, SELECTED_COLOR);
		} else {
			st->con_out->set_attr(st->con_out, DEFAULT_COLOR);
		}

		st->con_out->set_cursor_pos(st->con_out, 0, i + 3);
		st->con_out->output_string(st->con_out, screen->entries[i]->text);
	}
}

// Wait for a key press
void menu_wait_for_key()
{
	uintn_t index;
	bs->wait_for_event(1, &st->con_in->wait_for_key, &index);
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
void menu_banner(efi_char16_t *banner_text)
{
	// Clear the screen
	menu_clearscreen();
	// Draw banner
	st->con_out->set_attr(st->con_out, SELECTED_COLOR);
	for (size_t i = 0; i < BANNER_HEIGHT; ++i) {
		st->con_out->set_cursor_pos(st->con_out, 0, i);
		for (size_t j = 0; j < cols; ++j) {
			st->con_out->output_string(st->con_out, L" ");
		}
	}
	st->con_out->set_cursor_pos(st->con_out, 0, BANNER_HEIGHT / 2);
	st->con_out->output_string(st->con_out, banner_text);
}

menu_entry_t *menu_run(menu_screen_t *screen, size_t default_selected_entry)
{
	size_t menu_index;
	efi_in_key_t key;
	menu_entry_t *submenu_entry;

	menu_index = default_selected_entry;

	// Sanity check
	if (screen->entry_count < 1) {
		return NULL;
	}

	menu_banner(screen->title);
	menu_entries(screen, menu_index);

	for (;;) {
		// Read user input
		menu_wait_for_key();
		st->con_in->read_key(st->con_in, &key);

		// First try to take action based on the scancode
		switch (key.scan) {

		case 0x01: // Arrow up
			// convert the index to a signed and check out of bounds
			if (((intn_t) menu_index) - 1 < 0) {
				continue;
			}
			--menu_index;
			menu_entries(screen, menu_index);
			break;

		case 0x02: // Arrow down
			if (menu_index + 1 > screen->entry_count - 1) {
				continue;
			}
			++menu_index;
			menu_entries(screen, menu_index);
			break;

		}

		// Than try the character code
		if (key.c == L'\n' || key.c == L'\r' || key.c == L' ') {
			switch (screen->entries[menu_index]->type) {

			case menu_entry_subscreen: // Display submenu on submenu entries
				submenu_entry = menu_run(((menu_entry_subscreen_t *) screen->entries[menu_index])->subscreen, 0);

				switch (submenu_entry->type) {

				case menu_entry_exit: // Continue on if the submenu was exited
					break;

				default: // Return the entry if something was choosen from the submenu (except exit and info entries)
					return submenu_entry;

				}

				menu_banner(screen->title);
				menu_entries(screen, menu_index);
				break;

			case menu_entry_info: // Don't do anything on info entries
				break;

			default: // Return the choosen entry
				return screen->entries[menu_index];

			} // entry type switch END

		} // character code if END

	} // for loop END
}
