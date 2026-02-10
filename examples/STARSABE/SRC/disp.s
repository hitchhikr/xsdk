;	disp.s : low level display program

; NO_APP

GPIP		equ	$e88001
TEXT_END	equ	$e80000
GRAP_END	equ	$d00000
SPCON		equ	$eb0000

	section .text,code

	xdef	SpDisp
	xdef	Vdispwait
	xdef	hsyncwait
	xdef	tx_clr
	xdef	gr_clr

	xref	spbuf

;SpDisp()

SEND		macro

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,\1*208(a6)

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,\1*208+52(a6)

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,\1*208+104(a6)

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,\1*208+156(a6)

		endm

SpDisp:
		movem.l	d0-d7/a0-a6,-(sp)
		lea.l	spbuf,a5
		lea.l	SPCON,a6

		SEND	0
		SEND	1
		SEND	2
		SEND	3

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,832(a6)

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,832+52(a6)

		movem.l	(a5)+,d0-d7/a0-a4
		movem.l	d0-d7/a0-a4,832+104(a6)


		movem.l	(a5)+,d0-d7/a0
		movem.l	d0-d7/a0,832+156(a6)

		movem.l	(sp)+,d0-d7/a0-a6
		rts

;vdispwait()

Vdispwait:
		movea.l #GPIP,a0
_vloop1:
		btst.b	#4,(a0)
		beq	_vloop1
_vloop2:
		btst.b	#4,(a0)
		bne	_vloop2
		rts

;hsyncwait()

hsyncwait:
		movea.l #GPIP,a0
_hloop1:
		btst.b	#7,(a0)
		beq	_hloop1
_hloop2:
		btst.b	#7,(a0)
		bne	_hloop2
		rts

;tx_clr()

CLR_		macro

		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)
		movem.l	d0-d3/a0-a3,-(a4)

		endm

tx_clr:
		movem.l	d0-d7/a0-a6,-(sp)
		movea.l	#TEXT_END,a4

		move.l	#0,d0
		move.l	#0,d1
		move.l	#0,d2
		move.l	#0,d3
		movea.l	#0,a0
		movea.l	#0,a1
		movea.l	#0,a2
		movea.l	#0,a3

		move.w	#$ff,d4
_tloop:
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_

		dbra	d4,_tloop
		movem.l	(sp)+,d0-d7/a0-a6
		rts

;gr_clr()

gr_clr:
		movem.l	d0-d7/a0-a6,-(sp)
		movea.l	#GRAP_END,a4

		move.l	#0,d0
		move.l	#0,d1
		move.l	#0,d2
		move.l	#0,d3
		movea.l	#0,a0
		movea.l	#0,a1
		movea.l	#0,a2
		movea.l	#0,a3

		move.w	#$ff,d4
_gloop:
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_
		CLR_

		dbra	d4,_gloop
		movem.l	(sp)+,d0-d7/a0-a6
		rts

	end
