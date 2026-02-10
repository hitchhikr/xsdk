;
;	disp.s	( for Solf NT )
;		write data to sram
;
;		Original by ici (KMC)	Jan. 16, 1990
;		Modified by peach(KMC)

;	REGISTERS
;	d0 : always ZERO ?
;	d1 : always 0x20
;	a0 : pointer to BYTE data
;	a1 : pointer to WORD data
;	a2 : pointer to VRAM
;	a3 : jmp address (CMD & RL)
;	a4 : jmp address (MULTI & MOVE)
;	d2-d7/a5-a6 : move multi

TRANS52		macro
	movem.l	(a1)+,d0-d7/a2-a6
	movem.l	d0-d7/a2-a6,\1*52(a0)
	endm

MOVED2A2_8	macro
	move.l	d2,(a2)+
	move.l	d2,(a2)+
	move.l	d2,(a2)+
	move.l	d2,(a2)+
	move.l	d2,(a2)+
	move.l	d2,(a2)+
	move.l	d2,(a2)+
	move.l	d2,(a2)+
		endm

MOVEM_8	macro
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	movem.l	d2-d7/a5-a6,(a2)
	adda.l	d1,a2
	endm

MOVEA1A2_8	macro
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
		endm

SET_D2	macro
	move.w	(a1)+,d2
	move.w	d2,d3
	swap.w	d2
	move.w	d3,d2
	endm

NEXT	macro
	move.b	(a0)+,d0
	jmp	CMDOFS(a3,d0.w)
	endm

CMDOFS		equ	$7e
RLOFS		equ	rl-cmds+CMDOFS
MOVEOFS		equ	$7e
MULTIOFS	equ	multi-mv+MOVEOFS

;
;
;_trans1
;	int	trans1( void *cmdp , void *colp , void *dstp );
;

	xdef	trans1
	section .text,code

trans1:
	link	a6,#0
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	8(a6),a0
	move.l	12(a6),a1
	move.l	16(a6),a2
	moveq	#0,d0
	moveq	#32,d1
	move.l	#cmds-CMDOFS,a3
	move.l	#mv-MOVEOFS,a4
	NEXT

;
;	trans2( void *dst , void *src ) ;
;		This is a simple solid transfer

	xdef	_trans2
	section .text,code
_trans2:
	link	a6,#0
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	8(a6),a0		*dst
	move.l	12(a6),a1		*src

	TRANS52	0
	TRANS52	1
	TRANS52	2
	TRANS52	3
	TRANS52	4
	TRANS52	5
	TRANS52	6
	TRANS52	7
	TRANS52	8
	movem.l	(a1)+,d0-d7/a2-a4 	* 8 + 3 = 11
	movem.l	d0-d7/a2-a4,9*52(a0)

	movem.l	(sp)+,d0-d7/a0-a6
	unlk	a6
	rts



;****************************************************************
rl:
	MOVED2A2_8
	MOVED2A2_8
	MOVED2A2_8
	MOVED2A2_8
	MOVED2A2_8
	MOVED2A2_8
	NEXT
;****************************************************************
do_dataend:
	movem.l	(sp)+,d0-d7/a0-a6
	unlk	a6
	rts
;****************************************************************
cmds:
dataend:	bra.s	do_dataend
rl_4:		SET_D2
rl_4_noset:	move.l	d2,(a2)+
rl_2_noset:	move.l	d2,(a2)+
		NEXT
rl_5:		SET_D2
rl_5_noset:	move.l	d2,(a2)+
rl_3_noset:	move.l	d2,(a2)+
rl_1_noset:	move.w	d2,(a2)+
		NEXT
rl_6:		SET_D2
rl_6_noset:	move.l	d2,(a2)+
		move.l	d2,(a2)+
		move.l	d2,(a2)+
		NEXT
rl_7:		SET_D2
rl_7_noset:	move.l	d2,(a2)+
		move.l	d2,(a2)+
		move.l	d2,(a2)+
		move.w	d2,(a2)+
		NEXT
rl_8:		SET_D2
rl_8_noset:	move.l	d2,(a2)+
		move.l	d2,(a2)+
		move.l	d2,(a2)+
		move.l	d2,(a2)+
		NEXT
rl_odd:		SET_D2
rl_odd_noset:	move.w	d2,(a2)+
		move.b	(a0)+,d0
		jmp	RLOFS(a3,d0.w)
rl_even:	SET_D2
rl_even_noset:	move.b	(a0)+,d0
		jmp	RLOFS(a3,d0.w)
rl_multi:	SET_D2
rl_multi_noset:	move.l	d2,d3
		move.l	d2,d4
		move.l	d2,d5
		move.l	d2,d6
		move.l	d2,d7
		move.l	d2,a5
		move.l	d2,a6
rl_multi_nosetregs:
		move.b	(a0)+,d0
		jmp	MULTIOFS(a4,d0.w)
mv_odd:		move.w	(a1)+,(a2)+
		move.b	(a0)+,d0
		jmp	MOVEOFS(a4,d0.w)
mv_even:	move.b	(a0)+,d0
		jmp	MOVEOFS(a4,d0.w)
mv_29:		move.l	(a1)+,(a2)+
mv_27:		move.l	(a1)+,(a2)+
mv_25:		move.l	(a1)+,(a2)+
mv_23:		move.l	(a1)+,(a2)+
mv_21:		move.l	(a1)+,(a2)+
mv_19:		move.l	(a1)+,(a2)+
mv_17:		move.l	(a1)+,(a2)+
mv_15:		move.l	(a1)+,(a2)+
mv_13:		move.l	(a1)+,(a2)+
mv_11:		move.l	(a1)+,(a2)+
mv_9:		move.l	(a1)+,(a2)+
mv_7:		move.l	(a1)+,(a2)+
mv_5:		move.l	(a1)+,(a2)+
mv_3:		move.l	(a1)+,(a2)+
mv_1:		move.w	(a1)+,(a2)+
		NEXT
