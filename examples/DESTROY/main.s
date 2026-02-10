; =======================================================
; DESTROY THEM MY ROBOT !
; A 64 bytes effect written by hitchhikr/Neural
; =======================================================

; =======================================================
_SUPER		            equ	    $ff20                           ; Switch to supervisor/user mode
GVRAM_PAGE0             equ     $C00000
CRT_CTRL1               equ     $E80028 	                    ; Memory Mode/View Mode Control (R/W)
BG_PAL0_DTA             equ     $E82000
GFX_EN                  equ     $E82600                         ; Enable/Disable various gfx stuff
MFP_GPIP                equ     $E88001                         ; General-purpose I/O register
SYS_PORT1               equ     $E8E001                         ; Computer screen contrast (r/w)

DOS                     macro
                        dc.w	\1
                        endm
                        
; =======================================================
                        xdef    _start
_start:
                        ; switch to supervisor so we can access the hw registers
                        clr.l   -(a7)
                        DOS     _SUPER
                        lea     MFP_GPIP,a5
                        ; 1024x1024
                        ; 16 colors
                        moveq   #%10100,d0
                        move.b  d0,CRT_CTRL1-MFP_GPIP(a5)
                        move.b  d0,GFX_EN+1-MFP_GPIP(a5)
                        lea     GVRAM_PAGE0,a0
                        ; fill all the pages
                        swap    d0
calc:
                        move.l  d0,d4
                        asr.l   #8,d4
                        eor.l   d0,d4
                        asr.l   #7,d4
                        move.w  d4,(a0)+
                        subq.l  #1,d0
                        bne.b   calc
scroll:
                        btst    #6,(a5)
                        bne.b   scroll
                        lea     BG_PAL0_DTA-MFP_GPIP(a5),a0
                        ; change the contrast
                        eor.b   d0,SYS_PORT1-MFP_GPIP(a5)
                        ; and the colors
                        add.w   d0,(a0,d0.w)
                        addq.b  #2,d0
                        bra.b   scroll

                        end
