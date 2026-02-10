;
; Cassette streamer tape driver for X680x0/Human68k
;
    mc68000
    opt o+
    opt all+

	include	doscalls.inc
	section .text,code

    xdef _start

; Device Header
_start:
Deviceheader:
	dc.l	-1		        ; -1 since only one pointer to the next device driver is supported
	dc.w	$8020		    ; Device attributes Character device ioctrl not allowed in RAW MODE
	dc.l	Strategy	    ; Strategy Routine Entry Point
	dc.l	Interrupt	    ; Entry point of the interrupting routine
DeviceFName:
	dc.b	'        '	    ; Device Name

; Request Buffer

Request:
    dc.l	0		        ; Stores a pointer to the request header

; Strategy Routines
; input
; a5 : A pointer to the request header
; return
; None

Strategy:
	move.l	a5,Request
	rts

; SCSI ID storage

targetID:
	dc.l	4		        ; Stores the drive's SCSI ID

; Interrupt Routine
; input
; None
; return
; d0 : (Device driver) error code

Interrupt:
	movem.l	d1/a4-a5,-(sp)
	movea.l	Request,a5
	lea	JmpTbl,a4
	clr.l	d0
	move.b	2(a5),d0	    ; Enter the command code in d0
	lsl.w	#2,d0		    ; Table jump
	adda.l	d0,a4
	movea.l	(a4),a4
	jsr	(a4)
	move.w	d0,d1
	move.b	d1,3(a5)	    ; Error code (lo 8 bits) setting
	lsr.w	#8,d1
	move.b	d1,4(a5)	    ; Error code (hi 8 bits) setting
	movem.l	(sp)+,d1/a4-a5
	rts

JmpTbl:
	dc.l	Init_all	    ;  0 initialization
	dc.l	ErrorReturn	    ;  1 Error
	dc.l	ErrorReturn	    ;  2 Unused
	dc.l	ErrorReturn	    ;  3 Input via IOCTRL
	dc.l	Input		    ;  4 Input
	dc.l	ErrorReturn	    ;  5 Type-ahead input
	dc.l	ChkIOStatus	    ;  6 Input Status Check
	dc.l	ReturnTRUE	    ;  7 Clear the input buffer
	dc.l	Output_		    ;  8 Output (verify off)
	dc.l	Output_		    ;  9 Output (verify on)
	dc.l	ChkIOStatus	    ; 10 Output status check
	dc.l	ErrorReturn	    ; 11 Unused
	dc.l	ErrorReturn	    ; 12 Output by IOCTRL

ErrorReturn:
	move.w	#$1003,d0	    ; Abort/Invalid command code
	rts

ReturnTRUE:
	clr.w	d0
	rts

; Command code 6,10 Input/output status check
; input
; a5 : A pointer to the request header
; return
; d0 : (Device driver) error code

ChkIOStatus:
	bsr	TestReady
	tst.l	d0
	bne	ChkIOStatusOK
	move.w	#1,d0
	bra	ChkIOStatusEND
ChkIOStatusOK:
	clr.w	d0
ChkIOStatusEND:
	rts

; Check drive availability
; input
; None
; return
; d0 : 0  Not available
;      1  Available
;      2  Available (media replaced)

TestReady:
	movem.l	d1/d4,-(sp)
	move.l	targetID,d4	    ; The drive's SCSI ID
	move.l	#$24,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; S_TESTUNIT
	tst.w	d0
	beq	TestReadyOK
	move.l	targetID,d4	    ; The drive's SCSI ID
	move.l	#$24,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; S_TESTUNIT
	tst.w	d0
	beq	TestReadyCH
	clr.l	d0
	bra	TestReadyEND
TestReadyCH:
	moveq.l	#2,d0
	bra	TestReadyEND
TestReadyOK:
	moveq.l	#1,d0
TestReadyEND:
	movem.l	(sp)+,d1/d4
	rts

; Enter command code 4
; input
; a5 : A pointer to the request header
; return
; d0 : (Device driver) error code

Input:
	movem.l	d1-d4/a1,-(sp)
	bsr	TestReady
	tst.l	d0
	beq	NotReady
