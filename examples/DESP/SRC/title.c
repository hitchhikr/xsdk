/*
 *		Title.c(for DesperadO)
 *
 */

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<graph.h>
#include	"disp2.h"
#include	"txput.h"
#include	"sound.h"
#include	"name.h"
#include	"sps.h"
#include	"txput.h"
#include	"dsp.h"
#include	"graph.h"

extern	short	GScroll[];
extern	int	startstage;
extern	int	invincible;
extern	int	difficulty;
extern	char	*PicLoadPath;
extern	short	RASTER1;
extern	short	RASTER2;

#define	RIGHT			8
#define	LEFT			4
#define	UP			1
#define	DOWN			2
#define	RASTER1_NZ		(130*2+76)
#define	RASTER2_NZ		(228*2+76)

const static int config_y[6] = { 6, 8, 10, 12, 14, 18 };
static char *dif_mes[4] = { "   EASY", " NORMAL", "   HARD", "   GURO" };
static char *mes_dismode[4] = { "    NORMAL", "15KHZ MODE", " SLOW MODE", "DEBUG MODE" };

#define	CRTC(n)		*(volatile unsigned short *)(0xe80000+2*n)
#define	HTOTAL		*(volatile unsigned short *)0xeb080a
#define	HDISP		*(volatile unsigned short *)0xeb080c
#define	VDISP		*(volatile unsigned short *)0xeb080e
short	JOY(int);

int dismode = 0;


void	ScrModeSet(void)
{
	CRTC(8) = 0;	/* TX y-axis scroll */

	switch(dismode){
	  case 1:
		ScrInit(0x10B);
/*
		CRTC(4) =204;
		CRTC(7) =200;
*/
		CRTC(0) =36;
		CRTC(1) = 3;
		CRTC(2) = 0;
		CRTC(3) =32;
		CRTC(4) =275;
		CRTC(5) = 0;
		CRTC(6) = VDISP = 2;
		CRTC(7) =265;
		RASTER1 = 110;
		RASTER2 = 220;
		break;
	  case 3:
		ScrInit(0x108);
		break;
	  case 0:
		ScrInit(0x10A);
		break;
	  default:
		ScrInit(0x10A);
		CRTC(4) =721;
		CRTC(5) =71;
		CRTC(6) = VDISP =116;
		CRTC(7) =628;
		RASTER1 = RASTER1_NZ;
		RASTER2 = RASTER2_NZ;
		break;
	}
}

void	config(void)
{
	int	i, x, vx;
	int	y = 0, exflug = 0, bgno = 0, sdno = 0;
	int	memsize;

	memsize = (int) MALLOC(0xffffff) - 0x81000000;
	gpaletfade(16);
	tx_clr2();

	while (BITSNS(0) & 2);

	tx_puts(2,  6, "  STAGE SELECT     ", 14);
	tx_puts(2,  8, "  BGM              ", 14);
	tx_puts(2, 10, "  SOUND            ", 14);
	tx_puts(2, 12, "  DISPLAY MODE     ", 14);
	tx_puts(2, 14, "  DIFFICULTY       ", 14);
	tx_puts(2, 18, "  EXIT             ", 14);
	tx_puts(6, 22, "FREE AREA          BYTES", 14);
	tx_putn2(16, 22, memsize, 8, 14);

	while ((BITSNS(0) & 2) == 0 && exflug == 0) {
		int	vy = y;

		tx_puts(2, config_y[y], ">", 14);

		tx_putn2(28,  6, startstage, 2, 14);
		tx_putn2(28,  8,       bgno, 2, 14);
		tx_putn2(28, 10,       sdno, 2, 14);
		tx_puts(23, 14, dif_mes[difficulty], 14);
		tx_puts(20, 12, mes_dismode[dismode], 14);

		i = JOY(0) & 255;
		vx = 0;

		if (! (i & UP)) y += 5;
		else if (! (i & DOWN)) y++;
		y %= 6;

		if (!(i & RIGHT)) vx = 1;
		else if (! (i & LEFT)) vx = -1;

		if (vx && y == 0) startstage = (startstage + 8 + vx) % 9 + 1;
		if (vx && y == 4) difficulty = (difficulty + 4 + vx) % 4;
		if (vx && y == 1) bgno = (bgno + 10 + vx) % 10;
		if (vx && y == 2) sdno = (sdno + 10 + vx) % 10;
		if (vx && y == 3) {
			dismode = (dismode + vx + 2) % 2;
			ScrModeSet();
		}
		if ((i & 96) != 96) {
			if (y == 1) bgm(bgno);
			if (y == 2) sd_on(sdno);
			if (y == 5) exflug = 1;
		}

		while(JOY(0) != 255);			/*  WAIT  */
		if (BITSNS(2) & 2) {
			gpaletfade(32);
			dspexit();			/*  End  */
		}

		if (vy != y) tx_puts(2, config_y[vy], " ", 14);
		vdispwait();

	}

	for(x = 0; x < 32; x++) {
		for(y = 0; y < 26; y++) {
			tx_putc(x, y, 0, 14);
		}
	}

	gpaletfade(32);

}

int	title(void)
{
	char	*filename = "TITLE.DP";
	char	strbuf[64];

	short	count_ = 1;
	int	i;

	ScrModeSet();
	gpaletoff();

	for(i = 0; i < 128; i ++) {
		*((volatile unsigned short *)0xeb0000 + i * 4) = 0;
	}
	i = 0;

	GScroll[1] = GScroll[3] = GScroll[5] = GScroll[7] = 0;
	GScroll[0] = GScroll[2] = 256;
	GScroll[4] = GScroll[6] = 0;
	home(0, 256, 0);
	home(1, 0, 0);

	sprintf(strbuf , "%s\\%s", PicLoadPath, filename);
	dp_load(strbuf);

	calc_fade();

	tx_clr2();
	namedisp();

	while((JOY(0) & 96) == 96) {

		if (BITSNS(0) & 2) config();

		i++;
		vdispwait();
		if (i <= 32) gpaletfade(i);
		if (i > 600 && i <= 632) {
			GScroll[0] = GScroll[2] = (i - 568) * 8;
			home(0, (i - 568) * 8, 0);
		}
		if (count_ == 600) {
			tx_clr2();
			sd_on(22);
		}
		if (count_++ > 1200) {
			count_ = 0;
			break;
		}
	}
	tx_clr2();
	return count_;
}
