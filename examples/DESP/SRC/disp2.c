/*
 *		DISP2.c(for DesperadO.X)
 *			Sprite Color & Graphic Routine
 */

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<graph.h>
#include	<sprite.h>
#include	"txput.h"
#include	"disp2.h"
#include	"display.h"
#include	"sound.h"
#include	"dsp.h"
#include	"graph.h"

#define		PALSIZE		32768

#define		RGB_R(c)	(((c) / 64) & 31)
#define		RGB_G(c)	(((c) / 2048) & 31)
#define		RGB_B(c)	(((c) / 2) & 31)

#define		RGB_(r, g, b)	((r) * 64 + (b)* 2 + (g) * 2048)

unsigned short	gpalet[32][256];		//Contains the palette of the graphic.
unsigned short	spalet[32][256];		//Create a sprite palette.

extern	char	fontdata[];
extern	char	*PicLoadPath;

unsigned short	wingpalet[8];	// Jet spray palette.

/*
 *	Sprite Palette Loading
 */

void	get_spalet(void)
{
	static	int	flg = 0;
	int	i, j;
	FILE	*fp;

	if (flg == 0) {
		flg = 1;
		fp = fopen("dsp.pal", "rb");
		if (fp != NULL) {
			fread(spalet[31] + 16, sizeof(unsigned short), 240, fp);
            fclose(fp);
		}
	}

	for (i = 0; i < 16; i++) {
		spalet[31][i] = *(unsigned short *)(fontdata + i * 2);
	}

	// Calculate FADE DATA of SPRITE PALETTE

	for(j = 0; j < 31; j ++) {
		for(i = 0; i < 256; i++) {
			spalet[j][i] = RGB_(
				RGB_R(spalet[31][i]) * (j + 1) / 32,
				RGB_G(spalet[31][i]) * (j + 1) / 32,
				RGB_B(spalet[31][i]) * (j + 1) / 32
				);
		}
	}
}

//	Calculate FADE DATA of GRAPHIC PALETTE

void	calc_fade(void)
{
	short i, j, r, g, b;
	unsigned short c;
	

	for(i = 0; i < 256; i++) {
		c = gpalet[31][i];
		r = RGB_R(c);
		g = RGB_G(c);
		b = RGB_B(c);
		for(j = 0; j < 31; j ++) {
			gpalet[j][i] = RGB_(
				(short)(r * (j + 1)) / 32,
				(short)(g * (j + 1)) / 32,
				(short)(b * (j + 1)) / 32 
				);
		}
	}
}

/*
 *	SPRITE PALETTE FADEIN/OUT ROUTINE
 */

void	spaletfade(int fade, int mode)
{
	int		i = 0;
	unsigned short	c;
	volatile unsigned short	*addr = (volatile unsigned short *)0xe82200;

	if (mode) {
		i = 16;
		addr += 16;
	}

	if (fade >= 32) fade = 32;

	if (fade <= 0) {
		for(; i < 256; i ++)
			*(addr ++) = 0;
		wingpalet[0] = wingpalet[1] = 0;
	} else {
		for(; i < 256; i++) {
			*(addr ++) = spalet[fade - 1][i];
		}
		wingpalet[0] = spalet[fade - 1][25];
		wingpalet[1] = spalet[fade - 1][26];
	}
}

void	gpaletoff(void)
{
	short	i, j = 31;
	volatile unsigned short	*addr = (volatile unsigned short *)0xe82000;

//	for(j = 0; j < 31; j++) {
//		for (i = 0; i < 256; i++) {
//			gpalet[j][i] = 0;
//			palet(i, 0);
//		}
//	}

	for (i = 0; i < 256; i++) {
		*(addr++) = 0;
	}

} 

void	gpaleton(void)
{
	short		i;
	volatile unsigned short	*addr = (volatile unsigned short *)0xe82000;
	for (i = 0; i < 256; i++) {
		*(addr++) = gpalet[31][i];
	}
}

