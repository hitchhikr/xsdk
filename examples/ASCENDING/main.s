; =======================================================
; Ascending
; A 128 bytes effect written by hitchhikr/Neural
; =======================================================

; =======================================================
_CRTMOD                 equ     $10                             ; Set Screen Attributes
_VDISPST                equ     $6c                             ; De/Install a vblank interrupt handler

_SUPER		            equ	    $ff20                           ; Switch to supervisor/user mode

GVRAM_PAGE0             equ     $C00000
GVRAM_PAGE1             equ     $C80000
GVRAM_PAGE2             equ     $D00000
GVRAM_PAGE3             equ     $D80000

CRT_GFX_X0              equ     $E80018 	                    ; X0
CRT_GFX_Y0              equ     $E8001A 	                    ; Y0
CRT_GFX_X1              equ     $E8001C 	                    ; X1
CRT_GFX_Y1              equ     $E8001E 	                    ; Y1
CRT_GFX_X2              equ     $E80020 	                    ; X2
CRT_GFX_Y2              equ     $E80022 	                    ; Y2
CRT_GFX_X3              equ     $E80024 	                    ; X3
CRT_GFX_Y3              equ     $E80026 	                    ; Y3

CRT_CTRL1               equ     $E80028 	                    ; Memory Mode/View Mode Control (R/W)

BG_PAL0_DTA             equ     $E82000
GFX_COLMODE             equ     $E82400
GFX_EN                  equ     $E82600                         ; Enable/Disable various gfx stuff

DOS                     macro
                        dc.w	\1
                        endm

; =======================================================
                        xdef    _start
_start:
                        ; switch to supervisor so we can access
                        ; the hw registers
                        clr.l   -(a7)
                        DOS     _SUPER

                        ; 256x240 16 colors 15 Khz ~60hz
                        ; (also 4 pages counter)
                        moveq   #4-1,d1
                        moveq   #_CRTMOD,d0
                        trap    #15

                        lea     GFX_EN,a5

                        ; d6 = 0 at startup
                        move.w  d6,CRT_CTRL1-GFX_EN(a5)
                        move.w  d6,GFX_COLMODE-GFX_EN(a5)
                        ; 4 pages/16 colors + 1 xtra bit
                        ; (which doesn't matter in that case)
                        ; (also used for the gradient)
                        moveq   #%0000000000011111,d0
                        move.w  d0,(a5)

                        lea     GVRAM_PAGE0,a0
                        lea     BG_PAL0_DTA+2+(4*2)-GFX_EN(a5),a1
                        ; color index selection and chessboard sizes
                        moveq   #1,d3
                        ; 4 chessboards
.set_pages:
                        moveq   #16/2,d4
                        lsl.w   d3,d4
                        bra.b   .go_squares
.set_squares:
                        moveq   #32*2,d5
                        lsr.w   d3,d5
                        bra.b   .go_square
.set_square:
                        moveq   #16/2,d2
                        lsl.w   d3,d2
                        bra.b   .go_lines
.set_lines:
                        moveq   #32*2,d7
                        lsr.w   d3,d7
                        bra.b   .go_line
.set_line:
                        ; set the pixel with the color index
                        move.w  d6,(a0)+
.go_line:
                        dbf     d7,.set_line
                        eor.w   d3,d6
.go_lines:
                        dbf     d2,.set_lines
.go_square:
                        dbf     d5,.set_square
                        eor.w   d3,d6
.go_squares:
                        dbf     d4,.set_squares
                        ; next pixel color index and chessboard size
                        addq.w  #1,d3
                        ; set the color for this page
                        move.w  d0,-(a1)
                        ; to the next...
                        lsl.w   d3,d0
                        dbf     d1,.set_pages

                        ; install vblank int
                        lea     vblank_int(pc),a1
                        moveq   #1,d1
                        moveq   #_VDISPST,d0
                        trap    #15

; =======================================================
never_end:
                        bra.b   never_end

; =======================================================
vblank_int:
                        ; move all the pages with scroll registers (x & y)
                        ; upper word of d3 is always 0 so no x moves
                        lea     CRT_GFX_X0-GFX_EN(a5),a6
                        ; use a byte so it warps at 256
                        addq.b  #1,d7
                        move.b  d7,d3
                        moveq   #4-1,d2
.move_chessboards:
                        move.l  d3,(a6)+
                        ; slowdown scroll for next page
                        lsr.b   #1,d3
                        dbf     d2,.move_chessboards
                        rte

; =======================================================
                        end
