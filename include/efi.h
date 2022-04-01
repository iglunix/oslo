/*
 * EFI Structure definitions
 */
#ifndef EFI_H
#define EFI_H

/* Freestanding compiler headers */
#include <stddef.h>
#include <stdint.h>

/* CPU specific header */
#ifdef __x86_64__
#include <x86_64/cpu.h>
#else
#error unimplemented arch
#endif

// EFI handles
typedef void *efi_handle;

// EFI chars
typedef efi_u8 efi_ch8;
typedef efi_u16 efi_ch16;

// EFI bool
typedef efi_u8 efi_bool;
#define true  ((efi_bool) 1)
#define false ((efi_bool) 0)

// EFI GUID
typedef struct {
    efi_u32 data1;
    efi_u16 data2;
    efi_u16 data3;
    efi_u8  data4[8];
} efi_guid;

// EFI Inteface type
typedef enum {
    efi_native_interface
} efi_interface_type;

// EFI time
typedef struct {
    efi_u16 year;
    efi_u8  month;
    efi_u8  day;
    efi_u8  hour;
    efi_u8  minute;
    efi_u8  second;
    efi_u8  pad1;
    efi_u32 nanosecond;
    efi_i16 time_zone;
    efi_u8  daylight;
    efi_u8  pad2;
} efi_time;

typedef struct {
    efi_u32  resolution;
    efi_u32  accuracy;
    efi_bool sets_to_zero;
} efi_time_cap;

// EFI status
#include <efi_status.h>

// Physical address
typedef efi_u64 efi_physical_address;

// Virtual address
typedef efi_u64 efi_virtual_address;

// Task priority type
typedef efi_size efi_tpl;

#define TPL_APPLICATION 4
#define TPL_CALLBACK    8
#define TPL_NOTIFY      16
#define TPL_HIGH_LEVEL  31

// EFI event type and related definitions
typedef void *efi_event;

#define EVT_TIMER                         0x80000000
#define EVT_RUNTIME                       0x40000000
#define EVT_NOTIFY_WAIT                   0x00000100
#define EVT_NOTIFY_SIGNAL                 0x00000200
#define EVT_SIGNAL_EXIT_BOOT_SERVICES     0x00000201
#define EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE 0x60000202

typedef efi_status (efiapi *efi_event_notify)(efi_event event, void *context);

typedef enum {
    timer_cancel,
    timer_periodic,
    timer_relative,
} efi_timer_delay;

// Allocation type
typedef enum {
    allocate_any_pages,
    allocate_max_address,
    allocate_address,
    max_allocate_type
} efi_allocate_type;

// Memory type
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
} efi_memory_type;

// Memory descriptor
typedef struct {
    efi_u32         type;
    efi_physical_address    start;
    efi_virtual_address virtual_start;
    efi_u64         number_of_pages;
    efi_u64         attribute;
} efi_memory_descriptor;

// Locate search type
typedef enum {
    all_handles,
    by_register_notify,
    by_protocol
} efi_locate_search_type;

// Information entry about a protocol
typedef struct {
    efi_handle agent_handle;
    efi_handle controller_handle;
    efi_u32 attrib;
    efi_u32 open_count;
} efi_open_protocol_information_entry;

// Protocol headers depend on the system table
typedef struct efi_system_table efi_system_table;

// Protocol headers
#include <protocol/efi_simple_text_out.h>
#include <protocol/efi_simple_text_in.h>
#include <protocol/efi_device_path.h>
#include <protocol/efi_loaded_image.h>
#include <protocol/efi_file_protocol.h>
#include <protocol/efi_simple_file_system.h>
#include <protocol/efi_graphics_output.h>
#include <protocol/efi_unicode_collation.h>
#include <protocol/efi_hii_database.h>

// Non-protocol GUIDs
#define EFI_GLOBAL_VARIABLE \
    { 0x8BE4DF61, 0x93CA, 0x11d2, 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, \
        0x2B, 0x8C }

// EFI tables
typedef struct {
    efi_u64 signature;
    efi_u32 revision;
    efi_u32 header_size;
    efi_u32 crc32;
    efi_u32 reserved;
} efi_table_header;

