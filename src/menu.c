#include <efi.h>
#include <efiutil.h>
#include "menu.h"

// Menu colors
#define DEFAULT_COLOR  EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK
#define SELECTED_COLOR EFI_BLACK     | EFI_BACKGROUND_LIGHTGRAY

// Banner height
#define BANNER_HEIGHT 3

// Menu globals
static efi_size cols, rows;
static efi_event timer_event;

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

void menu_wait_for_key(efi_in_key *key)
{
    efi_size index;

    bs->wait_for_event(1, &st->con_in->wait_for_key, &index);
    st->con_in->read_key(st->con_in, key);
}

void menu_init()
{
    efi_status status;

    status = st->con_out->query_mode(st->con_out,
        st->con_out->mode->mode, &cols, &rows);
    status = st->con_out->enable_cursor(st->con_out, false);
    status = bs->create_event(EVT_TIMER,
        TPL_APPLICATION, NULL, NULL, &timer_event);
    if (EFI_ERROR(status))
        efi_abort(L"Failed to create timer event!", EFI_ABORTED);
}

void menu_fini()
{
    efi_status status = bs->close_event(timer_event);
    if (EFI_ERROR(status))
        efi_abort(L"Failed to close timer event!", EFI_ABORTED);
}

void menu_clearscreen()
{
    st->con_out->set_attr(st->con_out, DEFAULT_COLOR);
    st->con_out->clear_screen(st->con_out);
}

void menu_draw_banner(efi_ch16 *banner_text)
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

static void menu_draw_entries(menu_screen *screen)
{
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

//
// Convert seconds to 100ns
//
#define SEC_TO_100NS(secs) (secs) * 10000000

//
// Wait for a timer or keypress
//
static void wait_for_timer_or_key(efi_event timer_event, efi_in_key *key)
{
    efi_event events[2];
    efi_size index;
    events[0] = timer_event;
    events[1] = st->con_in->wait_for_key;
    bs->wait_for_event(ARRAY_SIZE(events), events, &index);
    switch (index) {
    case 0:
        // Fake enter press on timeout
        key->scan = 0;
        key->c = L'\n';
        break;
    case 1:
        // Read real key
        st->con_in->read_key(st->con_in, key);
        break;
    }
}

menu_entry *menu_run(menu_screen *screen)
{
    efi_status status;

    // Set timer of needed
    if (screen->timeout > 0) {
        status = bs->set_timer(timer_event, timer_relative,
            SEC_TO_100NS(screen->timeout));
        if (EFI_ERROR(status))
            efi_abort(L"Failed to set timer!", EFI_ABORTED);
    } else if (screen->timeout == 0) {
        goto activate_entry;
    }

    // Draw menu header and entries
    menu_draw_banner(screen->title);
    menu_draw_entries(screen);

    // Wait for user input or timeout
    for (;;) {
        efi_in_key key;
        wait_for_timer_or_key(timer_event, &key);

        // First try to take action based on the scancode
        switch (key.scan) {
        case 0x01: // Up arrow
            if (!screen->selected_entry)
                continue;
            --screen->selected_entry;
            menu_draw_entries(screen);
            break;
        case 0x02: // Down arrow
            if (screen->selected_entry == screen->entry_count - 1)
                continue;
            ++screen->selected_entry;
            menu_draw_entries(screen);
            break;
        }

        // Than try the character code
        if (key.c == L'\n' || key.c == L'\r' || key.c == L' ') {
activate_entry:
            // Disable the timeout if an option got selected
            screen->timeout = -1;

            // Take action based on entry type
            menu_entry *submenu_entry;

            switch (screen->entries[screen->selected_entry].type) {
            case menu_type_subscreen:
                // Display screen for submenu entry
                submenu_entry =
                    menu_run(screen->entries[screen->selected_entry].subscreen);

                switch (submenu_entry->type) {
                case menu_type_exit:
                    // Continue current menu, if the submenu was exited
                    break;
                default:
                    // Return the entry if something was choose from the submenu
                    return submenu_entry;
                }

                // Re-draw current menu
                menu_draw_banner(screen->title);
                menu_draw_entries(screen);
                break;
            case menu_type_info:
                // Do nothing on info entries
                break;
            default:
                // Return selected entry
                return screen->entries + screen->selected_entry;
            }
        }
    }
}
