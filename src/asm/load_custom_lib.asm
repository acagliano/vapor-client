
_ChkFindSym    := $02050C
_Mov9ToOp1     := $020320
_ChkInRam      := $021F98
_PushOP1       := $020628
_PopOP1        := $0205C4
_Arc_Unarc     := $021448

_OP1           := $D005F8

;bool load_library(const char *libname);
_load_library:
	ld	hl,__custom_libloadappvar
	call	_Mov9ToOP1
	ld	a,21
	ld	(OP1),a
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
	ld	de,__custom_relocationstart
	jp	(hl)
__custom_notfound:
	ld a, 0         ; return 0 if we could not load the lib
    ret
__relocationstart:
; place library jump locations and headers here
;...

__custom_libloadappvar:
	db	" LibLoad",0

;; Where do we return from if we succeed?
;; Where do we load in the name of the lib to load?
