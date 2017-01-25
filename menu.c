#include <efi.h>
#include <util.h>
#include <menu.h>

// Wait for a keypress
static inline void wait_for_key()
{
	uintn_t index;
	bs->wait_for_event(1, &st->con_in->wait_for_key, &index);
}

// Menu colors
#define DEFAULT_COLOR efi_lightgray | efi_background_black
#define SELECTED_COLOR efi_black | efi_background_lightgray

// Banner height
#define BANNER_HEIGHT 3

// Menu variables
size_t cols, rows;

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

menu_redraw:
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
	st->con_out->output_string(st->con_out, screen->title);

menu_redraw_entries:
	// Draw the entries
	for (size_t i = 0; i < screen->entry_count; ++i) {
		if (i == menu_index) {
			st->con_out->set_attr(st->con_out, SELECTED_COLOR);
		} else {
			st->con_out->set_attr(st->con_out, DEFAULT_COLOR);
		}

		st->con_out->set_cursor_pos(st->con_out, 0, i + 3);
		st->con_out->output_string(st->con_out, screen->entries[i]->text);
	}

menu_rescan:
	// Read user input
	wait_for_key();
	st->con_in->read_key(st->con_in, &key);

	// First try to take action based on the scancode
	switch (key.scan) {
	case 0x01: // Arrow up
		// convert the index to a signed and check out of bounds
		if (((intn_t) menu_index) - 1 < 0) {
			goto menu_rescan;
		}
		--menu_index;
		goto menu_redraw_entries;
	case 0x02:
		if (menu_index + 1 > screen->entry_count - 1) {
			goto menu_rescan;
		}
		++menu_index;
		goto menu_redraw_entries;
	}

	// Than try the character code
	switch (key.c) {
	case L'\n':
	case L'\r':
	case ' ':
		switch (screen->entries[menu_index]->type) {
		case menu_entry_subscreen: // Display submenu on submenu entries
			submenu_entry = menu_run(((menu_entry_subscreen_t *) screen->entries[menu_index])->subscreen, 0);
			switch (submenu_entry->type) {
			case menu_entry_exit: // Continue on if the submenu was exited
				break;
			default: // Return the entry if something was choosen from the submenu (except exit and info entries)
				return submenu_entry;
			}
			goto menu_redraw;
		case menu_entry_info: // Don't do anything on info entries
			goto menu_rescan;
		default:
			return screen->entries[menu_index];
		}
	}

	goto menu_rescan;
}
