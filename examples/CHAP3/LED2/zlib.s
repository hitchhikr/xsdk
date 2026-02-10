;
;		Control Zmusic.
;
;		void z_init();
;		int	z_alloc(track,size);
;		int	z_assign(channel,track);
;		int	z_play(d2,d3,d4);
;		int	z_stat(bitptr);
;		int	z_stop(d2,d3,d4);
;		int	z_cont(d2,d3,d4);
;		int	z_init_all();
;		int	z_int_stop();
;		int	play_cnv_data(size,adrs);
;		int	se_play(track,mmlstr);
;		int	se_adpcm1(pcmadr,size,pfw);
;		int	se_adpcm2(NOTE,pfw);
;		int	fade_out(speed);
;
;			Programmed by Momo 1991 Dec.

	public	z_init
	public	z_alloc
	public	z_assign
	public	z_play
	public	z_stat
	public	z_stop
	public	z_cont
	public	z_init_all
	public	z_int_stop
	public	play_cnv_data
	public	se_play
	public	se_adpcm1
	public	se_adpcm2
	public	fade_out
	public	z_chk_drv
	public	z_get_wk_tbl

	include	"iocscalls.inc"

ZTRAP	macro
	moveq.l	#\1,d1
	trap	#3
	endm
        rsset 4
par1	rs.l	1
par2	rs.l	1
par3	rs.l	1
par4	rs.l	1
par5	rs.l	1
par6	rs.l	1
par7	rs.l	1
par8	rs.l	1

	section .text,code
z_init:
	ZTRAP	0
	rts
z_init_all:
	ZTRAP	0
	rts
z_int_stop
	ZTRAP	0
	rts

z_alloc:
	move.l	par1(sp),d2
	swap	d2
	move.l	par2(sp),d0
	beq	zalcer0
	subq.l	#1,d0
	cmp.l	#$10000,d0
	bcc	zalcer0
	move.w	d0,d2
	ZTRAP	$1
	rts
zalcer0:	moveq.l	#-1,d0
	rts

z_assign:
	move.l	par1(sp),d2
	swap	d2
	move.l	par2(sp),d0
	move.w	d0,d2
	ZTRAP	$2
	rts

play_cnv_data:
	move.l	par1(sp),d2
	move.l	par2(sp),a1
	addq.l	#7,a1
	ZTRAP	$11
	rts

z_play:
	movem.l	d3-d4,-(sp)

	move.l	12(sp),d2
	move.l	16(sp),d3
	move.l	20(sp),d4
	ZTRAP	$08

	movem.l	(sp)+,d3-d4
	rts

z_stop:
	movem.l	d3-d4,-(sp)

	move.l	12(sp),d2
	move.l	16(sp),d3
	move.l	20(sp),d4
	ZTRAP	$0a

	movem.l	(sp)+,d3-d4
	rts

z_cont:
	movem.l	d3-d4,-(sp)

	move.l	12(sp),d2
	move.l	16(sp),d3
	move.l	20(sp),d4
	ZTRAP	$11

	movem.l	(sp)+,d3-d4
	rts

se_play:
	move.l	par1(sp),d2
	move.l	par2(sp),a1
	ZTRAP	$12
	rts


se_adpcm1:
	movem.l	d3,-(sp)

	move.l	8(sp),a1
	move.l	12(sp),d2
	move.l	16(sp),d3
	ZTRAP	$13

	movem.l	(sp)+,d3
	rts

se_adpcm2:
	move.l	d3,-(sp)

	move.l	8(sp),d2
	move.l	12(sp),d3
	ZTRAP	$14

	move.l	(sp)+,d3
	rts

fade_out:
	move.l	par1(sp),d2
	ZTRAP	$1a
	rts

z_stat:
	move.l	par1(sp),d2
	ZTRAP	$09
	rts

; > eq=Resident confirmation
; > mi=not resident
z_chk_drv:
	move.l  $8c.w,a0
	subq.w  #8,a0
	cmpi.l  #'ZmuS',(a0)+
	bne     chk_drv_err
	cmpi.w  #'iC',(a0)+
	bne     chk_drv_err
	moveq.l #0,d0
	rts
chk_drv_err:
	moveq.l #-1,d0
	rts

z_get_wk_tbl:
	moveq	#1,d2
	ZTRAP	$3c
	move.l	a0,d0
	rts

