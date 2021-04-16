
_ChkFindSym    := $02050C
_Mov9ToOp1     := $020320
_ChkInRam      := $021F98
_PushOP1       := $020628
_PopOP1        := $0205C4
_Arc_Unarc     := $021448

_OP1           := $D005F8

extern _frameset
public _load_library

;bool load_library(const uint8_t *name);
_load_library:
	ld	hl,__custom_libloadappvar
	call	_Mov9ToOp1
	ld	a,21
	ld	(_OP1),a
__custom_findlibload:
	call	_ChkFindSym
	jp	c,__custom_notfound
	call	_ChkInRam
	jp	nz,__custom_inarc
	call	_PushOP1
	call	_Arc_Unarc
	call	_PopOP1
	jr	__custom_findlibload
__custom_inarc:
	ex	de,hl
	ld	de,9
	add 	hl,de
	ld	e,(hl)
	add	hl,de
	inc	hl
	inc	hl
	inc	hl
	pop	bc,de
	push	de,bc
	ld	bc,__custom_notfound	;libload returns on fail
	push	bc
    ld  bc, $aa55aa
	jp	(hl)
__custom_notfound:
	xor a,a         ; return 0 if we could not load the lib
    ret
__relocationstart:
; place library jump locations and headers here
;...

__custom_libloadappvar:
	db	" LibLoad",0