typedef struct {
    efi_table_header hdr;

    // TPL services
    efi_tpl (efiapi *raise_tpl)(efi_tpl new_tpl);
    void (efiapi *restore_tpl)(efi_tpl old_tpl);

    // Memory allocation services
    efi_status (efiapi *allocate_pages)(
        efi_allocate_type type,
        efi_memory_type memory_type,
        efi_size pages,
        efi_physical_address *memory);
    efi_status (efiapi *free_pages)(efi_physical_address memory, efi_size pages);
    efi_status (efiapi *get_memory_map)(
        efi_size *memory_map_size,
        efi_memory_descriptor *memory_map,
        efi_size *map_key,
        efi_size *descriptor_size,
        efi_u32 *descriptor_version);
    efi_status (efiapi *allocate_pool)(efi_memory_type pool_type, efi_size size, void **buffer);
    efi_status (efiapi *free_pool)(void *buffer);

    // Event services
    efi_status (efiapi *create_event)(
        efi_u32 type,
        efi_tpl notify_tpl,
        efi_event_notify notify_function,
        void *notify_context,
        efi_event *event);
    efi_status (efiapi *set_timer)(efi_event event, efi_timer_delay type, efi_u64 trigger_time);
    efi_status (efiapi *wait_for_event)(efi_size num_events, efi_event *event, efi_size *index);
    efi_status (efiapi *signal_event)(efi_event event);
    efi_status (efiapi *close_event)(efi_event event);
    efi_status (efiapi *check_event)(efi_event event);

    // Protocol services
    efi_status (efiapi *install_protocol_interface)(
        efi_handle *handle,
        efi_guid *protocol,
        efi_interface_type interface_type,
        void *interface);
    efi_status (efiapi *reinstall_protocol_interface)(
        efi_handle handle,
        efi_guid *protocol,
        void *old_interface,
        void *new_interface);
    efi_status (efiapi *uninstall_protocol_interface)(efi_handle handle, efi_guid *protocol, void *interface);
    efi_status (efiapi *handle_protocol)(efi_handle handle, efi_guid *protocol, void **interface);
    void *reserved;
    efi_status (efiapi *register_protocol_notify)(efi_guid *protocol, efi_event event, void **registration);
    efi_status (efiapi *locate_handle)(
        efi_locate_search_type search_type,
        efi_guid *protocol,
        void *search_key,
        efi_size *buffer_size,
        efi_handle *buffer);
    efi_status (efiapi *locate_device_path)(
        efi_guid *protocol,
        efi_device_path_protocol **device_path,
        efi_handle *device);
    efi_status (efiapi *install_configuration_table)(efi_guid *guid, void *table);

    // Image services
    efi_status (efiapi *load_image)(
        efi_bool boot_policy,
        efi_handle parent_image_handle,
        efi_device_path_protocol *device_path,
        void *source_buffer,
        efi_size source_size,
        efi_handle *image_handle);
    efi_status (efiapi *start_image)(efi_handle image_handle, efi_size *exit_data_size, efi_ch16 **exit_data);
    efi_status (efiapi *exit)(
        efi_handle image_handle,
        efi_status exit_status,
        efi_size exit_data_size,
        efi_ch16 *exit_data);
    efi_status (efiapi *unload_image)(efi_handle image_handle);
    efi_status (efiapi *exit_boot_services)(efi_handle image_handle, efi_size map_key);

    // Misc services
    efi_status (efiapi *get_next_monotonic_count)(efi_u64 *count);
    efi_status (efiapi *stall)(efi_size microseconds);
    efi_status (efiapi *set_watchdog_timer)(
        efi_size timeout,
        efi_u64 watchdog_code,
        efi_size data_size,
        efi_ch16 *watchdog_data);

    // Driver support services
    efi_status (efiapi *connect_controller)(
        efi_handle controller_handle,
        efi_handle *driver_image_handle,
        efi_device_path_protocol *remaining_device_path,
        efi_bool recursive);
    efi_status (efiapi *disconnect_controller)(
        efi_handle controller_handle,
        efi_handle driver_image_handle,
        efi_handle child_handle);

    // Open and close protocol services
    efi_status (efiapi *open_protocol)(
        efi_handle handle,
        efi_guid *protocol,
        void **interface,
        efi_handle agent_handle,
        efi_handle controller_handle,
        efi_u32 attrib);
    efi_status (efiapi *close_protocol)(
        efi_handle handle,
        efi_guid *protocol,
        efi_handle agent_handle,
        efi_handle controller_handle);
    efi_status (efiapi *open_protocol_information)(
        efi_handle handle,
        efi_guid *protocol,
        efi_open_protocol_information_entry **entry_buffer,
        efi_size *entry_count);

    // Library services
    efi_status (efiapi *protocols_per_handle)(
        efi_handle handle,
        efi_guid ***protocol_buffer,
        efi_size *protocol_buffer_count);
    efi_status (efiapi *locate_handle_buffer)(
        efi_locate_search_type search_type,
        efi_guid *protocol,
        void *search_key,
        efi_size *handle_count,
        efi_handle **handle_buffer);
    efi_status (efiapi *locate_protocol)(efi_guid *protocol, void *registration, void **interface);
    efi_status (efiapi *install_multiple_protocol_interfaces)(efi_handle *handle, ...);
    efi_status (efiapi *uninstall_multiple_protocol_interfaces)(efi_handle handle, ...);

    // CRC32
    efi_status (efiapi *calculate_crc32)(void *data, efi_size data_size, efi_u32 *crc32);

    // Misc services
    void (efiapi *copy_mem)(void *dest, void *src, efi_size length);
    void (efiapi *set_mem)(void *buffer, efi_size size, efi_u8 value);

    // Extended event creation (UEFI 2.0+ only)
    efi_status (efiapi *create_event_ex)(
        efi_u32 type,
        efi_tpl notify_tpl,
        efi_event_notify notify_function,
        void *notify_context,
        efi_guid *event_group,
        efi_event *event);
} efi_boot_services;

