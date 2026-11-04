;=====================================================
; ROM Debugger Enabler Driver v1.0
;=====================================================
; The debugger communicates via serial port
; so it needs RSDRV.SYS loaded before in your CONFIG.SYS:
;
; DEVICE = \SYS\RSDRV.SYS
; DEVICE = \SYS\RUNDBG.SYS
;
;=====================================================
; Type the 'h' command to display a list of all available
; commands from the debugger (once it has been triggered).
;=====================================================

;=====================================================
                section program,code

                xdef    _start

;=====================================================
_start:
                dc.l    $FFFFFFFF
                dc.w    $8020
                dc.l    strategy
                dc.l    install
                ;        ++++++++
                dc.b    'RUNDBG  '
                even
request:
                ds.l    1
strategy:
                move.l  a5,request
                rts
banner_msg:
                dc.b    13,10,"ROM Debugger Enabler v1.0",13,10
                dc.b    "Written By Franck 'hitchhikr' Charlet.",13,10,13,10,0
ready_msg:
                dc.b    $1b,"[32m","ROM Debugger Is Enabled.",$1b,"[0m",13,10,0
error_msg:
                dc.b    $1b,"[32m","ROM Debugger Not Found.",$1b,"[0m",13,10,0
                even
install:
                movem.l d0-d7/a1-a5,-(a7)
                move.l  request(pc),a5
                ; command init (0)
                tst.b   2(a5)
                bne.b   .exit
                lea     banner_msg(pc),a1
                moveq   #$21,d0
                trap    #15
                ; Search debugger entry point
                lea     $FC0000,a2
.look_for_debugger:
                cmp.w   #$2F08,(a2)
                bne.b   .next_word
                cmp.w   #$41F9,2(a2)
                bne.b   .next_word
                cmp.w   #$0000,4(a2)
                bne.b   .next_word
                cmp.w   #$1000,6(a2)
                bne.b   .next_word
                bra.b   .found
.next_word:
                addq.l  #2,a2
                cmp.l   #$FFFFFE,a2
                bne.b   .look_for_debugger
                ; not found (but shouldn't happen)
                lea     error_msg(pc),a1
                moveq   #$21,d0
                trap    #15
                move.l	#install,14(a5)
                bra.b   .exit
.found:
                move.l	#install,14(a5)
                lea     ready_msg(pc),a1
                moveq   #$21,d0
                trap    #15
                ; kickstart it
                jsr     (a2)
.exit:
                movem.l (a7)+,d0-d7/a1-a5
                moveq   #0,d0
                rts

                end
