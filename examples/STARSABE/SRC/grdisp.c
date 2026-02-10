/*	grdisp.c : graphic display program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	"main.h"
#include	"calc.h"
#include	"dim.h"
#include	"star.h"
#include	"disp.h"

#define	GPIP		(volatile unsigned char *)0xe88001

#define	S0X		(volatile unsigned short *)0xe80018
#define	S0Y		(volatile unsigned short *)0xe8001a
#define	S1X		(volatile unsigned short *)0xe8001c
#define	S1Y		(volatile unsigned short *)0xe8001e
#define	S2X		(volatile unsigned short *)0xe80020
#define	S2Y		(volatile unsigned short *)0xe80022
#define	S3X		(volatile unsigned short *)0xe80024
#define	S3Y		(volatile unsigned short *)0xe80026

#define	GPAGE0		(volatile unsigned short *)0xc00000
#define	GPAGE1		(volatile unsigned short *)0xc80000

#define	VRAM0(x, y)	(GPAGE0 + (x) + (y) * 512)
#define	VRAM1(x, y)	(GPAGE0 + (x) + (y) * 512)
#define	VRAM2(x, y)	(GPAGE1 + (x) + (y) * 512)

#define	GPALET_		(volatile unsigned short *)0xe82000

#define	RGB(r, g, b)	((r) * 0x40 + (g) * 0x800 + (b) * 2)
#define	R(c)		((c & 0x7C0) >> 6)
#define	G(c)		((c & 0xf800) >> 11)
#define	B(c)		((c & 0x3e) >> 1)

#define	COL0(c)		((c) * 16)		/*	0~7	*/
#define	COL1(c)		(c)			/*	0~15	*/
#define	COL2(c)		((c) + 128)		/*	0~127	*/

#define	PAL0(c)		(GPALET_ + COL0(c))	/*	0~7	*/
#define	PAL1(c)		(GPALET_ + COL1(c))	/*	0~15	*/
#define	PAL2(c)		(GPALET_ + COL2(c))	/*	0~127	*/

#define	SCROLL0(x, y)	(*S1X=(x),*S1Y=(y))
#define	SCROLL1(x, y)	(*S0X=(x),*S0Y=(y))
#define	SCROLL2(x, y)	(*S2X=(x),*S2Y=(y),*S3X=(x),*S3Y=(y))

#define	PALETINIT	0xff

#define	BASEPAL		1


/*number	0			1		2			*/
/*priority	0-1			0-0		1-X			*/
/*color		16-112 / 16 ( 1-7 )	1-15		128-255			*/
/*	        7êF                  15êF		    128êF			*/
/*divide	star			    planet		space			*/

const int		star_color[4] = {1, 2, 2, 1};
unsigned short		*imgbuf[STAGEMAX + 2];
static const char	*imgname[STAGEMAX + 2] = {
	"stage1.pic", "stage2.pic", "stage3.pic", "stage4.pic", "stage5.pic",
	"stage6.pic", "stage7.pic", "default.pic", "default.pic", "stage3n2.pic", "control.pic" };
unsigned short		gpalet[128], gpalet16[16];
unsigned char		paletcheck[32768];
static const unsigned short	st5col[9] = {	RGB(0, 0, 0),
						RGB(0, 0, 0),
						RGB(6, 0, 7),
						RGB(7, 0, 8),
						RGB(8, 0, 10),
						RGB(10, 0, 12),
						RGB(8, 0, 10),
						RGB(7, 0, 8),
						RGB(6, 0, 7)		};

void	ReadPic(void)
{
	FILE	*fp;
	int	i, size;
	char	tmpbuf;

	for (i = 0; i < STAGEMAX + 2; ++i) {
		tmpbuf = 0;
		if (NULL == (fp = fopen(imgname[i], "rb"))) {
			fprintf(stdout, "pic file %s open error." ,imgname[i]);
			exit(1);
		}
		size = FILELENGTH(fp);
		if ((imgbuf[i] = (unsigned short *)MALLOC(size)) >= (unsigned short *)0x81000000) {
			fclose(fp);
			fprintf(stdout, "pic %s out of memory", imgname[i]);
			exit(1);
		}
		while (tmpbuf != 0x1a) fread(&tmpbuf, 1, 1, fp);
		printf("graphic data %s now reading ...\n", imgname[i]);
		fread(imgbuf[i], 1, size, fp);
		fclose(fp);
	}
}

