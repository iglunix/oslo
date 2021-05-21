#ifndef CONFIG_H
#define CONFIG_H

//
// Path to the confiugration file
//
#define ENTRIES_FILE_NAME L"\\efi\\yaub\\entries.ini"

//
// Add the entries from the configuration file to a menu screen
//
void add_boot_entries(menu_screen **menu);

#endif
