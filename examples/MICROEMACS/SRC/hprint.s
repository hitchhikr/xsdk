;-------------------------------------------------
;
;   <<<Super fast print>>>
;
;   H_PRINT( char *s, char *w)
;       s: The string to put in Cyan
;       w: String to put in white
;
;   Written by IRUKA 1989. All right reserved.
;
;-------------------------------------------------

            section .text,code

            public  H_ERA       ; Erase line (Current Line)
            public  H_PRINT     ; Super fast print (*Cyan,*White,EOL)
            public  H_INIT      ; Preparation (reading ANK font, etc.)
            public  LDIRL       ; Block transfer (long word)
            public  kbhit
            public  iskanji
            public  iskanji2
;
;   _LDIRL is transferred in bytes
;   Always call _H_INIT before executing _H_ERA or _H_PRINT.
;   _H_INIT only needs to be called once.
;   After using _H_PRINT, there is no need to perform a CLOSE process.
;
_EXIT       equ     $ff00
_SUPER      equ     $ff20
_INPOUT     equ     $ff06

X_MAX       equ     $970
Y_MAX       equ     $972
X_POS       equ     $974
Y_POS       equ     $976

DOS         macro
            dc.w    \1
            endm

push        macro
            move.l  \1,-(sp)
            endm

pop         macro
            move.l  (sp)+,\1
            endm

put8        macro
            move.b  (a4)+,\1*128-1(a5)
            endm

put16       macro
            move.b  (a4)+,\1*128-2(a5)
            move.b  (a4)+,\1*128-1(a5)
            endm

put16w      macro
            move.w  (a4)+,\1*128-2(a5)
            endm

ldz         macro
            dc.w    $3038+\2*512    ;0011 000 000 111 000
            dc.w    \1
            endm

cmpz        macro
            dc.w    $b078+\2*512    ;1011 000 001 111 000
            dc.w    \1
            endm

incz        macro
            dc.w    $5278
            dc.w    \1
            endm

incz2       macro
            dc.w    $5478
            dc.w    \1
            endm

H_INIT:
            movem.l d0-d2/a0-a1,-(a7)
            moveq   #0,d0
            push    d0
            DOS     _SUPER
            pop     d0
            bsr     m11
            bsr     sheffer
            bsr     eolchar
            bsr     eeol_line63
            movem.l (a7)+,d0-d2/a0-a1
            rts

eeol_line63:
            move.l  #$e00000,a0
            add.l   #128*16*63,a0
            bsr     eeol_1plane
            add.l   #$020000,a0

eeol_1plane:
            move.l  a0,-(sp)
            move    #128,d1
            move.l  #0,d0
eeol_l63:
            move.l  d0,(a0)+
            move.l  d0,(a0)+
            move.l  d0,(a0)+
            move.l  d0,(a0)+
            subq    #1,d1
            bne     eeol_l63
            move.l  (sp)+,a0
            rts

kbhit:
            move.w  #$FE,-(sp)
            DOS     _INPOUT
            addq.l  #2,sp
            rts

iskanji:
            move.l  4(sp),d1
            moveq   #0,d0
            cmp.b   #$81,d1
            bcs.s   .ok
            cmp.b   #$A0,d1
            bcs.s   .error
            cmp.b   #$E0,d1
            bcs.s   .ok
            cmp.b   #$F0,d1
            bcc.s   .ok
.error:
            moveq   #1,d0
.ok:
            rts

iskanji2:
            move.l  4(sp),d1
            moveq   #0,d0
            cmp.b   #$40,d1
            bcs.s   .ok
            cmp.b   #$7F,d1
            bcs.s   .error
            cmp.b   #$80,d1
            bcs.s   .ok
            cmp.b   #$FD,d1
            bcc.s   .ok
.error:
            moveq   #1,d0
.ok:
            rts

;-------------------------------------------------
;
;   Delete one line()
;
;-------------------------------------------------
H_ERA:
            movem.l d1-d3,-(a7)
            ldz     $976,1      ; y
            and     #$003f,d1
            asl     #2,d1
            add     #$fc00,d1
            moveq   #4,d2
            moveq   #3,d3
            move.l  #$df,d0
            trap    #15         ; Raster Copy
            movem.l (a7)+,d1-d3
            rts

;-------------------------------------------------
;
;   <<<Super fast print>>>
;
;   H_PRINT( char *s, char *w,int f)
;       s: The string to put in Cyan
;       w: String to put in white
;
;   Register Allocation List
;       a0:A pointer to the display string (reading)
;       a2:e8002a (simultaneous write port)
;       a3:Pointer to the half-width font buffer (fixed)
;
;       a4:Font buffer pointer to put
;       a5:Pointer to the VRAM address to put to
;
;       d0:Character to put := (a0)++
;       d1:For ShiftJIS fonts
;       d2-d3:scratch!
;
;
;   str: 7f=Vertical bar
;        1f=Line break mark
;-------------------------------------------------
H_PRINT:
_HP_40:
            movem.l d1-d3/a0/a2-a5,-(a7)

