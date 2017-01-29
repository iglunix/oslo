/*
 * config.h
 *
 *  Created on: Jan 28, 2017
 *      Author: kmate
 */

#ifndef INCLUDE_CONFIG_H_
#define INCLUDE_CONFIG_H_

/* On-disk location of the boot entry definition file */
#define ENTRIES_FILE_NAME L"\\efi\\yaub\\entries.ini"

/* Read all boot entries */
void get_entries(uintn_t *returned_entries, menu_entry_exec_t ***entries);

#endif /* INCLUDE_CONFIG_H_ */
