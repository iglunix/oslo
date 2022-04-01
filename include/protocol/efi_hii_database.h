//
// Human Interface Infrastructure Database Protocol
//

#ifndef EFI_HII_DATABASE_H
#define EFI_HII_DATABASE_H

#define EFI_HII_DATABASE_PROTOCOL_GUID \
    { 0xef9fc172, 0xa1b2, 0x4693, 0xb3, 0x27, 0x6d, 0x32, 0xfc, 0x41, \
        0x60, 0x42 }

typedef void *efi_hii_handle;

//
// HII Package
//

typedef struct {
    efi_u32 length : 24;
    efi_u32 type   : 8;
    efi_u8  data[1];
} efi_hii_package_header;

#define EFI_HII_PACKAGE_TYPE_ALL             0x00
#define EFI_HII_PACKAGE_TYPE_GUID            0x01
#define EFI_HII_PACKAGE_FORM                 0x02
#define EFI_HII_PACKAGE_KEYBOARD_LAYOUT      0x03
#define EFI_HII_PACKAGE_STRINGS              0x04
#define EFI_HII_PACKAGE_FONTS                0x05
#define EFI_HII_PACKAGE_IMAGES               0x06
#define EFI_HII_PACKAGE_SIMPLE_FONTS         0x07
#define EFI_HII_PACKAGE_DEVICE_PATH          0x08
#define EFI_HII_PACKAGE_END                  0xDF
#define EFI_HII_PACKAGE_TYPE_SYSTEM_BEGIN    0xE0
#define EFI_HII_PACKAGE_TYPE_SYSTEM_END      0xFF

//
// HII Package List
//
typedef struct {
    efi_guid package_list_guid;
    efi_u32  package_length;
} efi_hii_package_list_header;

//
// HII Database Notification
//

#define EFI_HII_DATABASE_NOTIFY_NEW_PACK    0x00000001
#define EFI_HII_DATABASE_NOTIFY_REMOVE_PACK 0x00000002
#define EFI_HII_DATABASE_NOTIFY_EXPORT_PACK 0x00000004
#define EFI_HII_DATABASE_NOTIFY_ADD_PACK    0x00000008

typedef efi_status (efiapi *efi_hii_database_notify)(efi_u8 package_type,
    efi_guid *package_guid, efi_hii_package_header *package,
    efi_hii_handle handle, efi_size notify_type);

typedef struct efi_hii_database_protocol efi_hii_database_protocol;
struct efi_hii_database_protocol {
    efi_status (efiapi *new_package_list)(efi_hii_database_protocol *self,
        efi_hii_package_list_header *package_list, efi_handle driver_handle,
        efi_hii_handle *handle);
    efi_status (efiapi *remove_package_list)(efi_hii_database_protocol *self,
        efi_hii_handle handle);
    efi_status (efiapi *update_package_list)(efi_hii_database_protocol *self,
        efi_hii_handle handle,
        efi_hii_package_list_header *package_list);
    efi_status (efiapi *list_package_lists)(efi_hii_database_protocol *self,
        efi_u8 package_type, efi_guid *package_guid,
        efi_size *buffer_size, efi_hii_handle *buffer);
    efi_status (efiapi *export_package_lists)(efi_hii_database_protocol *self,
        efi_hii_handle handle, efi_size *buffer_size,
        efi_hii_package_list_header *buffer);
    efi_status (efiapi *register_package_notify)(efi_hii_database_protocol *self,
        efi_u8 package_type, efi_guid *package_guid,
        efi_hii_database_notify notify_function,
        efi_size notify_type, efi_handle *notify_handle);
    efi_status (efiapi *unregister_package_notify)(efi_hii_database_protocol *self,
        efi_handle notify_handle);
    efi_status (efiapi *find_keyboard_layouts)(efi_hii_database_protocol *self,
        efi_u16 *key_guid_buffer_length,
        efi_guid *key_guid_buffer);
    efi_status (efiapi *get_keyboard_layout)(efi_hii_database_protocol *self,
        efi_guid *key_guid, efi_u16 *keyboard_layout_length,
        void *keyboard_layout);
    efi_status (efiapi *set_keyboard_layout)(efi_hii_database_protocol *self,
        efi_guid *key_guid);
    efi_status (efiapi *get_package_list_handle)(efi_hii_database_protocol *self,
        efi_hii_handle package_list_handle, efi_handle *driver_handle);
};

#endif
