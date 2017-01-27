/*
 * cpu.h
 *
 *  Created on: Jan 26, 2017
 *      Author: kmate
 */

#ifndef API_IA32_CPU_H_
#define API_IA32_CPU_H_

/* Fixed width types */
#include <stdint.h>

/* Native width type */
typedef int32_t intn_t;
typedef uint32_t uintn_t;

/* Size type */
typedef uint32_t size_t;

/* Native max bit */
#define MAX_BIT (uintn_t) 1 << 32

/* EFI function calling convention */
#define efi_func

#endif /* API_IA32_CPU_H_ */
