# based of code from 
# https://blog.ghaiklor.com/2017/11/02/how-to-implement-a-second-stage-boot-loader/

.code16
.equ          STAGE2_OFFSET, 0x1000


cli
xor %ax, %ax
mov %ax, %ds
mov %ax, %ss
mov %ax, %es
mov %ax, %fs
mov %ax, %gs
sti

mov $0x7c00, %sp

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
#	call STAGE2_OFFSET
	jmp enter_protected
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

gdt:
    .quad 0x00000000
gdt_code:
    .word 0xffff
    .word 0x0000
    .byte 0x00
    .byte 0b10011010
    .byte 0b11001111
    .byte 0x00
gdt_data:
    .word 0xffff
    .word 0x0000
    .byte 0x00
    .byte 0b10010010
    .byte 0b11001111
    .byte 0x00
gdt_end:

gdt_desc:
    .word gdt_end-gdt-1
    .quad gdt

.equ codeseg, gdt_code - gdt
.equ dataseg, gdt_data - gdt


enter_protected:

	movw $ENTER_PROTECT_MSG, %bx
	call print_string

    inb $0x92,%al
    orb $2,%al
    outb %al, $0x92

    cli
	push %ds

    lgdt gdt_desc

    mov %cr0,%eax
    or 1, %al
    mov %eax,%cr0

    ljmp $codeseg, $start_protect

.code32

start_protect:

    mov $0x10, %ax

    mov %ax, %ds

# TODO:
# move to unreal mode and do before
# loading from disk and load 64M (or more)
# from disk. This will be decoded by the FAT
# driver later

#	and $0xfe, %al
#	mov %eax, %cr0

#	pop %ds
#	sti
#	mov %ax, %ss
#	mov %ax, %es
#	mov %ax, %fs
#	mov %ax, %gs

	jmp jmp_to_stg2

jmp_to_stg2:
	movb $71, 0xb8002
	jmp STAGE2_OFFSET



BOOT_DRIVE: .byte 0x80
WELCOME_MSG: .asciz "Welcome to OSLO: The Operating System LOader"
LOADING_BOOT_MSG: .asciz "Loading Boot"
EXECUTING_BOOT_MSG: .asciz "Executing Boot"
DISK_READ_ERROR_MSG: .asciz "Disk Error"
ENTER_PROTECT_MSG: .asciz "Entering Procetcted Mode"

.org 510
.word 0xAA55
