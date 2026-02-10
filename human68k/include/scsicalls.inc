; ------------------------------------------------------------------
; SCSI calls
; ------------------------------------------------------------------
_SCSIDRV            equ     $F5

_S_RESET            equ     $00
_S_SELECT           equ     $01
_S_CMDOUT           equ     $03
_S_DATAIN           equ     $04
_S_DATAOUT          equ     $05
_S_STSIN            equ     $06
_S_MSGIN            equ     $07
_S_MSGOUT           equ     $08
_S_PHASE            equ     $09
_S_INQUIRY          equ     $20
_S_READ             equ     $21
_S_WRITE            equ     $22
_S_FORMAT           equ     $23
_S_TESTUNIT         equ     $24
_S_READCAP          equ     $25
_S_READEXT          equ     $26
_S_WRITEEXT         equ     $27
_S_REZEROUNIT       equ     $2B
_S_REQUEST          equ     $2C
_S_SEEK             equ     $2D
_S_STARTSTOP        equ     $2F
_S_REASSIGN         equ     $31
_S_PAMEDIUM         equ     $32

; ------------------------------------------------------------------
SCSI                MACRO
                    moveq   #\1,d1
                    moveq   #-($100-_SCSIDRV),d0
                    trap    #TRAP_IOSCALL
                    ENDM
