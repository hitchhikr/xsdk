/*
 *		display.c	(for DesperadO.X)
 *				Programmed by ìç
 */

#include	<stdio.h>
#include	<graph.h>
#include	<sprite.h>
#include	<basic0.h>

#include	"txput.h"
#include	"display.h"

#define		SP_METER_X		23	// Speedometer center coordinates
#define		SP_METER_Y		26

#define		DIV_SPEEDMETER		12
#define		SPEEDMETER_MAX		7

#define		DIV_ENERGY		128

const	unsigned short	liner[16] = {
		0x7fff, 0x3fff, 0x1fff, 0x0fff,
		0x07ff, 0x03ff, 0x01ff, 0x00ff,
		0x007f, 0x003f, 0x001f, 0x000f,
		0x0007, 0x0003, 0x0001, 0x0000 };
const	unsigned short	linel[16] = {
		0x0000, 0x8000, 0xc000, 0xe000,
		0xf000, 0xf800, 0xfc00, 0xfe00,
		0xff00, 0xff80, 0xffc0, 0xffe0,
		0xfff0, 0xfff8, 0xfffc, 0xfffe };

const	unsigned short	textmask[16] = {
		0x7fff, 0xbfff, 0xdfff, 0xefff,
		0xf7ff, 0xfbff, 0xfdff, 0xfeff,
		0xff7f, 0xffbf, 0xffdf, 0xffef,
		0xfff7, 0xfffb, 0xfffd, 0xfffe };


extern	short	enemy_left;				//	The number of remaining enemies.
extern	int	count_;					//	Time from the start of the game
extern	int	bester_energy;				//	Bester's Energy

static	int	warn_count;				//	Counter for warning display.

/*
 *		Text box fill for radar
 */

static	void	boxfill(short x1, short y1, short x2, short y2, short c)
{
	short	i;
	unsigned short	a;
	unsigned short	x01, x02, x11, x12;
	volatile unsigned short	*addr1, *addr2;

	x01 = x1 / 16;
	x02 = x2 / 16;
	x11 = x1 & 15;
	x12 = x2 & 15;

	if (c) {
		a = 0xffff;
		*(volatile unsigned short *) 0xe8002a = 3 * 16 + 768;
	} else {
		a = 0x0000;
		*(volatile unsigned short *) 0xe8002a = 14 * 16 + 768;
	}

	addr1 = (volatile unsigned short *)0xe00000 + y1 * 0x40 + x01;
	addr2 = (volatile unsigned short *)0xe00000 + y1 * 0x40 + x02;

	if (x01 == x02) {
		*(volatile unsigned short *)0xe8002e = (linel[x11] | liner[x12]);
		for (i = y1; i < y2; i ++) {
			*(addr1 += 0x40) = a;
		}
	} else {
		*(volatile unsigned short *)0xe8002e = linel[x11];
		for (i = y1; i < y2; i ++) {
			*(addr1 += 0x40) = a;
		}

		*(volatile unsigned short *)0xe8002e = liner[x12];
		for (i = y1; i < y2; i ++) {
			*(addr2 += 0x40) = a;
		}
	}
}

/*		Write the display field at the bottom of the screen		*/

void	tx_dispinit(int DesperadO, int score)
{
	short	x, y;

	*(volatile unsigned char *)0xe82500 = 0x12;

	for(x = 0; x < 32; x++) {
		for(y = 0; y < 26; y++) {
			tx_putc(x, y, 80, 15);
		}
	}
//                  01234567890123456789012345678901
    tx_puts(0, 26, "accccccccccccccueeeeeeefeeeeeeeq", 15);
//	tx_puts(0, 31, "boooooooooooooooooooooooooooooor", 15);
    tx_puts(0, 27, "t SCORE      0                 s", 15);
	tx_puts(0, 28, "t ENEMY     32                 s", 15);
	tx_puts(0, 29, "t DESPERADO  4                 s", 15);
	tx_puts(0, 30, "wyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyx", 15);

	tx_putn(8, 27, score, 5, 15);
	tx_putn(12, 29, DesperadO, 2, 15);
	tx_putn(12, 28, enemy_left, 2, 15);

	energy_disp1(bester_energy);

	for(x = -64; x <= 64; x++){
		for(y = 0; y < 18; y++){
			radar_set(x * DIV_RADAR_X, y * DIV_RADAR_Y, 3);
		}
	}
	warn_count = 0;

}

void	tx_dispinit1(int DesperadO, int score)
{
	short	x, y;

	*(volatile unsigned char *)0xe82500 = 0x12;

	for(x = 0; x < 32; x++) {
		for(y = 0; y < 26; y++) {
			tx_putc(x, y, 80, 15);
		}
	}
	tx_puts(0, 26, "accccccccccccccueeeeeeefeeeeeeeq", 15);
	tx_putc(13, 27, '0'-32, 15);
	tx_putn(8, 27, score, 5, 15);
	tx_putn(12, 29, DesperadO, 2, 15);
	tx_putn(12, 28, enemy_left, 2, 15);

	energy_disp1(bester_energy);

	for(x = -64; x <= 64; x++){
		for(y = 0; y < 18; y++){
			radar_reset(x * DIV_RADAR_X, y * DIV_RADAR_Y, 15);
			radar_set(x * DIV_RADAR_X, y * DIV_RADAR_Y, 3);
		}
	}
	warn_count = 0;

}


/*
 *		Draws a speedometer. However, this must be called after B_SUPER.
 */


