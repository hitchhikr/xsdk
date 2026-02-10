
	xdef sp_trans
	xdef sp_wait

	xref sp_trans_adrs

SP_REG_ADRS      = $eb0000
CRTC_RASTER_ADRS = $e80012
SP_CTRL_ADRS     = $eb0808

SP_L_SIZE  = 90*4
SP_M_SIZE  = 30*4
SP_H_SIZE  = 90*4

RASTER0   = 260*2
RASTER1   = 131*2

	rsreset
low:    rs.w SP_L_SIZE
mid:    rs.w SP_M_SIZE
high:   rs.w SP_H_SIZE

	section .data,data

low_flag:
	dc.w 0
wait_flag:
	dc.w 0

	section .text,code

sp_wait:
	movem.l	a0,-(sp)
	lea		wait_flag,a0
	tst.w	(a0)
	beq		wait_true
wait_false:
	tst.w	(a0)
	bne		wait_false
	movem.l	(sp)+,a0
	rts
wait_true:
	tst.w	(a0)
	beq		wait_true
	movem.l	(sp)+,a0
	rts

sp_trans:
	movem.l	d0-d7/a0-a6,-(sp)

	movea.l	sp_trans_adrs,a0
	movea.l	#SP_REG_ADRS,a1
	lea		low_flag,a2
	tst.w	(a2)
	beq		set_high

set_low:
	clr.w	(a2)
	move.w	#RASTER1,CRTC_RASTER_ADRS
	move.w	SP_CTRL_ADRS,d0
	andi.w	#$00ff,d0
	move.w	d0,SP_CTRL_ADRS
	lea		wait_flag,a2
	tst.w	(a2)
	beq		wait_flag_on
wait_flag_off:
	clr.w	(a2)
	bra		trans_main
wait_flag_on:
	move.w	#1,(a2)
	bra		trans_main

set_high:
	move.w	#1,(a2)
	lea		mid(a0),a0
	move.w	#1,(a2)
	move.w	#RASTER0,CRTC_RASTER_ADRS

trans_main:
	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,52(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,104(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,156(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,208(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,260(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,312(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,364(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,416(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,468(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,520(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,572(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,624(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,676(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,728(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,780(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,832(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,884(a1)

	movem.l	(a0)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,936(a1)

	movem.l	(a0)+,d0-d7/a2
	movem.l	d0-d7/a2,988(a1)

	tst.w	low_flag
	bne		trans_end
	move.w	SP_CTRL_ADRS,d0
	ori.w	#$0200,d0
	move.w	d0,SP_CTRL_ADRS
trans_end:
	movem.l	(sp)+,d0-d7/a0-a6
	rte

	end


