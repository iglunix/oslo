#ifndef _EFI_SIMPLE_TEXT_IN_H
#define _EFI_SIMPLE_TEXT_IN_H

typedef struct {
	uint16_t     scan;
	efi_char16_t c;
} efi_in_key_t;

typedef struct _efi_simple_text_in_protocol efi_simple_text_in_protocol_t;
struct _efi_simple_text_in_protocol {
	efi_status_t (efi_func *reset)    (efi_simple_text_in_protocol_t* self, efi_bool_t ext_verf);
	efi_status_t (efi_func *read_key) (efi_simple_text_in_protocol_t* self, efi_in_key_t* key);
	efi_event_t  wait_for_key;
};

#endif
