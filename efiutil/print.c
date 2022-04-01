/*
 * Formatted printing support
 *
 * There is no support printing standard C types, this is because in UEFI,
 *  fixed width types are preferred.
 *
 * The following format specifiers are supported:
 *  - %p:  pointers
 *  - %c:  efi_ch16
 *  - %g:  efi_guid
 *  - %s:  NUL-terminated efi_ch16 string
 *  - %d:  32-bit signed,   decimal
 *  - %ld: 64-bit signed,   decimal
 *  - %u:  32-bit unsinged, decimal
 *  - %lu: 64-bit unsigned, decimal
 *  - %x:  32-bit unsinged, hex
 *  - %lx: 64-bit unsigned, hex
 */

#include <efi.h>
#include <efiutil.h>

//
// Print a string to the default console
//
#define efi_puts(str) st->con_out->output_string(st->con_out, str)

//
// EFI can't do putchar directly, thus we need to emulate it
//
static void efi_putchar(efi_ch16 ch)
{
    efi_ch16 buffer[2];

    buffer[0] = ch;
    buffer[1] = 0;
    efi_puts(buffer);
}

//
// Character set for integer to string conversion
//
static efi_ch16 *digits = L"0123456789abcdef";

//
// Type specific integer to string conversion functions
//
#define GEN_PRINT(U, S)                                                        \
                                                                               \
static void efi_print_##U(U num, efi_size base, efi_size pad)                  \
{                                                                              \
    efi_ch16 buf[20];                                                          \
    efi_ch16 *p = buf + ARRAY_SIZE(buf);                                       \
                                                                               \
    *--p = 0;                                                                  \
    do {                                                                       \
        *--p = digits[num % base];                                             \
    } while (p >= buf && (num /= base));                                       \
                                                                               \
    efi_size numlen = ARRAY_SIZE(buf) - (p - buf) - 1;                         \
    if (numlen < pad) {                                                        \
        pad -= numlen;                                                         \
        while (pad--)                                                          \
            efi_putchar(L'0');                                                 \
    }                                                                          \
    for (; *p; ++p)                                                            \
        efi_putchar(*p);                                                       \
}                                                                              \
                                                                               \
static void efi_print_##S(S num, efi_size base)                                \
{                                                                              \
    if (num < 0) {                                                             \
        efi_putchar(L'-');                                                     \
        num *= -1;                                                             \
    }                                                                          \
    efi_print_##U(num, base, 0);                                               \
}

//
// Fixed width types
//
GEN_PRINT(efi_u32, efi_i32)
GEN_PRINT(efi_u64, efi_i64)

//
// Size type (also used for pointers)
// NOTE: we assume efi_size can fit any pointer, this is true for all
//   platforms supported by libefi
//
GEN_PRINT(efi_size, efi_ssize)

//
// Read the number of padding characters from the format string
//
static efi_size read_padcnt(efi_ch16 **fmt)
{
    efi_size cnt = 0;
    efi_ch16 *s = *fmt;

    for (;; ++s)
        switch (*s) {
        case L'0' ... L'9':
            cnt = cnt * 10 + *s - L'0';
            break;
        default:
            *fmt = s;
            return cnt;
        }
}

void efi_print(efi_ch16 *fmt, ...)
{
    va_list va;
    efi_size padcnt;
    efi_guid *guid;

    va_start(va, fmt);
    for (; *fmt; ++fmt)
        switch (*fmt) {
        case L'%':
            switch (*++fmt) {
            case L'c':
                efi_putchar(va_arg(va, int));
                break;
            case L's':
                efi_puts(va_arg(va, efi_ch16 *));
                break;
            case L'p':
                efi_print_efi_size(va_arg(va, efi_size), 16, 0);
                break;
            case L'd': // 32-bit
                efi_print_efi_i32(va_arg(va, efi_i32), 10);
                break;
            case L'u':
                efi_print_efi_u32(va_arg(va, efi_u32), 10, 0);
                break;
            case L'x':
                efi_print_efi_u32(va_arg(va, efi_u32), 16, 0);
                break;
            case L'0': // 0-padded
                ++fmt;
                padcnt = read_padcnt(&fmt);
                switch (*fmt) {
                case L'd': // 32-bit
                    efi_print_efi_i32(va_arg(va, efi_i32), 10);
                    break;
                case L'u':
                    efi_print_efi_u32(va_arg(va, efi_u32), 10, padcnt);
                    break;
                case L'x':
                    efi_print_efi_u32(va_arg(va, efi_u32), 16, padcnt);
                    break;
                case L'l': // 64-bit
                    switch (*++fmt) {
                    case L'd':
                        efi_print_efi_i64(va_arg(va, efi_i64), 10);
                        break;
                    case L'u':
                        efi_print_efi_u64(va_arg(va, efi_u64), 10, padcnt);
                        break;
                    case L'x':
                        efi_print_efi_u64(va_arg(va, efi_u64), 16, padcnt);
                        break;
                    default:
                        goto unknown;
                    }
                    break;
                default:
                    goto unknown;
                }
                break;
            case L'l': // 64-bit
                switch (*++fmt) {
                case L'd':
                    efi_print_efi_i64(va_arg(va, efi_i64), 10);
                    break;
                case L'u':
                    efi_print_efi_u64(va_arg(va, efi_u64), 10, 0);
                    break;
                case L'x':
                    efi_print_efi_u64(va_arg(va, efi_u64), 16, 0);
                    break;
                default:
                    goto unknown;
                }
                break;
            case L'g':
                guid = va_arg(va, efi_guid *);
                efi_print(L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                    guid->data1, guid->data2, guid->data3,
                    guid->data4[0], guid->data4[1], guid->data4[2],
                    guid->data4[3], guid->data4[4], guid->data4[5],
                    guid->data4[6], guid->data4[7]);
                break;
            case L'%':
                efi_putchar(L'%');
                break;
            default:
            unknown:
                efi_putchar(L'?');
                break;
            }
            break;
        case '\r': /* Ignore CR */
            break;
        case L'\n': /* Write CRLF on LF */
            efi_puts(L"\r\n");
            break;
        default:
            efi_putchar(*fmt);
            break;
        }
    va_end(va);
}
