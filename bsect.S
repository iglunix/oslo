.code16
.equ STAGE2_OFFSET, 0x1000

xor %ax, %ax
mov %ax, %ds
mov %ax, %es
mov %ax, %ss
mov %ax, %fs
mov %ax, %gs

mov $0x7c00, %sp

movw $WELCOME_MSG, %bx
call r_puts

a20:
	inb $0x92,%al
	orb $2,%al
	outb %al, $0x92

etr_pmode:
	cli
	push %ds
	push %es

	lgdt gdt_desc

	mov %cr0, %eax
	or 1, %al
	mov %eax, %cr0

	jmp pmode

pmode:
	mov $0x10, %bx
	mov %bx, %ds
	mov %bx, %es

	and $0xfe, %al
	mov %eax, %cr0

urmode:
	pop %es
	pop %ds
	sti

	movw $UNREAL_MSG, %bx
	call r_puts
	hlt

disk_read:
	movw $READING_MSG, %bx
	call r_puts
	movb $0x0F,%dh
	pushw %dx
	movw $STAGE2_OFFSET, %bx

	movb $0x02,%ah
	movb %dh,%al
	movb $0x00,%ch
	movb $0x02,%cl
	movb $0x00,%dh
	movb $0x80,%dl
	int $0x13

	jc disk_err

	popw %dx
	cmpb %al,%dh
	jne disk_err
	jmp disk_noerr

disk_err:
	movw $DISK_ERR_MSG, %bx
	call r_puts
spin:
	hlt
	jmp spin

# Real mode print
r_puts:
	pusha

r_puts.loop:
	movb (%bx),%al
	cmpb $0,%al
	je r_puts.ret
	movb $0x0E,%ah
	int $0x10
	incw %bx
	jmp r_puts.loop

r_puts.ret:
	movb $0x0E,%ah
	movb $0x0A,%al
	int $0x10
	movb $0x0D,%al
	int $0x10

	popa
	ret

disk_noerr:
	movw $STG2_MSG, %bx
	call r_puts
	cli

	mov %cr0,%eax
	or 1, %al
	mov %eax,%cr0

	ljmp $codeseg,$exec_stg2

.code32

exec_stg2:
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs

	jmp STAGE2_OFFSET

	hlt

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


BOOT_DRIVE: .byte 0x80
WELCOME_MSG: .asciz "Welcome to OSLO: The Operating System LOader"
UNREAL_MSG: .asciz "Currently Unreal"
DISK_ERR_MSG: .asciz "Disk Failure"
READING_MSG: .asciz "Reading Disk"
STG2_MSG: .asciz "Executing Stage 2"

.org 510
.word 0xaa55