ToRead:
	move.l	targetID,d4
	bsr	busy_wait
	tst.l	d0
	bne	ReadErr
	move.l	18(a5),d3	    ; Number of bytes of data to transfer
	move.l	d3,d0		    ; Input and output is only possible in 512-byte units.
	andi.l	#$1ff,d0	    ; If there is a fraction
	bne	OddDataSize	        ; Treat as an error
	lsr.l	#8,d3
	lsr.l	#1,d3		    ; Number of blocks when 1 block is 512 bytes
	move.l	14(a5),a1	    ; A pointer to the transfer buffer
	move.l	targetID,d4
	bsr	tape_READ
	tst.l	d0
	beq	OutPutEND
ReadErr:
	move.w	#$700b,d0	    ; Abort, Retry, Ignore/Read Error
	bra	OutPutEND

; Command code 8,9 output Since it is a sequential access device,
; the verify function is omitted.
; input
; a5 : A pointer to the request header
; return
; d0 : (Device driver) error code

Output_:
	movem.l	d1-d4/a1,-(sp)
	bsr	TestReady
	tst.w	d0
	beq	NotReady
ToWrite:
	move.l	targetID,d4
	bsr	busy_wait
	tst.l	d0
	bne	WriteErr
	move.l	targetID,d4
	bsr	ChkProtect	        ; Write protect check
	cmpi.l	#1,d0
	beq	Protected
	tst.l	d0
	bne	WriteErr
	move.l	18(a5),d3	    ; Number of bytes of data to transfer
	move.l	d3,d0		    ; Input and output is only in 512-byte units
	andi.l	#$1ff,d0	    ; If there is a fraction
	bne	OddDataSize	        ; Treat as an error
	lsr.l	#8,d3
	lsr.l	#1,d3		    ; Number of blocks when 1 block is 512 bytes
	move.l	14(a5),a1	    ; A pointer to the transfer buffer
	move.l	targetID,d4
	bsr	tape_WRITE
	tst.l	d0
	beq	OutPutEND
WriteErr:
	move.w	#$700a,d0	    ; Abort, Retry, Ignore/Write Error
	bra	OutPutEND
NotReady:
	move.w	#$3002,d0	    ; Abort, Retry/Drive not ready
	bra	OutPutEND
OddDataSize:			    ; There is no suitable error, so substitute below:
	move.w	#$1008,d0	    ; Abort/Sector not found
	bra	OutPutEND
Protected:
	move.w	#$300d,d0	    ; Abort, Retry/Write Protect
OutPutEND:
	movem.l	(sp)+,d1-d4/a1
	rts

; Write protect check
; input
; d4 : target ID
; return
; d0 : 0 Not protected
;      1 Protected
;     -1 An error occurred

ChkProtect:
	link	a5,#-16
	movem.l	d1-d3/a1,-(sp)
	lea	-$10(a5),a1	        ; Received Data Buffer
	move.l	#$10,d3		    ; The length of the data to be received
	clr.l	d2		        ; Page code (currently 0)
	move.l	#$29,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; modesense
	tst.l	d0
	beq	ChkProtectCHK
	moveq.l	#$ff,d0
	bra	ChkProtectEND
ChkProtectCHK:
	btst.b	#$0007,2(a1)	; Check the bit that indicates write protection
	beq	ChkProtectEND
	moveq	#$1,d0
ChkProtectEND:
	movem.l	(sp)+,d1-d3/a1
	unlk	a5
	rts

; Waiting for the drive to process Before issuing a read/write command,
; wait until the drive has finished processing the previous command.
; With the streamer drive I have, I was able to check whether the processing was complete by checking
; a specific byte of the data obtained with S_REQUEST, so I used that method.
; input
; d4 : The SCSI ID of the target
; return
; d0 : 0        Processing pending
;      Non-zero An error occurred

busy_wait:
	link	a5,#-64
	movem.l	d1/d3/a1,-(sp)
	lea	-$40(a5),a1
req_wait:
	move.l	#$40,d3		    ; The length of the data to be received
	move.l	#$2c,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; S_REQUEST
	tst.l	d0
	bne	req_error
	tst.b	6(a1)		    ; After issuing a read/write command,
                            ; this counter decreases over time,
                            ; and when it reaches 0, the process is complete.
	bne	req_wait
	clr.l	d0