mv_28:		move.l	(a1)+,(a2)+
mv_26:		move.l	(a1)+,(a2)+
mv_24:		move.l	(a1)+,(a2)+
mv_22:		move.l	(a1)+,(a2)+
mv_20:		move.l	(a1)+,(a2)+
mv_18:		move.l	(a1)+,(a2)+
mv_16:		move.l	(a1)+,(a2)+
mv_14:		move.l	(a1)+,(a2)+
mv_12:		move.l	(a1)+,(a2)+
mv_10:		move.l	(a1)+,(a2)+
mv_8:		move.l	(a1)+,(a2)+
mv_6:		move.l	(a1)+,(a2)+
mv_4:		move.l	(a1)+,(a2)+
mv_2:		move.l	(a1)+,(a2)+
		NEXT
skip_w:		adda.w	(a1)+,a2
		NEXT
skip_512:	lea	512(a2),a2
		NEXT

;****************************************************************
multi:
	MOVEM_8
	MOVEM_8
	MOVEM_8
	MOVEM_8
	MOVEM_8
	NEXT
;****************************************************************
;	If you jump to mv, it moves 284 words (284 = 256 + 28).
mv:
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8

	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8

	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8
	MOVEA1A2_8

	MOVEA1A2_8
	MOVEA1A2_8
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	move.l	(a1)+,(a2)+
	NEXT

;****************************************************************
;*
;*	CMD offset table
;*
;****************************************************************

	xdef	_ofs_dataend
	xdef	_ofs_rl_1
	xdef	_ofs_rl_odd,_ofs_rl_even
	xdef	_ofs_rl_multi,_ofs_rl_multi_noset,_ofs_rl_multi_nosetregs
	xdef	_ofs_mv_odd,_ofs_mv_even
	xdef	_ofs_mv_1
	xdef	_ofs_skip_w,_ofs_skip_512

_ofs_dataend:	dc.b	dataend-cmds
_ofs_rl_1:	dc.b	rl_1_noset-cmds-8
_ofs_rl_2:	dc.b	rl_2_noset-cmds-8
_ofs_rl_3:	dc.b	rl_3_noset-cmds-8
_ofs_rl_4:	dc.b	rl_4-cmds
_ofs_rl_5:	dc.b	rl_5-cmds
_ofs_rl_6:	dc.b	rl_6-cmds
_ofs_rl_7:	dc.b	rl_7-cmds
_ofs_rl_8:	dc.b	rl_8-cmds
_ofs_rl_odd:	dc.b	rl_odd-cmds
_ofs_rl_even:	dc.b	rl_even-cmds
_ofs_rl_multi:	dc.b	rl_multi-cmds
_ofs_rl_multi_noset:	dc.b	rl_multi_noset-cmds
_ofs_rl_multi_nosetregs:	dc.b	rl_multi_nosetregs-cmds
_ofs_mv_odd:	dc.b	mv_odd-cmds
_ofs_mv_even:	dc.b	mv_even-cmds
_ofs_mv_1:	dc.b	mv_1-cmds
_ofs_mv_2:	dc.b	mv_2-cmds
_ofs_mv_3:	dc.b	mv_3-cmds
_ofs_mv_4:	dc.b	mv_4-cmds
_ofs_mv_5:	dc.b	mv_5-cmds
_ofs_mv_6:	dc.b	mv_6-cmds
_ofs_mv_7:	dc.b	mv_7-cmds
_ofs_mv_8:	dc.b	mv_8-cmds
_ofs_mv_9:	dc.b	mv_9-cmds
_ofs_mv_10:	dc.b	mv_10-cmds
_ofs_mv_11:	dc.b	mv_11-cmds
_ofs_mv_12:	dc.b	mv_12-cmds
_ofs_mv_13:	dc.b	mv_13-cmds
_ofs_mv_14:	dc.b	mv_14-cmds
_ofs_mv_15:	dc.b	mv_15-cmds
_ofs_mv_16:	dc.b	mv_16-cmds
_ofs_mv_17:	dc.b	mv_17-cmds
_ofs_mv_18:	dc.b	mv_18-cmds
_ofs_mv_19:	dc.b	mv_19-cmds
_ofs_mv_20:	dc.b	mv_20-cmds
_ofs_mv_21:	dc.b	mv_21-cmds
_ofs_mv_22:	dc.b	mv_22-cmds
_ofs_mv_23:	dc.b	mv_23-cmds
_ofs_mv_24:	dc.b	mv_24-cmds
_ofs_mv_25:	dc.b	mv_25-cmds
_ofs_mv_26:	dc.b	mv_26-cmds
_ofs_mv_27:	dc.b	mv_27-cmds
_ofs_mv_28:	dc.b	mv_28-cmds
_ofs_mv_29:	dc.b	mv_29-cmds
_ofs_skip_w:	dc.b	skip_w-cmds
_ofs_skip_512:	dc.b	skip_512-cmds

	even

;****************************************************************
;*
;*
;*_long_clear
;*
;*
	xdef	_long_clear
	section .text,code

_long_clear:
	link	a6,#0
	move.l	a0,-(sp)
	move.l	8(a6),a0
	move.l	12(a6),d0
L1:
	clr.l	(a0)+
	subq.l	#1,d0
	bne	L1
	move.l	(sp)+,a0
	unlk	a6
	rts

	end
    
