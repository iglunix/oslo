#include <efi.h>
#include <util.h>

// globals
efi_handle_t self_image_handle;
efi_loaded_image_protocol_t *self_loaded_image;
efi_system_table_t *st;
efi_boot_services_t *bs;

void init_util(efi_handle_t image_handle, efi_system_table_t *system_table)
{
	efi_status_t status;

	self_image_handle = image_handle;
	st = system_table;
	bs = system_table->boot_services;
	// Locate self loaded image
	status = bs->handle_protocol(self_image_handle, &(efi_guid_t) EFI_LOADED_IMAGE_PROTOCOL_GUID, (void **) &self_loaded_image);
	if (EFI_ERROR(status)) {
		abort(L"Error handling efi_loaded_image_protocol_t for the image handle!\r\n");
	}
}

efi_char16_t digits[] = L"0123456789ABCDEFGHIJKLMNOPQRSTOVWXYZ";
efi_char16_t putc_buffer[] = { 0, 0 };
#define puts(x) st->con_out->output_string(st->con_out, x);
void puthex(uintn_t number)
{
	puts(L"0x");
	for (size_t shift = sizeof(uintn_t) * 8 - 4;; shift -= 4) {
		putc_buffer[0] = digits[(number >> shift) & 0xf];
		puts(putc_buffer);
		if (shift == 0)
			return;
	}
}

void abort(efi_char16_t *error_msg)
{
	st->con_out->output_string(st->con_out, error_msg);
	// TODO: call exit
	// Hang the system if exiting fails
	asm volatile ("cli");
	for (;;) {
		asm volatile("hlt");
	}
}

void bzero(void *buffer, size_t len)
{
	for (size_t i = 0; i < len; ++i) {
		((uint8_t *) buffer)[i] = 0;
	}
}

void *malloc(size_t size)
{
	void *buffer;
	efi_status_t status;

	status = bs->allocate_pool(efi_loader_data, size, &buffer);
	if (EFI_ERROR(status)) {
		return NULL;
	}

	bzero(buffer, size);

	return buffer;
}

void free(void *buffer)
{
	bs->free_pool(buffer);
}
