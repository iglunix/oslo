#ifndef _MENU_H
#define _MENU_H

// Defines the type a menu entry can have
typedef enum {
	menu_entry_info,
	menu_entry_subscreen,
	menu_entry_exec,
	menu_entry_exit
} menu_entry_type_t;

// Represents a menu entry
typedef struct {
	menu_entry_type_t type;
	efi_char16_t     *text;
} menu_entry_t;

// A menu entry that executes an EFI application
typedef struct {
	menu_entry_t  base;
	efi_char16_t *path;
	efi_char16_t *flags;
} menu_entry_exec_t;

// Forward declaration for a menu screen
typedef struct menu_screen menu_screen_t;

// A menu entry that enters a subscreen
typedef struct {
	menu_entry_t   base;
	menu_screen_t *subscreen;
} menu_entry_subscreen_t;

// Represents a menu scren
typedef struct menu_screen {
	efi_char16_t  *title;
	size_t         entry_count;
	menu_entry_t **entries;
} menu_screen_t;

/* Wait for a keypress */
void menu_wait_for_key(efi_in_key_t *key);

/* Display a banner */
void menu_banner(efi_char16_t *banner_text);

// Initialize the menu handling code
void menu_init();

// Clear the screen
void menu_clearscreen();

// Run a menu
menu_entry_t *menu_run(menu_screen_t *screen, size_t default_selected_entry);

#endif