// Types of resets
typedef enum {
    efi_reset_cold,
    efi_reset_warm,
    efi_reset_shutdown,
    efi_reset_platform_specific,
} efi_reset_type;

typedef struct {
    efi_table_header hdr;

    // Time services
    efi_status (efiapi *get_time)(efi_time *time, efi_time_cap *cap);
    efi_status (efiapi *set_time)(efi_time *time);
    efi_status (efiapi *get_wakeup_time)(efi_bool *enabled, efi_bool *pending, efi_time *time);
    efi_status (efiapi *set_wakeup_time)(efi_bool enable, efi_time *time);

    // Virtual memory services
    efi_status (efiapi *set_virtual_address_map)(
        efi_size memory_map_size,
        efi_size desc_size,
        efi_size desc_version,
        efi_memory_descriptor *virtual_map);
    efi_status (efiapi *convert_pointer)(efi_size debug_disposition, void **address);

    // Variable services
    efi_status (efiapi *get_variable)(
        efi_ch16 *variable_name,
        efi_guid *vendor_guid,
        efi_u32 *attrib,
        efi_size *data_size,
        void *data);
    efi_status (efiapi *get_next_variable_name)(
        efi_size *variable_name_size,
        efi_ch16 *variable_name,
        efi_guid *vendor_guid);
    efi_status (efiapi *set_variable)(
        efi_ch16 *variable_name,
        efi_guid *vendor_guid,
        efi_u32 attrib,
        efi_size data_size,
        void *data);

    // Misc services
    efi_status (efiapi *get_next_high_monotonic_count)(efi_u32 *high_count);
    void (efiapi *reset_system)(
        efi_reset_type reset_type,
        efi_status reset_status,
        efi_size data_size,
        void *reset_data);

    // Capsule services (UEFI 2.0+)
    void *update_capsule;
    void *query_capsule_capabilities;

    // Variable infromation service (UEFI 2.0+)
    efi_status (efiapi *query_variable_info)(
        efi_u32 attrib,
        efi_u64 *max_storage_size,
        efi_u64 *rem_storage_size,
        efi_u64 *max_size);
} efi_runtime_services;

// EFI configuration table
typedef struct {
    efi_guid    vendor_guid;
    void        *vendor_table;
} efi_configuration_table;

// EFI system table
struct efi_system_table {
    efi_table_header             hdr;
    efi_ch16                     *vendor;
    efi_u32                      revision;
    efi_handle                   con_in_handle;
    efi_simple_text_in_protocol  *con_in;
    efi_handle                   con_out_handle;
    efi_simple_text_out_protocol *con_out;
    efi_handle                   std_err_handle;
    efi_simple_text_out_protocol *std_err;
    efi_runtime_services         *runtime_services;
    efi_boot_services            *boot_services;
    efi_size                     cnt_config_entries;
    efi_configuration_table      *config_entries;
};

#endif
