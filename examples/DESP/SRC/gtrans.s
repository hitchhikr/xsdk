;
;	void gtrans( int *dst , int *src )
;
mv	macro			;	translate a line( 32 bytes )

	move.b	(a1)+,(a0)
	move.b	(a1)+,2(a0)
	move.b	(a1)+,4(a0)
	move.b	(a1)+,6(a0)
	move.b	(a1)+,8(a0)
	move.b	(a1)+,10(a0)
	move.b	(a1)+,12(a0)
	move.b	(a1)+,14(a0)
	move.b	(a1)+,16(a0)
	move.b	(a1)+,18(a0)
	move.b	(a1)+,20(a0)
	move.b	(a1)+,22(a0)
	move.b	(a1)+,24(a0)
	move.b	(a1)+,26(a0)
	move.b	(a1)+,28(a0)
	move.b	(a1)+,30(a0)
	move.b	(a1)+,32(a0)
	move.b	(a1)+,34(a0)
	move.b	(a1)+,36(a0)
	move.b	(a1)+,38(a0)
	move.b	(a1)+,40(a0)
	move.b	(a1)+,42(a0)
	move.b	(a1)+,44(a0)
	move.b	(a1)+,46(a0)
	move.b	(a1)+,48(a0)
	move.b	(a1)+,50(a0)
	move.b	(a1)+,52(a0)
	move.b	(a1)+,54(a0)
	move.b	(a1)+,56(a0)
	move.b	(a1)+,58(a0)
	move.b	(a1)+,60(a0)
	move.b	(a1),62(a0)
	adda.w	#1024,a0
	adda.w	#481,a1

	endm

	section .text,code

	xdef gtrans

gtrans:
	move.l	4(sp),a0	* a0 = dst
	move.l	8(sp),a1	* a1 = src
	movem.l d3-d7/a2-a6,-(sp)
	addq.w	#1,a0

	mv	;0
	mv
	mv
	mv
	mv
	mv	;5
	mv
	mv
	mv
	mv
	mv	;10
	mv
	mv
	mv
	mv
	mv	;15
	mv
	mv
	mv
	mv
	mv	;20
	mv
	mv
	mv

	movem.l (sp)+,d3-d7/a2-a6
	rts
	
    end
