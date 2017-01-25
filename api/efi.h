#ifndef __EFI_H
#define __EFI_H

#include <stdint.h>
#include <stddef.h>

#ifndef __GNUC__
#error Please use GCC.
#endif

// Native width integer
#if __SIZEOF_POINTER__ == 4
typedef uint32_t uintn_t;
typedef int32_t intn_t;
#elif __SIZEOF_POINTER__ == 8
typedef uint64_t uintn_t;
typedef int64_t intn_t;
#else
#error Unsupported word width.
#endif

// EFI function
#if defined __amd64__ && !defined _WIN32
// Force ms_abi for 64-bit non-win32 compilers
#define efi_func __attribute__((ms_abi))
#else
#define efi_func
#endif

// EFI handles
typedef void* efi_handle_t;
typedef void* efi_event_t;

// EFI guid
typedef struct {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
} efi_guid_t;

// EFI status
typedef uintn_t efi_status_t;
#define EFIERR(x) (x | 0x8000000000000000)
#define EFI_ERROR(x) (((intn_t) x) < 0)

// Status codes
#define EFI_SUCCESS 0
#define EFI_LOAD_ERROR EFIERR(1)
#define EFI_INVALID_PARAMETER EFIERR(2)
#define EFI_UNSUPPORTED EFIERR(3)
#define EFI_BAD_BUFFER_SIZE EFIERR(4)
#define EFI_BUFFER_TOO_SMALL EFIERR(5)
#define EFI_NOT_READY EFIERR(6)
#define EFI_DEVICE_ERROR EFIERR(7)
#define EFI_WRITE_PROTECTED EFIERR(8)
#define EFI_OUT_OF_RESOURCES EFIERR(9)
#define EFI_VOLUME_CORRUPTED EFIERR(10)
#define EFI_VOLUME_FULL EFIERR(11)
#define EFI_NO_MEDIA EFIERR(12)
#define EFI_MEDIA_CHANGED EFIERR(13)
#define EFI_NOT_FOUND EFIERR(14)
#define EFI_ACCESS_DENIED EFIERR(15)
#define EFI_NO_RESPONSE EFIERR(16)
#define EFI_NO_MAPPING EFIERR(17)
#define EFI_TIMEOUT EFIERR(18)
#define EFI_NOT_STARTED EFIERR(19)
#define EFI_ALREADY_STARTED EFIERR(20)
#define EFI_ABORTED EFIERR(21)
#define EFI_ICMP_ERROR EFIERR(22)
#define EFI_TFTP_ERROR EFIERR(23)
#define EFI_PROTOCOL_ERROR EFIERR(24)
#define EFI_INCOMPATIBLE_VERSION EFIERR(25)
#define EFI_SECURITY_VIOLATION EFIERR(26)
#define EFI_CRC_ERROR EFIERR(27)
#define EFI_END_OF_MEDIA EFIERR(28)
#define EFI_END_OF_FILE EFIERR(31)
#define EFI_INVALID_LANGUAGE EFIERR(32)
#define EFI_COMPROMISED_DATA EFIERR(33)
#define EFI_WARN_UNKOWN_GLYPH 1
#define EFI_WARN_DELETE_FAILURE 2
#define EFI_WARN_WRITE_FAILURE 3
#define EFI_WARN_BUFFER_TOO_SMALL 4

// EFI chars
typedef uint16_t efi_char16_t;
typedef uint8_t efi_char8_t;

// EFI bool
typedef uint8_t efi_bool_t;
#define true ((efi_bool_t) 1)
#define false ((efi_bool_t) 0)

// EFI address
typedef uint64_t efi_physical_address_t;

// allocate type
typedef enum {
	allocate_any_pages,
	allocate_max_address,
	allocate_address,
	max_allocate_type
} efi_allocate_type_t;

// memory type
typedef enum {
	efi_reserved_memory_type,
	efi_loader_code,
	efi_loader_data,
	efi_boot_services_code,
	efi_boot_services_data,
	efi_runtime_services_code,
	efi_runtime_services_data,
	efi_conventional_memory,
	efi_unusable_memory,
	efi_acpi_reclaim_memory,
	efi_acpi_memory_nvs,
	efi_memory_mapped_io,
	efi_memory_mapped_io_port_space,
	efi_pal_code,
	efi_max_memory_type
} efi_memory_type_t;

// Forward declarations
typedef struct _efi_system_table efi_system_table_t;

// Protocols
#include <efi_simple_text_out_protocol.h>
#include <efi_simple_text_in_protocol.h>
#include <efi_loaded_image_protocol.h>

// EFI tables
typedef struct {
	uint64_t signature;
	uint32_t revision;
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
} efi_table_header_t;

typedef struct {
	efi_table_header_t hdr;
	// TPL
	void *raise_tpl;
	void *restore_tpl;
	// memory allocation
	efi_status_t (efi_func *allocate_pages)(efi_allocate_type_t type, efi_memory_type_t memory_type, uintn_t pages, efi_physical_address_t *memory);
	efi_status_t (efi_func *free_pages)(efi_physical_address_t memory, uintn_t pages);
	void *get_memory_map;
	efi_status_t (efi_func *allocate_pool)(efi_memory_type_t pool_type, uintn_t size, void **buffer);
	efi_status_t (efi_func *free_pool)(void *buffer);
	// event
	void *create_event;
	void *set_timer;
	efi_status_t (efi_func *wait_for_event)(uintn_t num_events, efi_event_t *event, uintn_t *index);
	void *signal_event;
	void *close_event;
	void *check_event;
	// protocol
	void *install_protocol_interface;
	void *reinstall_protocol_interface;
	void *uninstall_protocol_interface;
	efi_status_t (efi_func *handle_protocol)(efi_handle_t handle, efi_guid_t *protocol, void **interface);
} efi_boot_services_t;

// EFI system table
typedef struct _efi_system_table {
	efi_table_header_t hdr;
	efi_char16_t *vendor;
	uint32_t revision;
	efi_handle_t con_in_handle;
	efi_simple_text_in_protocol_t *con_in;
	efi_handle_t con_out_handle;
	efi_simple_text_out_protocol_t *con_out;
	efi_handle_t std_err_handle;
	efi_simple_text_out_protocol_t *std_err;
	void *runtime_services;
	efi_boot_services_t *boot_services;
	uintn_t cnt_config_entries;
	void *config_entries;
} efi_system_table_t;

#endif
