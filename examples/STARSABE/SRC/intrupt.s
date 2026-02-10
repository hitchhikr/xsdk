;	intrupt.s : interrupt program

; NO_APP

	section .text,code

	xref	v_int_count
	xdef    VInterrupt
	xdef    CInterrupt
	xdef    RInterrupt

;VInterrupt()
VInterrupt:
	and.b #191,$E88015
	addq.l #1,v_int_count
	or.b #64,$E88015
	rte

;CInterrupt()
CInterrupt:
	rte

;RInterrupt()
RInterrupt:
	rte

	end
