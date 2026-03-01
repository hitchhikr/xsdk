; =====================================================
; Imploder data decruncher
; =====================================================
; source address in a0
; dest address in a1 (can be the same address as source)
; bsr explode
; =====================================================
                    xdef    _explode

EXPL_READ_BITS      MACRO
                    add.b   d3,d3
                    bne.b   .new_byte\@
                    move.b  -(a3),d3
                    addx.b  d3,d3
.new_byte\@:
                    ENDM
_explode:
                    movem.l d1-d5/a0-a5,-(a7)
                    ;uncomment this if used from C (explode(source, dest)):
                    ;movem.l  (11*4)+4(a7),a0/a1
                    lea     (a0),a3
                    lea     (a1),a4
                    lea     (a1),a5
                    cmpi.l  #'IMP!',(a0)+
                    bne.b   .not_packed
                    add.l   (a0)+,a4
                    add.l   (a0)+,a3
                    lea     (a3),a2
                    move.l  (a2)+,-(a0)
                    move.l  (a2)+,-(a0)
                    move.l  (a2)+,-(a0)
                    ; initial token_run_len
                    move.l  (a2)+,d2
                    ; token
                    move.w  (a2)+,d3
                    bmi.b   .sub_pos
                    subq.l  #1,a3
.sub_pos:
                    lea     -28(a7),a7
                    lea     (a7),a1
                    ; run_base_offs
                    move.l  (a2)+,(a1)+
                    move.l  (a2)+,(a1)+
                    move.l  (a2)+,(a1)+
                    move.l  (a2)+,(a1)+
                    ; run_extra_bits
                    move.l  (a2)+,(a1)+
                    move.l  (a2)+,(a1)+
                    move.l  (a2),(a1)
                    lea     (a7),a1
                    moveq   #0,d4
.explode_loop:
                    tst.l   d2
                    beq.b   .no_token_run_len
.copy_literals:
                    move.b  -(a3),-(a4)
                    subq.l  #1,d2
                    bne.b   .copy_literals
.no_token_run_len:
                    cmp.l   a4,a5
                    bcs.b   .proceed
                    lea     28(a7),a7
                    moveq   #-1,d0
                    cmp.l   a3,a0
                    beq.b   .depack_error
.not_packed:
                    moveq   #0,d0
.depack_error:
                    movem.l (a7)+,d1-d5/a0-a5
                    tst.l   d0
                    rts
.proceed:
                    EXPL_READ_BITS
                    bcc.b   .set_match_len_2
                    EXPL_READ_BITS
                    bcc.b   .set_match_len_3
                    EXPL_READ_BITS
                    bcc.b   .set_match_len_4
                    EXPL_READ_BITS
                    bcc.b   .set_match_len_5
                    EXPL_READ_BITS
                    bcc.b   .get_match_len
                    move.b  -(a3),d4
                    moveq   #3,d0
                    bra.b   .got_match_len
.get_match_len:
                    EXPL_READ_BITS
                    addx.b  d4,d4
                    EXPL_READ_BITS
                    addx.b  d4,d4
                    EXPL_READ_BITS
                    addx.b  d4,d4
                    addq.b  #6,d4
                    moveq   #3,d0
                    bra.b   .got_match_len
.set_match_len_5:
                    moveq   #5,d4
                    moveq   #3,d0
                    bra.b   .got_match_len
.set_match_len_4:
                    moveq   #4,d4
                    moveq   #2,d0
                    bra.b   .got_match_len
.set_match_len_3:
                    moveq   #3,d4
                    moveq   #1,d0
                    bra.b   .got_match_len
.set_match_len_2:
                    moveq   #2,d4
                    moveq   #0,d0
.got_match_len:
                    moveq   #0,d5
                    move.w  d0,d1
                    EXPL_READ_BITS
                    bcc.b   .token_run_len
                    EXPL_READ_BITS
                    bcc.b   .token_run_len_plus_4
                    move.b  token_base(pc,d0.w),d5
                    move.b  token_extra_bits+8(pc,d0.w),d0
                    bra.b   .read_token_run_len_bits
.token_run_len_plus_4:
                    moveq   #2,d5
                    addq.b  #4,d0
.token_run_len:
                    move.b  token_extra_bits(pc,d0.w),d0
.read_token_run_len_bits:
                    EXPL_READ_BITS
                    addx.w  d2,d2
                    dbf     d0,.read_token_run_len_bits
                    add.w   d5,d2
                    moveq   #0,d5
                    move.l  d5,a2
                    move.w  d1,d0
                    EXPL_READ_BITS
                    bcc.b   .match
                    add.w   d1,d1
                    EXPL_READ_BITS
                    bcc.b   .match_plus_4
                    move.w  8(a1,d1.w),a2
                    move.b  16+8(a1,d0.w),d0
                    bra.b   .read_match_bits
.match_plus_4:
                    move.w  (a1,d1.w),a2
                    addq.b  #4,d0
.match:
                    move.b  16(a1,d0.w),d0
.read_match_bits:
                    EXPL_READ_BITS
                    addx.l  d5,d5
                    subq.b  #1,d0
                    bne.b   .read_match_bits
                    addq.l  #1,a2
                    add.l   d5,a2
                    add.l   a4,a2
.copy_match:
                    move.b  -(a2),-(a4)
                    subq.b  #1,d4
                    bne.b   .copy_match
                    bra.w   .explode_loop
token_base:
                    dc.b    6,10,10,18
token_extra_bits:
                    dc.b    1-1,1-1,1-1,1-1,2-1,3-1,3-1,4-1,4-1,5-1,7-1,14-1
