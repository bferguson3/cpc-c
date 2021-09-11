; CPC MACHINE CODE MONITOR
;  Original code (c)Interface Publications
;  Clive Gifford, Scott Vincent 1986
; Reworked 2021 @RetroDevDiscord

KL_U_ROM_ENABLE equ $b900
KL_L_ROM_ENABLE equ $b906 
KL_U_ROM_DISABLE equ $b903
KL_L_ROM_DISABLE equ $b909 
KM_TEST_KEY equ $bb1e
KM_RESET equ $bb03
SCR_HW_ROLL equ $bc4d 
TXT_OUTPUT equ $bb5a 

    org $4100 ;;40000

; Init
    ld l,(ix+0) 
    ld h,(ix+1) 
    ld e,(hl)
    inc hl 
    ld d,(hl) 
    ld l,(ix+2)
    ld h,(ix+3)
    ld a,(hl) 
    or a 
    jr z,SETRAM
    call KL_U_ROM_ENABLE
    call KL_L_ROM_ENABLE ; KL L ROM ENABLE
    jr _keeprom
SETRAM:
    call KL_U_ROM_DISABLE ; KL U ROM DISABLE
    call KL_L_ROM_DISABLE ; KL L ROM DISABLE
_keeprom:
    ld b,$19
_initscr:
    push bc 
    ld a,$19 
    call SCROLLSCR 
    pop bc 
    djnz _initscr
;
; Input loop
GETKEY:
    ld a,$39 ; KEY "3"
    call KM_TEST_KEY ; KM TEST KEY
    jr z,_keyx
    ld a,$19
    call SCROLLSCR 
    jr GETKEY
_keyx:
    ld a,$3f ; KEY "X"
    call KM_TEST_KEY 
    jr z,_keye
    ld a,1
    call SCROLLSCR
    jr GETKEY
_keye: 
    ld a,$3a ; KEY "E"
    call KM_TEST_KEY
    jr z,_keyd
    ld a,$19
    call SCROLLSLO
    jr GETKEY 
_keyd:
    ld a,$3d ; KEY "D"
    call KM_TEST_KEY 
    jr z,_keyret
    ld a,1
    call SCROLLSLO 
    jr GETKEY 
_keyret:
    ld a,$12 ; 2+2 = KEY "RETURN"
    call KM_TEST_KEY
    jr nz,$0f
    ld a,$43 ; 8+3 = KEY "Q"
    call KM_TEST_KEY
    jr z,GETKEY
    ld l,(ix+2)
    ld h,(ix+3)
    ld (hl),$63
; +0f
    ld l,(ix+0)
    ld h,(ix+1)
    ld (hl),e 
    inc hl 
    ld (hl),d 
    call KM_RESET ; clear buffer
    ret 

; Scrolling routines
; 
SCROLLSLO: ; Wait 5000h before proceeding with scroll
    push af 
    ld bc,$5000
_waitlp:
    dec bc 
    ld a,b 
    or c 
    jr nz,_waitlp
    pop af 
SCROLLSCR:
; A = 1 or 25 (1 = down)
    push af 
    push de ; contains ix+0/ix+1 (start address)
    ld b,a 
    srl b 
    srl b 
    srl b 
    srl b 
    xor a 
    call SCR_HW_ROLL
    ld a,$1f 
    call TXT_OUTPUT 
    ld a,5 ; 1F05 - query/wait
    call TXT_OUTPUT 
    pop de 
    pop af 
    push af 
    call TXT_OUTPUT
    cp 1
    jr z,_decde
    inc de 
    ld hl,24
    add hl,de 
    jr _outtxt ; 9d04
_decde:    
    dec de 
    ld h,d 
    ld l,e 
_outtxt:
    ld b,h 
    call PRINTHEX 
    ld b,l 
    call PRINTHEX 
    ld a,$1f
    call TXT_OUTPUT
    ld a,$0d ; 1f0d - CR
    call TXT_OUTPUT
    pop af 
    push af ; reset A
    call TXT_OUTPUT
    ld b,(hl) 
    call PRINTHEX 
    pop bc 
    ld a,(hl) 
    cp $21 ; ascii >33
    ret m 
    cp $7f ; and <127
    ret p 
    ld a,$1f 
    call TXT_OUTPUT
    ld a,$13 ; 1f13 - pause
    call TXT_OUTPUT
    ld a,b 
    call TXT_OUTPUT
    ld a,(hl) 
    call TXT_OUTPUT
    ret 

; Hex to ascii routine
;
PRINTHEX: 
; Input: [B] = val to print 
    ld a,b 
    and $0f 
    ld c,a 
    srl b 
    srl b 
    srl b 
    srl b 
    ld a, b 
    ld b, 2
_prhexloop:
    cp 10
    jp p,_atof
    add a,$30 ; '0' = 0x30
    jr _num
_atof:
    add a,$37 ; 'A' = 0x47
_num:
    call TXT_OUTPUT
    ld a,c 
    djnz _prhexloop
    ret 