void	speedmeter(short speed)
{
	short	c;
	c = speed / DIV_SPEEDMETER + SP_METER_X;

	if (c == SP_METER_X - SPEEDMETER_MAX || c == SP_METER_X + SPEEDMETER_MAX) {
		tx_putc(c, SP_METER_Y, 'f' - 32, 15);
		return;
	}

	if (speed == 0) {
		tx_putc(c - 1, SP_METER_Y, 'e' - 32, 15);
		tx_putc(c + 1, SP_METER_Y, 'e' - 32, 15);
		tx_putc(c, SP_METER_Y, 'e' - 32, 15);
		return;
	}
	
	if (c == SP_METER_X) {
		tx_putc(c - 1, SP_METER_Y, 'e' - 32, 15);
		tx_putc(c, SP_METER_Y, 'f' - 32, 15);
		tx_putc(c + 1, SP_METER_Y, 'e' - 32, 15);
		return;
	}

	if (c < SP_METER_X) {
		tx_putc(c - 1, SP_METER_Y, 'e' - 32, 15);
		tx_putc(c, SP_METER_Y, 'f' - 32, 15);
	} else {
		tx_putc(c + 1, SP_METER_Y, 'e' - 32, 15);
		tx_putc(c, SP_METER_Y, 'f' - 32, 15);
	}
}


/*
 *		Call after setting the radar point B_SUPER.
 */

#if 0
void	radar_set(short x, short y, short c)
{
	short	sx, sy;

	sx = RADAR_X + x / DIV_RADAR_X;
	sy = RADAR_Y + y / DIV_RADAR_Y;

	*(volatile unsigned short *)MASKREG = textmask[sx & 15];
	*(volatile unsigned short *)R21 = 0x300 + c * 16;

	*((volatile unsigned short *)0xe00000 + sy * 64 + sx / 16) = 0xffff;

	*(volatile unsigned short *)R21 = 0;

}

void	radar_reset(short x, short y, short c)
{
	short	sx, sy;

	sx = RADAR_X + x / DIV_RADAR_X;
	sy = RADAR_Y + y / DIV_RADAR_Y;

	*(volatile unsigned short *)MASKREG = textmask[sx & 15];
	*(volatile unsigned short *)R21 = 0x300 + 12 * 16;
	*((volatile unsigned short *)0xe00000 + sx / 16 + sy * 64) = 0;

//	*(unsigned short *)R21 = 0x300 + 3 * 16;
//	*((unsigned short *)0xe00000 + sx / 16 + sy * 64) = 0xffff;

	*(volatile unsigned short *)R21 = 0;

}
#endif

void	radar_scope(short scx, short scy, short scx0, short scy0)
{
	short	sx, sy, ex, ey;
						//	Erase
	sx = RADAR_X + scx0 / DIV_RADAR_X + 1;
	sy = RADAR_Y + scy0 / DIV_RADAR_Y;

	ex = sx + 14;
	ey = sy + 11;

	if (sx< RADAR_SX) sx = RADAR_SX;
	if (ex> RADAR_EX) {
		boxfill(RADAR_SX, sy, ex - 128, ey, 1);
		ex = RADAR_EX;
	}

	boxfill(sx, sy, ex, ey, 1);

						//	Write
	sx = RADAR_X + scx / DIV_RADAR_X + 1;
	sy = RADAR_Y + scy / DIV_RADAR_Y;

	ex = sx + 14;
	ey = sy + 11;

	if (sx< RADAR_SX) sx = RADAR_SX;
	if (ex> RADAR_EX) {
		boxfill(RADAR_SX, sy, ex - 128, ey, 0);
		ex = RADAR_EX;
	}

	boxfill(sx, sy, ex, ey, 0);
}


void	scoredisp(int score)
{
	tx_putn(8, 27, score, 5, 15);
}

/***********************************************************
 *		Energy gauge display!
  ***********************************************************/

void	energy_disp2(int ene)
{
	short	c, m;
	c = ene / DIV_ENERGY / 8;
	m = ene / DIV_ENERGY % 8;
	if (c > 29) return;
	if (c < 28) {
		tx_putc(c + 2, 31, 'd' - 32, 15);
	}
	if (m == 0) {
		tx_putc(c + 1, 31, 'd' - 32, 15);
		return;
	}
	tx_putc(c + 1, 31, 'o' - 32 - 7 + m, 15);
}

void	energy_disp1(int ene)
{
	short	c, i;
	c = ene / DIV_ENERGY;

	tx_puts(0, 31,"bddddddddddddddddddddddddddddddr", 15);
	for(i = 0; i < c / 8 && i < 30; i++) {
		tx_putc(i + 1, 31, 'o' - 32, 15);
	}
	energy_disp2(ene);
}

void	warn_display()
{
	warn_count = 64;
	tx_puts(12, 7, "WARNING!!", 14);
	tx_puts(2, 11, "'TARAI' IS FALLING TO BESTER.", 14);
}

void	warn(int flg)
{
	if (warn_count) {
		if (flg == 0) warn_count--; else warn_count = 0;
		if (warn_count == 0) {
			tx_puts(12, 7, "         ", 14);
			tx_puts(2, 11, "                             ", 14);
		}
	}
}

void	tarai_dir_display(int flg)
{
	static	char *tarai_l[4] = {
		"~}]^_","\x7f}]^_","}~]^_","}\x7f]^_"
	};
	static	char *tarai_r[4] = {
		"]^_z{","]^_z|","]^_{z","]^_|z"
	};

	if (flg & 1)	tx_puts(0, 25, tarai_l[count_ / 4 & 3], 15);
		else	tx_puts(0, 25, "ppppp", 15);
	if (flg & 2)	tx_puts(27, 25, tarai_r[count_ / 4 & 3], 15);
		else	tx_puts(27, 25, "ppppp", 15);
}
