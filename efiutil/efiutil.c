/*
 * UEFI helper functions
 */
#include <efi.h>
#include <efiutil.h>

efi_handle self_image_handle;
efi_system_table *st;
efi_boot_services *bs;
efi_runtime_services *rt;

void efi_init(efi_handle image_handle, efi_system_table *system_table)
{
	self_image_handle = image_handle;
	st = system_table;
	bs = system_table->boot_services;
	rt = system_table->runtime_services;
}

void efi_abort(efi_ch16 *error_msg, efi_status status)
{
	efi_print(error_msg);
	bs->exit(self_image_handle, status, 0, NULL);

	/* We can't do much if exit fails */
	for (;;)
		;
}

void efi_assert(efi_bool condition, efi_ch16 *error_msg)
{
	if (!condition)
		efi_abort(error_msg, EFI_ABORTED);
}

void *efi_alloc(efi_size size)
{
	efi_status status;
	void *buffer;

	status = bs->allocate_pool(efi_loader_data, size, &buffer);
	if (EFI_ERROR(status))
		efi_abort(L"Cannot allocate memory!\n", status);
	return buffer;
}

void efi_free(void *buffer)
{
	bs->free_pool(buffer);
}

void *efi_realloc(void *oldptr, efi_size oldsize, efi_size newsize)
{
	void *newptr;

	/* Allocate the new region */
	newptr = efi_alloc(newsize);

	/* Check if is oldptr is NULL */
	if (oldptr == NULL) {
		return newptr;
	}

	/* Copy over the contents from the old to the new region */
	memcpy(newptr, oldptr, oldsize);

	/* Free the old memory region */
	efi_free(oldptr);

	/* Return the pointer to the new memory region */
	return newptr;
}

efi_ssize efi_strcmp(efi_ch16 *str1, efi_ch16 *str2)
{
    while (*str1 == *str2++) {
        if (*str1++ == 0) {
            return 0;
        }
    }
    return *str1 - str2[-1];
}

efi_size efi_strlen(efi_ch16 *str)
{
	efi_ch16 *p = str;
	for (; *p; ++p);
	return p - str;
}

efi_size efi_strsize(efi_ch16 *str)
{
	return (efi_strlen(str) + 1) * sizeof(efi_ch16);
}

// Length of an EFI device path node
#define dp_node_len(node)  (node->length[0] | (node->length[1] << 8))
#define dp_next_node(node) ((efi_device_path_protocol *) (((efi_u8 *) node) + dp_node_len(node)))
#define dp_add_pointer(x, y) (efi_device_path_protocol *) ((efi_size) x + (efi_size) y)

// Determine the length of an EFI device path
static efi_size get_dp_len(efi_device_path_protocol *dp)
{
	efi_device_path_protocol *first;

	first = dp;

	while (dp->type != END_DEVICE_PATH_TYPE) {
		dp = dp_next_node(dp);
	}

	return (efi_size) dp - (efi_size) first;
}

static void fill_end_dp_node(efi_device_path_protocol *end_dp)
{
	end_dp->type = END_DEVICE_PATH_TYPE;
	end_dp->sub_type = END_ENTIRE_DEVICE_PATH_SUBTYPE;
	end_dp->length[0] = sizeof(efi_device_path_protocol);
	end_dp->length[1] = 0;
}

static void fill_file_path_dp_node(filepath_device_path *node, efi_ch16 *str, efi_size len)
{
	node->header.type = MEDIA_DEVICE_PATH;
	node->header.sub_type = MEDIA_FILEPATH_DP;
	node->header.length[0] = sizeof(efi_device_path_protocol) + len;
	node->header.length[1] = 0;
	memcpy(node->path_name, str, len);
}

efi_device_path_protocol *merge_device_paths(efi_device_path_protocol *first,
	efi_device_path_protocol *second)
{
	efi_size first_len;
	efi_size second_len;
	efi_device_path_protocol *result;

	first_len = get_dp_len(first);
	second_len = get_dp_len(second);

	result = efi_alloc(first_len + second_len + sizeof(efi_device_path_protocol));

	// Copy the contents of the device paths
	memcpy(result, first, first_len);
	memcpy(dp_add_pointer(result, first_len), second, second_len);

	// Create the end node for the resulting device path
	fill_end_dp_node(dp_add_pointer(result, (first_len + second_len)));

	// Return
	return result;
}

efi_device_path_protocol *append_filepath_device_path(
	efi_device_path_protocol *base, efi_ch16 *file_path)
{
	efi_device_path_protocol *result;
	efi_size base_len, file_path_len;

	base_len = get_dp_len(base);
	file_path_len = efi_strsize(file_path);

	result = efi_alloc(base_len + 2 * sizeof(efi_device_path_protocol) + file_path_len);

	memcpy(result, base, base_len);
	fill_file_path_dp_node((filepath_device_path *) dp_add_pointer(result, base_len), file_path, file_path_len);
	fill_end_dp_node(dp_add_pointer(result, (base_len + sizeof(efi_device_path_protocol) + file_path_len)));

	return result;
}

efi_status locate_all_handles(efi_guid *protocol, efi_size *num_handles,
	efi_handle **out_buffer)
{
#ifdef USE_EFI110
	return bs->locate_handle_buffer(by_protocol, protocol, NULL, num_handles, out_buffer);
#else
	efi_status status;
	efi_size buffer_size;

	buffer_size = sizeof(efi_handle);
retry:
	*out_buffer = efi_alloc(buffer_size);

	status = bs->locate_handle(by_protocol, protocol, NULL, &buffer_size, *out_buffer);
	if (status == EFI_BUFFER_TOO_SMALL) {
		efi_free(*out_buffer);
		goto retry;
	}
	if (EFI_ERROR(status)) {
		return status;
	}
	*num_handles = buffer_size / sizeof(efi_handle);
	return status;
#endif
}

efi_status locate_protocol(efi_guid *protocol, void **iface)
{
#ifdef USE_EFI110
	return bs->locate_protocol(protocol, NULL, iface);
#else
	efi_status status;
	efi_size handle_cnt;
	efi_handle *handle_buf;

	handle_buf = NULL;
	status = locate_all_handles(protocol, &handle_cnt, &handle_buf);
	if (EFI_ERROR(status))
		goto done;
	if (!handle_cnt) {
		status = EFI_NOT_FOUND;
		goto done;
	}

	status = bs->handle_protocol(handle_buf[0], protocol, iface);

done:
	if (handle_buf)
		efi_free(handle_buf);

	return status;
#endif
}

efi_status get_file_info(efi_file_protocol *file, efi_file_info **file_info)
{
	efi_status status;
	efi_size bufsize;

	bufsize = 0;
	*file_info = NULL;
retry:
	status = file->get_info(file,
		&(efi_guid) EFI_FILE_INFO_ID,
		&bufsize,
		*file_info);

	if (status == EFI_BUFFER_TOO_SMALL) {
		*file_info = efi_alloc(bufsize);
		goto retry;
	}

	return status;
}