;*-------------------------------------------------
;*  First, we will erase the current line, so we will assume that line 63 is cleared.
;*

            move.w  #$fcf8,d1       ; Line 63-->62

            tst.l   11*4(a7)        ; flg
            bne     nonflash_0
            ldz     $976,1 * y
            and.w   #$3f,d1
            asl.w   #2,d1
            add.w   #$fc00,d1
nonflash_0:
            moveq   #4,d2
            moveq   #3,d3
; d1=Src_raster/4 *256 +Dst_raster/4
; d2=count/4
; d3=direction(MSB)+plane enable
            move.l  #$df,d0
            trap    #15             ; Raster Copy

;---------------- High Speed Putc ----------------
; Entry is Here !!!! 
_HSETUP:    ; set vram address --> a5
            ; set font address --> a3
            movea.l #$e8002a,a2
            move.w  (a2),e8002a_sav
            move.w  #$113,(a2)      ; CYAN!
            moveq   #0,d0           ; d0= 00000000
            move    #$e0,d1
            swap    d1              ; d1= 00e0xxxx
            ldz     $974,1          ; x:d1

            move.l  #62*128*16,d0   ; y: 62 lines
            tst.l   11*4(a7)        ; flg
            bne     nonflash_1
            ldz     $976,0          ; y:d0
            ror.w   #6,d0           ;   y *= (128 * 16)
            asl.l   #1,d0           ;   
nonflash_1:
            add.l   d1,d0           ;   y += x + e00000
            movea.l d0,a5           ;  a5 = y;
            lea     _font,a3            ; font  ==   a3

            move.l  9*4(a7),a0      ; First string
            bsr     _HP_60

            move    #$0133,(a2)     ; WHITE
            move.l  10*4(a7),a0     ; Second string
            bsr     _HP_60

            tst.l   11*4(a7)        ; flg
            beq     flashed_1

            ldz     $976,1          ; y
            and.w   #$3f,d1
            asl.w   #2,d1
            add.w   #$f800,d1
            moveq   #4,d2
            moveq   #3,d3
; d1=Src_raster/4 *256 +Dst_raster/4
; d2=count/4
; d3=direction(MSB)+plane enable
            move.l  #$df,d0
            trap    #15             ; Raster Copy
flashed_1:

            move    e8002a_sav,(a2) ; e8002a return
            movem.l (a7)+,d1-d3/a0/a2-a5
            rts

;-------------------------------------------------
_HSCLR8:
            addq.l  #1,a5
            bra     _HP_59

;           ASCII | kana
_HP_ea:
            asl.w   #4,d0
            lea     0(a3,d0),a4     ; font
            move.b  (a4)+,(a5)+
            put8    1
            put8    2
            put8    3
            put8    4
            put8    5
            put8    6
            put8    7
            put8    8
            put8    9
            put8    10
            put8    11
            put8    12
            put8    13
            put8    14
            put8    15
_HP_59:
            incz    $974
