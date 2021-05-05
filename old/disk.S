.code32
.global int13h_disk_read


int13h_disk_read:

#    ljmp $0x08, $b16
#    ljmp $0x08, $skip

    ljmp $0x08,$b16
.code16
b16:
#	push %esp
#	push %ds
#	push %es
#   mov $0x10,%ax
#	mov %ax,%ds
#	mov %ax,%es
#	mov %ax,%ss
#	mov %ax,%fs
#	mov %ax,%gs
#
	mov %cr0, %eax
	and $0xfe,%al
	mov %eax, %cr0
#
#	ljmp $0x0, $cs0
#cs0:
#    xor %ax,%ax
#    mov %ax,%ss
#	mov %ax,%es

#	sti

#	int $0x13
#
#	cli

	mov %cr0, %eax
	or $1,%al
	mov %eax, %cr0

#	pop %es
#	pop %ds

#ljmp $codeseg, $b32
	ljmp $0x08,$b32
.code32

b32:
#	movw $0x10,%ax
#	movw %ax,%fs
#	movw %ax,%gs
#	movw %ax,%ss
##	movw %ax,%ds
#	movw %ax,%es

#	pop %esp


	ret
