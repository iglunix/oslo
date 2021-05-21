#ifndef MENU_H
#define MENU_H

// Defines the type a menu entry can have
typedef enum {
    menu_type_info,
    menu_type_subscreen,
    menu_type_exec,
    menu_type_exit
} menu_entry_type;

// Forward declaration for a menu screen
typedef struct menu_screen menu_screen;

// Represents a menu entry
typedef struct {
    menu_entry_type type;
    efi_ch16        *text;
    union {
        /* menu_type_subscreen */
        struct {
            menu_screen *subscreen;
        };
        /* menu_type_exec */
        struct {
            efi_ch16 *path;
            efi_ch16 *flags;
        };
    };
} menu_entry;

// Represents a menu scren
struct menu_screen {
    efi_ch16    *title;
    efi_ssize   timeout;
    efi_size    selected_entry;
    efi_size    entry_count;
    menu_entry  entries[];
};

/* Add an entry to a menu */
void menu_add_entries(menu_screen **menu, menu_entry *entry, efi_size cnt);

/* Wait for a keypress */
void menu_wait_for_key(efi_in_key *key);

/* Display a banner */
void menu_draw_banner(efi_ch16 *banner_text);

//
// Initialize the menu handling code
//
void menu_init();

//
// De-initialize menu handling code
//
void menu_fini();

//
// Clear the screen
//
void menu_clearscreen();

//
// Run a menu
//
menu_entry *menu_run(menu_screen *screen);

#endif
