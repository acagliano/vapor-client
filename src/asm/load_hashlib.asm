
public _load_hashlib
public _hashlib_SetMalloc
public _hashlib_Sha1Init
public _hashlib_Sha1Update
public _hashlib_Sha1Final

extern _load_library


_load_hashlib:
	ld de,.data
	push de
	call _load_library
	pop bc
	ret
.data:
	db "HASHLIB",0,2 ;HASHLIB version 2
_hashlib_SetMalloc: ;or whatever
	jp	0
_hashlib_Sha1Init:
    jp  12
_hashlib_Sha1Update:
    jp  15
_hashlib_Sha1Final:
    jp  18
; RSA functions when implemented

	pop hl ; libload executes this code on succesful load
	xor a,a ;return true
	inc a
	ret
