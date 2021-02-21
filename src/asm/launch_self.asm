extern _ti_Open
extern _ti_Close
extern _ti_OpenVar
extern _ti_Write
extern _ti_GetDataPtr
extern _ti_GetSize
extern _gfx_End
extern __exitsp
extern __exit

public _update_program

_InsertMem     := $020514
_DelMem        := $020590
_asm_prgm_size := $D0118C
_userMem       := $D1A881
_EnoughMem     := $02051C
_ChkFindSym    := $02050C
_Mov9ToOp1     := $020320

;void update_program(void);
_update_program:
    ld hl, (__exitsp)
    push hl
    ld a,($E30018)
    cp a,$2D ;OS 16bpp
    call nz,_gfx_End

	ld c,6 ;pprgm type
	ld de,mode_read ;open for reading
    ld hl, program_name ;open the program
    push bc,de,hl
	call _ti_OpenVar
	pop bc,bc,bc
	scf
	sbc hl,hl
	or a,a
	jq z,__exit ;use the C exit label that we haven't destroyed yet
	ld c,a
	push bc
	call _ti_GetDataPtr
	inc hl ;skip header bytes
	inc hl
	ld (new_program_ptr),hl
	call _ti_GetSize
	dec hl ;len-2 to account for header being skipped
	dec hl
	ld (new_program_len),hl
	call _ti_Close
	pop bc
    ld hl,jump_data
    ld de,jump_data_loc
    ld bc,jump_data_len
    push de
    ldir
    ret
mode_read:
	db "r",0

jump_data:
jump_data_loc:=$E30800
	ld sp,(__exitsp) ;doing this before clobbering usermem
	ld a,2
	ld ($FFFFFF),a ;this saves two bytes because we don't need to push/pop hl
    or a,a
    sbc hl,hl
    ld de,(_asm_prgm_size)
    ld (_asm_prgm_size),hl
    ld hl,_userMem
    call _DelMem
	ld hl,0
new_program_len:=$-3
    call _EnoughMem         ;returns HL in DE
    jr c, error_exit
    ex hl,de                ; size back in hl
    ld (_asm_prgm_size),hl
    push hl                 ; we will likely need to preserve size
    ld de,_userMem
    call _InsertMem
    pop bc                  ; get size back
	ld hl,0
new_program_ptr:=$-3
    ld de, _userMem         ; get data ptr (dest)
    ldir
jump_data_cleanup: ;do the same thing the toolchain does
	pop	iy		; iy = flags
	pop	af		; a = original flash wait states
	ex	(sp),hl		; hl = flash wait state control port,
				; save exit code
	ld	(hl),a		; restore flash wait states
	call	00004F0h	; _usb_ResetTimer
	ld	iy,$d00080
	set	1,(iy + $0d)	; use text buffer
	res	3,(iy + $4a)	; use first shadow buffer
	res	5,(iy + $4c)	; use shadow buffer
	res	4,(iy+9)	; onInterrupt,(iy+onFlags)
	set	0,(iy+3)	; graphDraw,(iy+graphFlags)
	call	020808h		; _ClrLCDFull
	call	020828h		; _HomeUp
	call	021A3Ch		; _DrawStatusBar

	pop	hl		; hl = exit code
jump_data_smc_exit:
	jp _userMem
error_exit:
	ld a,$C9
	ld (jump_data_loc + jump_data_smc_exit - jump_data),a ;return instead of jumping to usermem after cleaning up stuff
	jr jump_data_cleanup
program_name:
    db "VAPOR",0
jump_data_len:=$-jump_data