req_error:
	movem.l	(sp)+,d1/d3/a1
	unlk	a5
	rts

; The CDB sent to the write drive is $0a $01 num2 num1 num0 $00 (num is the number of blocks),
; but the CDB created by S_WRITE is $0a pos2 pos1 pos0 num $00 (pos is specified by d2, num is specified by d3),
; so we need to tweak it so that the CDB is created properly by S_WRITE.
; input
; d3 : Number of blocks to write
; d4 : The SCSI ID of the target
; a1 : A pointer to the data to write
; return
; d0 : (SCSI IOCS) error code

tape_WRITE:
	movem.l	d1-d2/d5,-(sp)
	clr.l	d2
	move.l	d3,d0
	andi.l	#$ff,d3		    ; Store the lower 8 bits of the input d3 in d3
	lsr.l	#8,d0
	move.w	d0,d2		    ; Store the upper 16 bits in d2.
	ori.l	#$00010000,d2	; Set 1 in the second byte of CDB
	move.l	#$1,d5		    ; Block size 512 bytes
	move.l	#$22,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; S_WRITE
	movem.l	(sp)+,d1-d2/d5
	rts

; The CDB sent to the read drive is $08 $01 num2 num1 num0 $00 (num is the number of blocks),
; but the CDB created by S_READ is $08 pos2 pos1 pos0 num $00 (pos is specified by d2, num is specified by d3),
; so we need to tweak it so that the CDB is created properly by S_READ.
; input
; d3 : Number of blocks to read
; d4 : The SCSI ID of the target
; a1 : A pointer to the buffer to read from
; return
; d0 : (SCSI IOCS) error code

tape_READ:
	movem.l	d1-d2/d5,-(sp)
	clr.l	d2
	move.l	d3,d0
	andi.l	#$ff,d3		    ; Store the lower 8 bits of the input d3 in d3
	lsr.l	#8,d0
	move.w	d0,d2		    ; Store the upper 16 bits in d2.
	ori.l	#$00010000,d2	; Set 1 in the second byte of CDB
	move.l	#$1,d5		    ; Block size 512 bytes
	move.l	#$21,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; S_READ
	movem.l	(sp)+,d1-d2/d5
	rts

; Command code 0 Initialization
; input
; a5 : A pointer to the request header
; return
; d0 : (Device driver) error code

Init_all:
	pea	TitleMes	        ; Title display
	DOS	_PRINT
	addq.l	#4,sp
	clr.l	d0
    ; Parameter Check
	movem.l	a1,-(sp)
	move.l	#Init_all,14(a5)
	move.l	18(a5),a1	    ; Parameter pointer
InitSkip_arg0:
	move.b	(a1)+,d0
	bne	InitSkip_arg0	    ; Skip the first parameter
	link	a5,#-80
	movem.l	a2-a3,-(sp)
InitNextArg:
	lea	-80(a5),a2
	move.w	#80,d0
InitOneArg:
	move.b	(a1)+,(a2)+
	dbeq	d0,InitOneArg
	lea	-80(a5),a2	        ; A pointer to one parameter
	tst.b	(a2)		    ; If it's 0, it's over
	beq	InitArgEND
	move.l	(a2)+,d0
	cmpi.l	#'/id=',d0
	beq	InitSetID
	cmpi.l	#'-id=',d0
	beq	InitSetID
	cmpi.l	#'/fn=',d0
	beq	InitSetName
	cmpi.l	#'-fn=',d0
	beq	InitSetName
	bra	InitNextArg
InitSetID:
	move.b	(a2)+,d0
	andi.l	#7,d0		    ; d0 contains the specified SCSI ID (a shortcut)
	move.l	d0,targetID
	move.w	#-1,IDsetFlg	; The drive's SCSI ID is
	bra	InitNextArg
InitSetName:			    ; Set the device name
	move.w	#7,d0
	lea	DeviceFName,a3
