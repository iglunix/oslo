.code32

start_protect:

	movb $71, 0xb8004

	call _start
	hlt

ENTER_PROTECT_MSG: .asciz "Entering Procetcted Mode"