void	DispPic(void)
{
	int		i, paletnum = 1, x = 0, y = 0;
	volatile unsigned short	*src = imgbuf[round_ - 1] + 2, *dst = VRAM2(0, 0);
	
	for (i = 0; i < 32768; ++i) paletcheck[i] = PALETINIT;

	while( y < 512 ) {
		unsigned int	col = *src++, len = *src++;
		
		if ( PALETINIT == paletcheck[col >> 1] ) {
			paletcheck[col >> 1] = paletnum;
			gpalet[paletnum] = col;
			col = paletnum++;
			if (paletnum > 127) --paletnum;
		} else {
			col = paletcheck[col >> 1];
		}
		while(len--) {
			*dst++ = COL2(col);
			if (512 == ++x) {
				x = 0;
				y++;
			}
		}
	}
}

void	DispPic16(int no)
{
	int		i, paletnum = 1, x = 0, y = 0;
	volatile unsigned short	*src = imgbuf[no] + 2, *dst = VRAM1(0, 0);
	
	for (i = 0; i < 32768; ++i) paletcheck[i] = PALETINIT;
	gpalet16[0] = paletcheck[0] = 0;

	while( y < 512 ) {
		unsigned int	col = *src++, len = *src++;
		
		if ( PALETINIT == paletcheck[col >> 1] ) {
			paletcheck[col >> 1] = paletnum;
			gpalet16[paletnum] = col;
			col = paletnum++;
			if (paletnum > 15) --paletnum;
		} else {
			col = paletcheck[col >> 1];
		}
		while(len--) {
			*dst++ = COL1(col);
			if (512 == ++x) {
				x = 0;
				y++;
			}
		}
	}
}

void	DispControl(void)
{
	int	i;
	
	SCROLL1(0,256);
	DispPic16(STAGEMAX + 1);
	for (i = 0; i < 16; ++i) *PAL1(i) = gpalet16[i];
	SCROLL1(0,0);
}

void	ChangeControl(int mode)
{
	SCROLL1(mode * 256, 0);
}

void	InitPalet(void)
{
	int	i;
	
	for (i = 0; i < 8; ++i)		*PAL0(i) = RGB(0, 0, 0);
	for (i = 0; i < 16; ++i)	*PAL1(i) = RGB(0, 0, 0);
	for (i = 0; i < 128; ++i)	*PAL2(i) = RGB(0, 0, 0);
}

short	G3Dto2D(short xy, short z)	/*	-HALF <= z <= HALF	*/
{
	unsigned short	*pos_ptr = (unsigned short *)pos_ + ((HALF - z) / DIV) * (HALF / DIV + 1);
	
	return xy >= 0 ? 256 + *( pos_ptr + xy / DIV ) : 256 - *( pos_ptr - xy / DIV );
}

void	Draw(void)
{
	int	x, y;

	switch (round_) {
		case	2:
			y = G3Dto2D(GROUND, -HALF) - 2;
			for (; y < 512; ++y) {
				for (x = 0; x < 512; ++x) *VRAM2(x, y) = COL2(0);
			}
			
			y = G3Dto2D(GROUND, 0);
			for (; y < 512; ++y) {
				for (x = 0; x < 512; ++x) *VRAM1(x, y) = COL1(1);
			}
			break;
		case	3:
			DispPic16(STAGEMAX);
			break;
		case	4:
			y = G3Dto2D(TENJOU, -HALF) + 2;
			for (; y >= 0; --y) {
				for (x = 0; x < 512; ++x) *VRAM2(x, y) = COL2(0);
			}
			
			y = G3Dto2D(TENJOU, 0);
			for (; y >= 0; --y) {
				for (x = 0; x < 512; ++x) *VRAM1(x, y) = COL1(1);
			}
			break;
		case	6:
			y = G3Dto2D(SEETOP, -HALF) + 1;
			for (; y >= 0; --y) {
				for (x = 0; x < 512; ++x) *VRAM2(x, y) = COL2(0);
			}
			y = G3Dto2D(SEEBOT, -HALF) - 1;
			for (; y < 512; ++y) {
				for (x = 0; x < 512; ++x) *VRAM2(x, y) = COL2(1);
			}
			
			y = G3Dto2D(SEETOP, 0);
			for (; y >= 0; --y) {
				for (x = 0; x < 512; ++x) *VRAM1(x, y) = COL1(1);
			}
			y = G3Dto2D(SEEBOT, 0);
			for (; y < 512; ++y) {
				for (x = 0; x < 512; ++x) *VRAM1(x, y) = COL1(2);
			}
			break;
		case	7:
			x = G3Dto2D(-KABE, -HALF) + 1;
			for (; x >= 0; --x) {
				for (y = 0; y < 512; ++y) *VRAM2(x, y) = COL2(0);
			}
			x = G3Dto2D(KABE, -HALF) - 1;
			for (; x < 512; ++x) {
				for (y = 0; y < 512; ++y) *VRAM2(x, y) = COL2(0);
			}
			
			x = G3Dto2D(-KABE, 0);
			for (; x >= 0; --x) {
				for (y = 0; y < 512; ++y) *VRAM1(x, y) = COL1(1);
			}
			x = G3Dto2D(KABE, 0);
			for (; x < 512; ++x) {
				for (y = 0; y < 512; ++y) *VRAM1(x, y) = COL1(1);
			}
			break;
		default:
			for (y = 0; y < 512; ++y) {
				for (x = 0; x < 512; ++x) {
					*VRAM1(x, y) = COL1(0);
				}
			}
			break;
	}
}