InitSetNset:
	move.b	(a2)+,(a3)+
	dbeq	d0,InitSetNset	; Copy up to '\0'
	tst.w	d0		        ; All 8 characters
	bmi	InitSetEnd	        ; If I don't fill it up
	move.b	#' ',-(a3)	    ; Replace '\0' with a space
InitSetEnd:			        ; The device name is set
	move.w	#-1,DevNamesetFlg
	bra	InitNextArg
InitArgEND:
	movem.l	(sp)+,a2-a3
	unlk	a5

	tst.w	DevNamesetFlg	; If the device name is set
	bne	InitDefNameEND	    ; to the next
	movem.l	a2-a3,-(sp)	    ; If not, set it up yourself
	move.w	#7,d0
	lea	DeviceFName,a3
	lea	DevDefaultName,a2
InitDefNameCP:
	move.b	(a2)+,(a3)+
	dbf	d0,InitDefNameCP
	movem.l	(sp)+,a2-a3
InitDefNameEND:
	tst.w	IDsetFlg	    ; If the drive's SCSI ID is specified
	bne	InitDefIDEnd	    ; to the next
	pea	IDNotSetMes	        ; If not, an error
	DOS	_PRINT
	addq.l	#4,sp
	move.w	#$700d,d0
	bra	InitERR
InitDefIDEnd:

	move.l	targetID,d0	    ; Displays the drive's SCSI ID
	addi.w	#'0',d0
	move.w	d0,-(sp)
	DOS	_PUTCHAR
	addq.l	#2,sp
	bsr	pr_inquiry	        ; View Inquiry Data
	pea	DevName		        ; Show Device Name
	DOS	_PRINT
	addq.l	#4,sp
	pea	DeviceFName
	DOS	_PRINT
	addq.l	#4,sp
	pea	CRLF
	DOS	_PRINT
	addq.l	#4,sp

InitEND:
	clr.l	d0
InitERR:
	movem.l	(sp)+,a1
	rts

; View Inquiry Data
; input
; None
; return
; None

pr_inquiry:
	link	a5,#-50
	movem.l	d1/d3-d4/a1-a2,-(sp)
	lea.l	-50(a5),a1
	movea.l	a1,a2
	move.l	#$2f,d0
inq_0:
	clr.b	(a1)+
	dbf	d0,inq_0
	movea.l	a2,a1		    ; A pointer to the receive buffer
	move.l	targetID,d4	    ; The drive's SCSI ID
	move.l	#$30,d3		    ; The length of the data to be received
	move.l	#$20,d1		    ; SCSI call number
	move.l	#$f5,d0		    ; IOCS Call Number
	trap	#$0f		    ; S_INQUIRY
	tst.l	d0
	bne	inq_end
	pea	IdMes
	DOS	_PRINT
	addq.l	#4,sp
	pea	8(a1)		        ; The first 8 bytes of the INQUIRY data are ignored
	DOS	_PRINT
	addq.l	#4,sp
	cmpi.b	#1,(a1)		    ; Checking the device type
	beq	inq_end		        ; If it's not a sequential access device
	pea	NotTapeMes	        ; Message display
	DOS	_PRINT
	addq.l	#4,sp
inq_end:
	movem.l	(sp)+,d1/d3-d4/a1-a2
	unlk	a5
	pea	CRLF
	DOS	_PRINT
	addq.l	#4,sp
	rts

	section .data,data

IDsetFlg:
	dc.w	0
DevNamesetFlg:
	dc.w	0
DevDefaultName:
	dc.b	'ST0     ',$00
	even
TitleMes:
	dc.b	$0d,$0a
	dc.b	'Magnetic Tape Driver ver.1.12N by J.Mitsugi(KMC)',$0d,$0a
	dc.b	'SCSI ID : ',$00
	even
IDNotSetMes:
	dc.b	'not set',$0d,$0a,$00
	even
IdMes:
	dc.b	$0d,$0a,'Identifier : ',$00
	even
NotTapeMes:
	dc.b	$0d,$0a
	dc.b	'Warning: target device is not sequensial access device',$00
	even
DevName:
	dc.b	'device file name is ',$00
	even
IllegalMes:
	dc.b	'illegal '
CRLF:
	dc.b	$0d,$0a,$00

	end
