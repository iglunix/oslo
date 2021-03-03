# based of code from 
# https://blog.ghaiklor.com/2017/11/02/how-to-implement-a-second-stage-boot-loader/

.code16
.equ          STAGE2_OFFSET, 0x1000

movb %dl,BOOT_DRIVE
movw $WELCOME_MSG, %bx
call print_string
call load_boot
call execute_boot

load_boot: 
	pusha
	movw $LOADING_BOOT_MSG, %bx
	call print_string
	movw $STAGE2_OFFSET, %bx
	movb $15,%dh
	movb BOOT_DRIVE,%dl
	call disk_read
	popa
	ret

execute_boot: 
	movw $EXECUTING_BOOT_MSG, %bx
	call print_string
	call STAGE2_OFFSET
	hlt

disk_read: 
	pusha
	pushw %dx

	movb $0x02,%ah
	movb %dh,%al
	movb $0x00,%ch
	movb $0x00,%dh
	movb $0x02,%cl
	int $0x13

	jc disk_read_error

	popw %dx
	cmpb %al,%dh
	jne disk_read_error

	popa
	ret

disk_read_error: 
	movw $DISK_READ_ERROR_MSG, %bx
	call print_string
	hlt

print_string: 
	pusha

print_string.loop: 
	movb (%bx),%al
	cmpb $0,%al
	je print_string.ret
	movb $0x0E,%ah
	int $0x10
	incw %bx
	jmp print_string.loop

print_string.ret: 
	movb $0x0E,%ah
	movb $0x0A,%al
	int $0x10
	movb $0x0D,%al
	int $0x10

	popa
	ret

BOOT_DRIVE: .byte 0x80
WELCOME_MSG: .asciz "Welcome to OSLO: The Operating System LOader"
LOADING_BOOT_MSG: .asciz "Loading Boot"
EXECUTING_BOOT_MSG: .asciz "Executing Boot"
DISK_READ_ERROR_MSG: .asciz "Disk Error"

.org 510
.word 0xAA55
