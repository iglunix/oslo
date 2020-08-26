#ifndef CONFIG_H
#define CONFIG_H

/* On-disk location of the boot entry definition file */
#define ENTRIES_FILE_NAME L"\\efi\\yaub\\entries.ini"

/* Read all boot entries */
void get_entries(efi_size *returned_entries, menu_entry_exec ***entries);

#endif
