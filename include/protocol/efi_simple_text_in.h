//
// EFI simple text input protocol
//
#ifndef EFI_SIMPLE_TEXT_IN_H
#define EFI_SIMPLE_TEXT_IN_H

//
// Unicode control characters
//
#define CHAR_NUL              0x0000
#define CHAR_BACKSPACE        0x0008
#define CHAR_TAB              0x0009
#define CHAR_LINEFEED         0x000A
#define CHAR_CARRIAGE_RETURN  0x000D

//
// EFI scan codes
//
#define SCAN_NULL       0x0000
#define SCAN_UP         0x0001
#define SCAN_DOWN       0x0002
#define SCAN_RIGHT      0x0003
#define SCAN_LEFT       0x0004
#define SCAN_HOME       0x0005
#define SCAN_END        0x0006
#define SCAN_INSERT     0x0007
#define SCAN_DELETE     0x0008
#define SCAN_PAGE_UP    0x0009
#define SCAN_PAGE_DOWN  0x000A
#define SCAN_F1         0x000B
#define SCAN_F2         0x000C
#define SCAN_F3         0x000D
#define SCAN_F4         0x000E
#define SCAN_F5         0x000F
#define SCAN_F6         0x0010
#define SCAN_F7         0x0011
#define SCAN_F8         0x0012
#define SCAN_F9         0x0013
#define SCAN_F10        0x0014
#define SCAN_ESC        0x0017

//
// These are technically not supported by the UEFI spec for this protocol, but
// several existing implementations do
//
#define SCAN_F11        0x0015
#define SCAN_F12        0x0016

typedef struct {
    efi_u16     scan;
    efi_ch16    c;
} efi_in_key;

typedef struct efi_simple_text_in_protocol efi_simple_text_in_protocol;
struct efi_simple_text_in_protocol {
    efi_status (efiapi *reset)    (efi_simple_text_in_protocol *self, efi_bool ext_verf);
    efi_status (efiapi *read_key) (efi_simple_text_in_protocol *self, efi_in_key *key);
    efi_event   wait_for_key;
};

#endif
