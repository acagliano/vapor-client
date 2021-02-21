extern _ti_Open
extern _ti_Close
extern _ti_OpenVar
extern _ti_Write
extern _ti_GetDataPtr
extern _ti_GetSize
extern _gfx_End
extern __exitsp

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
    ld hl,jump_data
    ld de,jump_data_loc
    ld bc,jump_data_len
    push de
    ldir
    ret


jump_data:
jump_data_loc:=$E30800
    push hl
    scf
    sbc hl,hl
    ld (hl),2
    pop hl
    or a,a
    sbc hl,hl
    ld de,(_asm_prgm_size)
    ld (_asm_prgm_size),hl
    ld hl,_userMem
    call _DelMem
    ld hl, jump_data_loc + program_name - jump_data
    call _Mov9ToOp1
    call _ChkFindSym        ; data in de
    jr c, error_exit
    push de                 ; save de
    ex de, hl
    ld de, (hl)
    ex.sis de, hl
    call _EnoughMem         ;returns HL in DE
    jr c, error_exit
    ex hl,de                ; size back in hl
    ld (_asm_prgm_size),hl
    push hl                 ; we will likely need to preserve size
    ld de,_userMem
    call _InsertMem
    pop bc                  ; get size back
    pop hl                  ; get data ptr back (src)
    inc hl
    inc hl                  ; pass size word
    ld de, _userMem         ; get data ptr (dest)
    ldir
    pop hl
    ld sp, hl
    ld hl, _userMem
    push hl                 ; make ret return to usermem?
    ret
error_exit:
    pop hl
    ld sp, hl
    ret
program_name:
    db $06,"VAPOR",0
jump_data_len:=$-jump_data