void	InitStagePalet(void)
{
	int	i;
	
	for (i = 1; i < 128; ++i) *PAL2(i) = gpalet[i];
	switch (round_) {
		case	1:
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(9, 12, 15);
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(6, 8, 10);
			break;
		case	2:
			/*	brown	*/
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(0, 0, 0);
			/*	Dark brown	*/
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(0, 0, 0);
			
			/*	ground	*/
			*PAL1(1) = RGB(10, 5, 0);
			*PAL2(0) = RGB(8, 4, 0);
			break;
		case	3:
			for (i = 0; i < 16; ++i) *PAL1(i) = gpalet16[i];
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(15, 12, 9);
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(10, 8, 6);
			break;
		case	4:
			/*	red	*/
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(15, 10, 0);
			/*	dark red	*/
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(12, 8, 0);
			/*	ceiling	*/
			*PAL1(1) = RGB(10, 0, 7);
			*PAL2(0) = RGB(9, 0, 6);
			break;
		case	5:
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(0, 11, 17);
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(0, 11, 17);
			break;
		case	6:
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(4, 9, 12);
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(4, 8, 10);
			*PAL1(1) = RGB(5, 11, 15);
			*PAL2(0) = RGB(4, 10, 14);
			*PAL1(2) = RGB(0, 8, 7);
			*PAL2(1) = RGB(0, 7, 7);
			break;
		case	7:
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(14,14,14);
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(11,11,11);
			*PAL1(1) = RGB(8, 8, 8);
			*PAL2(0) = RGB(7, 7, 7);
			break;
		default:
			/*	blue white	*/
			for (i = 0; i < 16; ++i) *(PAL0(1) + i) = RGB(12, 12, 15);
			/*	Red and White	*/
			for (i = 0; i < 16; ++i) *(PAL0(2) + i) = RGB(15, 12, 12);
			break;
	}
}

void	InitGr (int khz15, int nobackground)
{
	if (0 == nobackground) ReadPic();
	CRTMOD(10 + khz15);
	G_CLR_ON();
}

#define	Z1	HALF
#define	Z12	(HALF + HALF / 4)
#define	Z2	(HALF + HALF)

void	BackGround(void)
{
	switch (round_) {
		case	3:
			SCROLL1(
			me.x >= 0
			? 128 + *((unsigned short *)pos_ + (Z12 / DIV) * (HALF / DIV + 1) + me.x / DIV)
			: 128 - *((unsigned short *)pos_ + (Z12 / DIV) * (HALF / DIV + 1) - me.x / DIV),
			me.y >= 0
			? 128 + *((unsigned short *)pos_ + (Z12 / DIV) * (HALF / DIV + 1) + me.y / DIV)
			: 128 - *((unsigned short *)pos_ + (Z12 / DIV) * (HALF / DIV + 1) - me.y / DIV)
			);
			break;
		case	7:
			SCROLL1(me.x >= 0
			? 128 + *((unsigned short *)pos_ + (Z1 / DIV) * (HALF / DIV + 1) + me.x / DIV)
			: 128 - *((unsigned short *)pos_ + (Z1 / DIV) * (HALF / DIV + 1) - me.x / DIV)
			, 0
			);
			break;
		case	1:
			break;
		default:
			SCROLL1(0,
			me.y >= 0
			? 128 + *((unsigned short *)pos_ + (Z1 / DIV) * (HALF / DIV + 1) + me.y / DIV)
			: 128 - *((unsigned short *)pos_ + (Z1 / DIV) * (HALF / DIV + 1) - me.y / DIV)
			);
			break;
	}
	SCROLL2(
	me.x >= 0
	? 128 + *((unsigned short *)pos_ + (Z2 / DIV) * (HALF / DIV + 1) + me.x / DIV)
	: 128 - *((unsigned short *)pos_ + (Z2 / DIV) * (HALF / DIV + 1) - me.x / DIV),
	me.y >= 0
	? 128 + *((unsigned short *)pos_ + (Z2 / DIV) * (HALF / DIV + 1) + me.y / DIV)
	: 128 - *((unsigned short *)pos_ + (Z2 / DIV) * (HALF / DIV + 1) - me.y / DIV)
	);
}

