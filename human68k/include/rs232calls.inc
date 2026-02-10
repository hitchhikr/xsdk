; ------------------------------------------------------------------
; RS-232C calls
; ------------------------------------------------------------------
_RSDRV              equ     $F1

E_SET232C           equ     $30
E_LOF232C           equ     $31
E_INP232C           equ     $32
E_ISNS232C          equ     $33
E_OSNS232C          equ     $34
E_OUT232C           equ     $35
E_BUF232C           equ     $36
E_VER232C           equ     $37
E_PINP232C          equ     $38
E_PSET232C          equ     $39

; ------------------------------------------------------------------
ERS                 MACRO
                    move.l  #\1 or (\2 shl 8),d2
                    moveq   #-($100-_RSDRV),d0
                    trap    #TRAP_IOSCALL
                    ENDM
