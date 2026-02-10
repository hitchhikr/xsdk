; ------------------------------------------------------------------
; OPMDRV3.X calls
; ------------------------------------------------------------------
_OPMDRV             equ     $F0

_M_INIT             equ     $00
_M_ALLOC            equ     $01
_M_ASSIGN           equ     $02
_M_VGET             equ     $03
_M_VSET             equ     $04
_M_TEMPO            equ     $05
_M_TRK              equ     $06
_M_FREE             equ     $07
_M_PLAY             equ     $08
_M_STAT             equ     $09
_M_STOP             equ     $0A
_M_CONT             equ     $0B
_M_ATOI             equ     $0C
_M_USE              equ     $0F
_M_CHAN             equ     $10
_M_PROG             equ     $11
_M_VOL              equ     $12
_M_PAN              equ     $13
_M_VEL              equ     $14
_M_TRNS             equ     $15
_M_ANTOFF           equ     $18
_M_DIROUT           equ     $19
_M_ENABLE           equ     $1A
_M_NTON             equ     $1B
_M_NTOFF            equ     $1C
_M_SYNC             equ     $20
_M_METER            equ     $21
_M_CLICK            equ     $22
_M_RECINT           equ     $23
_M_RECSET           equ     $24
_M_RECSTA           equ     $25
_M_RECSTP           equ     $26
_M_RECLEN           equ     $27
_M_THRU             equ     $28
_M_SNDSET           equ     $29
_M_PCMSET           equ     $2A
_M_SYSCH            equ     $2B
_M_BEND             equ     $2D
_M_FREEA            equ     $2F
_M_IFCHK            equ     $2C
_M_MOD              equ     $2E
_M_OUT              equ     $30
_M_IN               equ     $31
_M_MEAS             equ     $32
_M_YCOM             equ     $33
_M_START            equ     $34
_M_END              equ     $35
_M_MODSNS           equ     $36
_M_PCMREC           equ     $38
_M_PCMGET           equ     $39
_M_PCMLEN           equ     $3A
_M_PCMBSY           equ     $3B
_M_PCMON            equ     $3C
_M_CTRLRES          equ     $3D
_M_OPMREG           equ     $3E
_M_MDREG            equ     $3F
_M_TNMSET           equ     $40
_M_TNMGET           equ     $41
_M_PNMSET           equ     $42
_M_PNMGET           equ     $43
_M_MSTVOL           equ     $44
_M_OPMLFQ           equ     $46
_M_OPMEXC           equ     $47
_M_PGMFLT           equ     $49
_M_VOLFLT           equ     $4A
_M_PANFLT           equ     $4B
_M_VERSION          equ     $5E

; ------------------------------------------------------------------
OPM                 MACRO
                    moveq   #\1,d1
                    moveq   #-($100-_OPMDRV),d0
                    trap    #TRAP_IOSCALL
                    ENDM