void	InitStage5(void)
{
	int	i;

	for (i = 1; i < 34; ++i) gpalet[i] = RGB(0, 0, 0);
	SCROLL2(0, 0);
	for (i = 0; i < 8; ++i) {
		gpalet[((i - 4 + 32) & 31) + BASEPAL] = st5col[i];
	}
}

void	InitStageGr(int nobackground)
{
	InitPalet();
	gr_clr();
	if (0 == nobackground) DispPic();
	if (nobackground < 2) Draw();
	if (5 == round_) {
		InitStage5();
	} else {
		BackGround();
	}
	InitStagePalet();
}

void	StarDisp(void)
{
	int		i, col;
	volatile unsigned short	*ptr;
	static int	toggle = 2;
	
	toggle = 2 - toggle;
	
	for (i = 0; i < STAR_NUM; ++i) {
		if (star[i].old_rx < 0 || star[i].old_rx > 255 || star[i].old_ry < 0 || star[i].old_ry > 255) goto pset;
		switch (star[i].old_box) {
			case	2:
				*(ptr = VRAM0(star[i].old_rx, star[i].old_ry)) &= 0xf;
				*(ptr +   1) &= 0xf;
				*(ptr + 512) &= 0xf;
				*(ptr + 513) &= 0xf;
				break;
			case	1:
				*(ptr = VRAM0(star[i].old_rx, star[i].old_ry)) &= 0xf;
				*(ptr + 512) &= 0xf;
				break;
			case	0:
				*VRAM0(star[i].old_rx, star[i].old_ry) &= 0xf;
				break;
		}
pset:
		if (star[i].rx < 0 || star[i].rx > 255 || star[i].ry < 0 || star[i].ry > 255) continue;
		col = COL0( star_color[toggle + (i & 1)] );
		switch (star[i].box) {
			case	2:
				*(ptr = VRAM0(star[i].rx, star[i].ry)) |= col;
				*(ptr +   1) |= col;
				*(ptr + 512) |= col;
				*(ptr + 513) |= col;
				break;
			case	1:
				*(ptr = VRAM0(star[i].rx, star[i].ry)) |= col;
				*(ptr + 512) |= col;
				break;
			case	0:
				*VRAM0(star[i].rx, star[i].ry) |= col;
				break;
		}
	}
}

void	GrStage5(void)
{
	int	i;

	for (i = 0; i < 9; ++i) {
		*PAL2(((round_counter * 2 + i - 4 + 32) & 31) + BASEPAL) = st5col[i];
	}
}

void	Flushing(void)
{
	int	i, j;
	unsigned short	palet[32][256];
	unsigned short	*gptr, *pptr;

	for (i = 0; i < 256; ++i) palet[0][i] = *(GPALET_ + i);
	for (i = 1; i < 32; ++i) {
		for (j = 0; j < 256; ++j) {
			palet[i][j] = RGB(
					31 - (31 - R(palet[0][j])) * (31 - i) / 32,
					31 - (31 - G(palet[0][j])) * (31 - i) / 32,
					31 - (31 - B(palet[0][j])) * (31 - i) / 32
					);
		}
	}
	pptr = (unsigned short *)palet;
	for (i = 0; i < 32; ++i) {
		Vdispwait();
		gptr = (unsigned short *)GPALET_;
		for (j = 0; j < 256; ++j) {
			*gptr++ = *pptr++;
		}
	}
}

void	GrDisp(void)
{
	StarDisp();
	if (5 == round_) {
		GrStage5();
	} else {
		BackGround();
	}
}
