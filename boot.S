.code32

start_protect:
	call _start
	hlt

ENTER_PROTECT_MSG: .asciz "Entering Procetcted Mode"