void	gpaletfade(int fade)
{
	short	i = 0;
	volatile unsigned short	*addr = (volatile unsigned short *)0xe82000;

	VPAGE(15);
	if (fade >= 32) {
		gpaleton();
	} else if (fade <= 0) {
		for(; i < 256; i ++)
			*(addr ++) = 0;
	} else {
		for(; i < 256; i++) {
				*(addr ++) = gpalet[fade - 1][i];
		}
	}
}

/*
 *		Screen initialization (first)
 */

void	dispinit0(void)
{

	get_spalet();

	sp_on(0, 127);
	sp_disp(1);
	B_CUROFF();

	tx_dispinit(-1,-1);

	window(0, 0, 511, 511);


}

/*
 *		Screen initialization (when starting a screen)
 */

static	char	*back_fname[12] = {
	"BACK01.PRL", "BACK02.PRL", "BACK03.PRL", "BACK04.PRL",
	"BACK01.PRL", "BACK02.PRL", "BACK03.PRL", "BACK04.PRL",
	"BACK05.PRL", "BACK05.PRL", "BACK03.PRL", "BACK04.PRL" };

static	int	bgm_no[12] = { 1, 2, 3, 8, 1, 2, 3, 8, 6 };

void	dispinit1(int stage, int DesperadO, int score)
{
	char	strbuf[64];
	char	strbuf2[64];
	int		i;
	long		dummy;
	FILE	*fp;

	gpaletoff();
	spaletfade(0, 1);
	tx_dispinit1(DesperadO, score);
	bgm(bgm_no[stage - 1]);

	sprintf(strbuf, "%s\\%s", PicLoadPath, back_fname[stage - 1]);
	fp = fopen(strbuf, "rb");
	if (fp == NULL) dsperror("Can't read file in function dispinit1");

	fread(gpalet[31], 2, 256, fp);
	fread(&dummy, 4, 1, fp);
	decode(fp, (volatile void *)0xc00000);

	tx_puts(12, 10, "STAGE", 14);
	tx_putn(18, 10, stage, 2, 14);

	for(i = 0; i < 4; i++) {
		fread(&dummy, 4, 1, fp);
		decode(fp, (volatile void *)(0xe08000 + 0x20000 * i));
	}
	fclose(fp);
	
	for(i = -128; i < 288; i += 2) {
		draw_to_graphic(i + 1024, i + 1024);
	}
	calc_fade();

}

#define	MASKPAL		0xe738
/* 0b1110 0111 0011 1000 */

/*	The screen lights up */
void	fadewhite(int white)
{
	short		i = 0;
	unsigned short	c;
	volatile unsigned short	*addr = (volatile unsigned short *)0xe82200 + 16;
	unsigned short	*addr2 = spalet[31] + 16;
	unsigned short	w;
	volatile unsigned short	*addr3 = (volatile unsigned short *)0xe82000;
	unsigned short	*addr4 = gpalet[31];

	if (white < 0) return;
	if (white > 127) white = 127;
	white = white / 4;
	w = RGB_(white, white, white);

	if (white > 10) {
		for(; i < 127; i++) {		// Sprite
			*(addr++) = 1;
//			c = *(addr2++);
//			if (c > 1) *(addr++) = 1; else *(addr++) = w;
		}
		for (i = 0; i < 128; i++) {		// Graphics (Space)
			*(addr3++) = w;
			*(addr3++) = w;
		}
	} else {
		for(; i < 127; i++) {		// Sprite
			*(addr++) = *(addr2++);
//			c = *(addr2++);
//			if (c > 1) *(addr++) = c; else *(addr++) = w;
		}
		for (i = 0; i < 128; i++) {		// Graphic
			c = *(addr4++);
			if (c & MASKPAL) *(addr3++) = c; else *(addr3++) = w;
			c = *(addr4++);
			if (c & MASKPAL) *(addr3++) = c; else *(addr3++) = w;
		}
	}
}
