#include <efi.h>
#include <util.h>

// globals
efi_handle_t self_image_handle;
efi_loaded_image_protocol_t *self_loaded_image;
efi_simple_file_system_protocol_t *self_volume;
efi_file_protocol_t *self_root_dir;
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
		abort(L"Error handling efi_loaded_image_protocol_t for the image handle!\r\n", status);
	}
	// Locate the volume we booted from
	status = bs->handle_protocol(self_loaded_image->device_handle, &(efi_guid_t) EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID,
			(void **) &self_volume);
	if (EFI_ERROR(status)) {
		abort(L"Error locating self volume!\r\n", status);
	}
	// Open the self volume
	status = self_volume->open_volume(self_volume, &self_root_dir);
	if (EFI_ERROR(status)) {
		abort(L"Error opening self volume!\r\n", status);
	}
}

void fini_util()
{
	efi_status_t status;

	status = self_root_dir->close(self_root_dir);
	if (EFI_ERROR(status)) {
		abort(L"Error closing self volume!\r\n", status);
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

void abort(efi_char16_t *error_msg, efi_status_t status)
{
	st->con_out->output_string(st->con_out, error_msg);
	bs->exit(self_image_handle, status, 0, NULL);

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

void memcpy(void *dest, void *src, size_t n)
{
	for (size_t i = 0; i < n; ++i) {
		((uint8_t *) dest)[i] = ((uint8_t *) src)[i];
	}
}

void *realloc(void *oldptr, size_t oldsize, size_t newsize)
{
	void *newptr;

	/* Allocate the new region */
	newptr = malloc(newsize);

	/* Check if is oldptr NULL */
	if (oldptr == NULL) {
		return newptr;
	}

	/* Copy over the contents from the old to the new region */
	memcpy(newptr, oldptr, oldsize);

	/* Free the old memory region */
	free(oldptr);

	/* Return the pointer to the new memory region */
	return newptr;
}

void *malloc(size_t size)
{
	void *buffer;
	efi_status_t status;

	status = bs->allocate_pool(efi_loader_data, size, &buffer);
	if (EFI_ERROR(status)) {
		abort(L"Dynamic memory allocation failed!\r\n", status);
	}

	bzero(buffer, size);

	return buffer;
}

void free(void *buffer)
{
	bs->free_pool(buffer);
}

// String utils
size_t strlen(efi_char16_t *str)
{
	efi_char16_t *p = str;
	for (; *p; ++p);
	return p - str;
}

size_t ascii_strlen(char *str)
{
	char *p = str;
	for (; *p; ++p);
	return p - str;
}

size_t strsize(efi_char16_t *str)
{
	return (strlen(str) + 1) * sizeof(efi_char16_t);
}

// Length of an EFI device path node
#define dp_node_len(node)  (node->length[0] | (node->length[1] << 8))
#define dp_next_node(node) ((efi_device_path_protocol_t *) (((uint8_t *) node) + dp_node_len(node)))
#define dp_add_pointer(x, y) (efi_device_path_protocol_t *) ((uintn_t) x + (uintn_t) y)

// Determine the length of an EFI device path
static size_t get_dp_len(efi_device_path_protocol_t *dp)
{
	efi_device_path_protocol_t *first;

	first = dp;

	while (dp->type != END_DEVICE_PATH_TYPE) {
		dp = dp_next_node(dp);
	}

	return (uintn_t) dp - (uintn_t) first;
}

static void fill_end_dp_node(efi_device_path_protocol_t *end_dp)
{
	end_dp->type = END_DEVICE_PATH_TYPE;
	end_dp->sub_type = END_ENTIRE_DEVICE_PATH_SUBTYPE;
	end_dp->length[0] = sizeof(efi_device_path_protocol_t);
	end_dp->length[1] = 0;
}

static void fill_file_path_dp_node(filepath_device_path_t *node, efi_char16_t *str, size_t len)
{
	node->header.type = MEDIA_DEVICE_PATH;
	node->header.sub_type = MEDIA_FILEPATH_DP;
	node->header.length[0] = sizeof(efi_device_path_protocol_t) + len;
	node->header.length[1] = 0;
	memcpy(node->path_name, str, len);
}

// Merge two EFI device paths together (only the first instances are used)
efi_device_path_protocol_t *merge_device_paths(efi_device_path_protocol_t *first, efi_device_path_protocol_t *second)
{
	uintn_t first_len;
	uintn_t second_len;
	efi_device_path_protocol_t *result;

	first_len = get_dp_len(first);
	second_len = get_dp_len(second);

	result = malloc(first_len + second_len + sizeof(efi_device_path_protocol_t));

	// Copy the contents of the device paths
	memcpy(result, first, first_len);
	memcpy(dp_add_pointer(result, first_len), second, second_len);

	// Create the end node for the resulting device path
	fill_end_dp_node(dp_add_pointer(result, (first_len + second_len)));

	// Return
	return result;
}

// Append a filepath device path node to the end of an EFI device path
efi_device_path_protocol_t *append_filepath_device_path(efi_device_path_protocol_t *base, efi_char16_t *file_path)
{
	efi_device_path_protocol_t *result;
	size_t base_len, file_path_len;

	base_len = get_dp_len(base);
	file_path_len = strsize(file_path);

	result = malloc(base_len + 2 * sizeof(efi_device_path_protocol_t) + file_path_len);

	memcpy(result, base, base_len);
	fill_file_path_dp_node((filepath_device_path_t *) dp_add_pointer(result, base_len), file_path, file_path_len);
	fill_end_dp_node(dp_add_pointer(result, (base_len + sizeof(efi_device_path_protocol_t) + file_path_len)));

	return result;
}

efi_status_t locate_all_handles(efi_guid_t *protocol, uintn_t *num_handles, efi_handle_t **out_buffer)
{
	efi_status_t status;
	uintn_t buffer_size;

	buffer_size = sizeof(efi_handle_t);
retry:
	*out_buffer = malloc(buffer_size);

	status = bs->locate_handle(by_protocol, protocol, NULL, &buffer_size, *out_buffer);
	if (status == EFI_BUFFER_TOO_SMALL) {
		free(*out_buffer);
		goto retry;
	}
	if (EFI_ERROR(status)) {
		return status;
	}
	*num_handles = buffer_size / sizeof(efi_handle_t);
	return status;
}
