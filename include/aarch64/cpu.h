/*
 * Type definitions for ARM (64-bit)
 */

#ifndef AARCH64_CPU_H
#define AARCH64_CPU_H

/* Fixed width types */
typedef uint8_t efi_u8;
typedef int8_t efi_i8;
typedef uint16_t efi_u16;
typedef int16_t efi_i16;
typedef uint32_t efi_u32;
typedef int32_t efi_i32;
typedef uint64_t efi_u64;
typedef int64_t efi_i64;

/* Size types */
typedef uint64_t efi_size;
typedef int64_t efi_ssize;

/*
 * Max bit for the size type
 */
#define SIZE_MAX_BIT 0x8000000000000000

/* Use default ABI on ARM */
#define efiapi

#endif
