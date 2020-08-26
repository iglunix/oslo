#ifndef MENU_H
#define MENU_H

// Defines the type a menu entry can have
typedef enum {
	menu_type_info,
	menu_type_subscreen,
	menu_type_exec,
	menu_type_exit
} menu_entry_type;

// Represents a menu entry
typedef struct {
	menu_entry_type	type;
	efi_ch16	*text;
} menu_entry;

// A menu entry that executes an EFI application
typedef struct {
	menu_entry	base;
	efi_ch16	*path;
	efi_ch16	*flags;
} menu_entry_exec;

// Forward declaration for a menu screen
typedef struct menu_screen menu_screen;

// A menu entry that enters a subscreen
typedef struct {
	menu_entry   base;
	menu_screen *subscreen;
} menu_entry_subscreen;

// Represents a menu scren
struct menu_screen {
	efi_ch16	*title;
	efi_size	entry_count;
	menu_entry	**entries;
};

/* Wait for a keypress */
void menu_wait_for_key(efi_in_key *key);

/* Display a banner */
void menu_banner(efi_ch16 *banner_text);

// Initialize the menu handling code
void menu_init();

// Clear the screen
void menu_clearscreen();

// Run a menu
menu_entry *menu_run(menu_screen *screen, efi_size default_selected_entry);

#endif