_HP_60:
            moveq   #0,d0
            move.b  (a0)+,d0
            beq     _HP__14         ; EOL
            cmp.b   #$20,d0
            beq     _HSCLR8         ; SPACE
            cmp.b   #$FE,d0
            beq     _HP_TABF        ; TAB(1st.)
            cmp.b   #$FF,d0
            beq     _HP_TABS        ; TAB(2nd.Å`)
            cmp.b   #$81,d0
            bcs     _HP_ea          ; ASCii
            cmp.b   #$a0,d0
            bcs     _HP_b0          ; kanji
            cmp.b   #$e0,d0
            bcs     _HP_ea          ; kana
            cmp.b   #$f0,d0
            bcs     _HP_b0          ; kanji
            cmp.b   #$f6,d0
            bcs     _HP_b0          ; f0-f5
            bra     _HP_ea          ; ASCii
;
;   <EOL>
_HP__14:
            rts

_HP_TABF:
            lea     _TFFONT(pc),a4
            bra     __HP_TABS
_HP_TABS:
            lea     _TSFONT(pc),a4
__HP_TABS:
            move.w  (a2),-(sp)
            move.w  #$113,(a2)
            move.b  (a4)+,(a5)+
            put8    1
            put8    2
            put8    3
            put8    4
            put8    5
            put8    6
            put8    7
            put8    8
            put8    9
            put8    10
            put8    11
            put8    12
            put8    13
            put8    14
            put8    15
            move.w  (sp)+,(a2)
            bra     _HP_59

;
;   KANJI
;
_HP_b0:
            move.b  d0,-(sp)
            move.w  (sp)+,d0
            move.b  (a0)+,d0
; Full-width Kanji
_HIJIS:
            move    d0,d1
            bsr     JISROM
            move.l  d0,a4
            move.l  a5,d1
            btst    #0,d1
            beq     _HIJISW
; Put Font (a4) ---> (a5)
            move.b  (a4)+,(a5)+
            move.b  (a4)+,(a5)+
            put16   1
            put16   2
            put16   3
            put16   4
            put16   5
            put16   6
            put16   7
            put16   8
            put16   9
            put16   10
            put16   11
            put16   12
            put16   13
            put16   14
            put16   15
            incz2   $974
            bra     _HP_60

; Put Font (a4) ---> (a5)
_HIJISW:
            move.w  (a4)+,(a5)+
            put16w  1
            put16w  2
            put16w  3
            put16w  4
            put16w  5
            put16w  6
            put16w  7
            put16w  8
            put16w  9
            put16w  10
            put16w  11
            put16w  12
            put16w  13
            put16w  14
            put16w  15
            incz2   $974
            bra     _HP_60

_H_CRR:
            rts

JISROM:
            cmp     #$eb00,d1       ; eb00 ~
            bcc.s   _ORGROM

; Check the lower byte (40~7e, 80~fc)
            move.l  #$f00000/32,d0  ; Kanji ROM base address
            move.b  d1,d0
            sub.b   #$40,d0
            bcs.s   _UFO
            cmp.b   #$7f,d1
            bcs.s   _JR1
            beq.s   _UFO
            subq.b  #1,d0
            cmp.b   #$fd,d1
            bcc.s   _UFO
_JR1:
            ;Alternate Multiplication   d0 += (d1-$81)*$bc
            moveq   #0,d2
            move    d1,d2
            and     #$ff00,d2
            sub     #$8100,d2
            lsr.w   #1,d2           ; x %11000000 - %100
            add.l   d2,d0
            lsr.w   #1,d2
            add.l   d2,d0
            lsr.w   #4,d2
            sub.l   d2,d0
;--------------------------------------
            lsl.l   #5,d0           ; x 32bytes
            cmp     #$8500,d1
            bcs.s   _JRET
            cmp     #$889f,d1
            bcs.s   _ORGROM         ; NEC external characters

            sub.l   #$5240,d0
            cmp     #$a000,d1       ; Non-Kanji Gap
            bcs.s   _JRET

            sub.l   #$5e000,d0
            cmp     #$e000,d1       ; Second Level Gap
            bcc.s   _JRET

_ORGROM:
            moveq   #8,d2           ; Size
            moveq   #$16,d0
            trap    #15             ; Get Kanji ROM address -->d0.l
_JRET:
            rts

;
;   Tofu characters
;
_UFO:
            lea     _UFONT(pc),a4
            move.l  a4,d0
            rts

_UFONT:
            dc.w    $5555,$aaaa,$5555,$aaaa,$5555,$aaaa,$5555,$aaaa
            dc.w    $5555,$aaaa,$5555,$aaaa,$5555,$aaaa,$5555,$aaaa

_TFFONT:
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00111000
            dc.b    %00111000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000

_TSFONT:
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %10001000
            dc.b    %10001000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000
            dc.b    %00000000

m11:
            moveq   #$20,d1
            moveq   #$7f,d2
            bsr     m111
            move.l  #$a0,d1
            move.l  #$e0,d2
m111:
            push    d1
            push    d2
            bsr     getrom_
            pop     d2
            pop     d1

            addq.w  #1,d1
            cmp     d2,d1
            bne     m111
            rts

getrom_:
            moveq   #8,d2
            push    d1
            moveq   #$16,d0
            trap    #15
            pop     d1
            movea.l d0,a0
            and     #$ff,d1
            asl     #4,d1
            add.l   #_font,d1
            movea.l d1,a1
            move.l  (a0)+,(a1)+
            move.l  (a0)+,(a1)+
            move.l  (a0)+,(a1)+
            move.l  (a0)+,(a1)+
            rts

sheffer:
            move.l  #$10101010,d0
            lea     $7f*16+_font,a0
            move.l  d0,(a0)+
            move.l  d0,(a0)+
            move.l  d0,(a0)+
            move.l  d0,(a0)+
            rts

eolchar:
            move.l  #$11f,d1
            bra     getrom_

LDIRL:
            movem.l a0/a1,-(a7)
            move.l  4+(4*2)(a7),a0
            move.l  8+(4*2)(a7),a1
            move.l  12+(4*2)(a7),d0
            bra.b   _go
_ldir1:
            move.b  (a0)+,(a1)+
_go:
            dbra    d0,_ldir1
            movem.l (a7)+,a0/a1
            rts

            section .bss,bss

e8002a_sav: ds.w    1
_font:      ds.b    4096
