//
// Unicode Collation Protocol
//

#ifndef EFI_UNICODE_COLLATION_H
#define EFI_UNICODE_COLLATION_H

#define EFI_UNICODE_COLLATION_PROTOCOL2_GUID \
    { 0xa4c751fc, 0x23ae, 0x4c3e, 0x92, 0xe9, 0x49, 0x64, 0xcf, 0x63, \
        0xf3, 0x49 }

typedef struct efi_unicode_collation_protocol efi_unicode_collation_protocol;
struct efi_unicode_collation_protocol {
    //
    // Compare two Unicode strings in a case-insensitive manner
    //
    efi_ssize (efiapi *stri_coll)(efi_unicode_collation_protocol *self,
        efi_ch16 *s1, efi_ch16 *s2);

    //
    // Match a unicode string against a pattern
    //
    efi_bool (efiapi *metai_match)(efi_unicode_collation_protocol *self,
        efi_ch16 *string, efi_ch16 *pattern);

    //
    // Convert a Unicode string in-place to lower-case
    //
    void (efiapi *str_lwr)(efi_unicode_collation_protocol *self, efi_ch16 *string);

    //
    // Convert a Unicode string in-place to upper-case
    //
    void (efiapi *str_upr)(efi_unicode_collation_protocol *self, efi_ch16 *string);

    //
    // Convert a FAT filename to a Unicode string
    //
    void (efiapi *fat_to_str)(efi_unicode_collation_protocol *self,
        efi_size fat_size, efi_ch8 *fat, efi_ch16 *string);

    //
    // Convert a Unicode string to a FAT filename
    //
    efi_bool (efiapi *str_to_fat)(efi_unicode_collation_protocol *self,
        efi_ch16 *string, efi_size fat_size, efi_ch8 *fat);

    //
    // Language codes supported by this instance
    //
    efi_ch8 *supported_languages;
};

#endif
