/*
 * efistatus.h
 *
 *  Created on: Jan 26, 2017
 *      Author: kmate
 */

#ifndef API_EFI_STATUS_H_
#define API_EFI_STATUS_H_

/* EFI status type */
typedef uintn_t efi_status_t;

/* Macro for defining error codes */
#define EFIERR(x) (x | 0x8000000000000000)

/* Macro for checking if a status code is an error */
#define EFI_ERROR(x) (((intn_t) x) < 0)

/* Success status code */
#define EFI_SUCCESS 0

/* Warning status codes */
#define EFI_WARN_UNKOWN_GLYPH     1
#define EFI_WARN_DELETE_FAILURE   2
#define EFI_WARN_WRITE_FAILURE    3
#define EFI_WARN_BUFFER_TOO_SMALL 4

/* Error status codes */
#define EFI_LOAD_ERROR           EFIERR(1)
#define EFI_INVALID_PARAMETER    EFIERR(2)
#define EFI_UNSUPPORTED          EFIERR(3)
#define EFI_BAD_BUFFER_SIZE      EFIERR(4)
#define EFI_BUFFER_TOO_SMALL     EFIERR(5)
#define EFI_NOT_READY            EFIERR(6)
#define EFI_DEVICE_ERROR         EFIERR(7)
#define EFI_WRITE_PROTECTED      EFIERR(8)
#define EFI_OUT_OF_RESOURCES     EFIERR(9)
#define EFI_VOLUME_CORRUPTED     EFIERR(10)
#define EFI_VOLUME_FULL          EFIERR(11)
#define EFI_NO_MEDIA             EFIERR(12)
#define EFI_MEDIA_CHANGED        EFIERR(13)
#define EFI_NOT_FOUND            EFIERR(14)
#define EFI_ACCESS_DENIED        EFIERR(15)
#define EFI_NO_RESPONSE          EFIERR(16)
#define EFI_NO_MAPPING           EFIERR(17)
#define EFI_TIMEOUT              EFIERR(18)
#define EFI_NOT_STARTED          EFIERR(19)
#define EFI_ALREADY_STARTED      EFIERR(20)
#define EFI_ABORTED              EFIERR(21)
#define EFI_ICMP_ERROR           EFIERR(22)
#define EFI_TFTP_ERROR           EFIERR(23)
#define EFI_PROTOCOL_ERROR       EFIERR(24)
#define EFI_INCOMPATIBLE_VERSION EFIERR(25)
#define EFI_SECURITY_VIOLATION   EFIERR(26)
#define EFI_CRC_ERROR            EFIERR(27)
#define EFI_END_OF_MEDIA         EFIERR(28)
#define EFI_END_OF_FILE          EFIERR(31)
#define EFI_INVALID_LANGUAGE     EFIERR(32)
#define EFI_COMPROMISED_DATA     EFIERR(33)

#endif /* API_EFI_STATUS_H_ */
