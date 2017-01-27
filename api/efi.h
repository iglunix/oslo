#ifndef __EFI_H
#define __EFI_H

/* Include CPU specific header */
#if defined __amd64__ || defined _M_AMD64
	#include <amd64/cpu.h>
#elif defined __i386__ || defined _M_IX86
	#include <ia32/cpu.h>
#else
	#error "Unsupported CPU architecture."
#endif

// NULL pointer
#define NULL (void *) 0

// EFI handles
typedef void* efi_handle_t;
typedef void* efi_event_t;

// EFI chars
typedef uint16_t efi_char16_t;
typedef uint8_t  efi_char8_t;

// EFI bool
typedef uint8_t efi_bool_t;
#define true  ((efi_bool_t) 1)
#define false ((efi_bool_t) 0)

// EFI guid
typedef struct {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t  data4[8];
} efi_guid_t;

// EFI status
#include <efi_status.h>

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
#include <protocol/efi_simple_text_out.h>
#include <protocol/efi_simple_text_in.h>
#include <protocol/efi_device_path.h>
#include <protocol/efi_loaded_image.h>

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

	// TPL services
	void *raise_tpl;
	void *restore_tpl;

	// Memory allocation services
	efi_status_t (efi_func *allocate_pages)  (efi_allocate_type_t type, efi_memory_type_t memory_type, uintn_t pages, efi_physical_address_t *memory);
	efi_status_t (efi_func *free_pages)      (efi_physical_address_t memory, uintn_t pages);
	void *get_memory_map;
	efi_status_t (efi_func *allocate_pool)   (efi_memory_type_t pool_type, uintn_t size, void **buffer);
	efi_status_t (efi_func *free_pool)       (void *buffer);

	// Event services
	void *create_event;
	void *set_timer;
	efi_status_t (efi_func *wait_for_event)  (uintn_t num_events, efi_event_t *event, uintn_t *index);
	void *signal_event;
	void *close_event;
	void *check_event;

	// Protocol services
	void *install_protocol_interface;
	void *reinstall_protocol_interface;
	void *uninstall_protocol_interface;
	efi_status_t (efi_func *handle_protocol) (efi_handle_t handle, efi_guid_t *protocol, void **interface);
	void *reserved;
	void *register_protocol_notify;
	//efi_status_t (efi_func *locate_handle) ();
} efi_boot_services_t;

// EFI system table
typedef struct _efi_system_table {
	efi_table_header_t              hdr;
	efi_char16_t                   *vendor;
	uint32_t                        revision;
	efi_handle_t                    con_in_handle;
	efi_simple_text_in_protocol_t  *con_in;
	efi_handle_t                    con_out_handle;
	efi_simple_text_out_protocol_t *con_out;
	efi_handle_t                    std_err_handle;
	efi_simple_text_out_protocol_t *std_err;
	void                           *runtime_services;
	efi_boot_services_t            *boot_services;
	uintn_t                         cnt_config_entries;
	void                           *config_entries;
} efi_system_table_t;

#endif
