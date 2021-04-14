
public _load_bigint
public _bigint_init ;make sure each jump has this kind of declaration here and in the header file

extern _load_library


;bool load_bigint(void);
_load_bigint:
	ld de,.data
	push de
	call _load_library
	pop bc
	ret

.data:
	db "BIGINTCE",0,1 ;BIGINTCE version 1
_bigint_init: ;or whatever
	jp	0
;follow with more of these jumps for each function offset in library

	pop hl ; libload executes this code on succesful load
	xor a,a ;return true
	inc a
	ret
