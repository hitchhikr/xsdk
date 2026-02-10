/*
 *		ending.c (for DesperadO.x)
 *
 *				Programmed by Momo 1992
 *
 */


#include	<stdio.h>
#include	<string.h>
#include	<basic.h>
#include	<iocslib.h>
#include	<basic0.h>
#include	<graph.h>
#include	"sound.h"
#include	"title.h"
#include	"sps.h"
#include	"disp2.h"

static	int	endy   = 600;			//	Coordinates to draw
static	int	endcnt1 =  0;			//	The number of the string to draw
static	int	endcnt2 =  0;			//	How many lines in a row are drawn?
static	int	endsy  =  0;			//	For scrolling
extern	int	dismode;			//	0 .. 31kHz   1 ... 15Khz

#define		STRNUM	256
#define		SKIP	-1
#define		END	-2

static	short	str_x[STRNUM], ftype[STRNUM], scrl[STRNUM];
static	char	*endstr[STRNUM] = {
	"2Cデスペラード", "", "",
	"0Cｆｏｒ　Ｘ６８０ｘ０",
	"","",
	"","",
	"",
	"2C原作",
	"",
	"2C伝説阪神",
	"0C（ＫＡＢＯＳＵアクションゲーム「ＤＥＳＰＥＲＡＤＯ」より）",
	"","",
	"","",
	"","",
	"2C設定",
	"",
	"2CＩ．ＭＡＩＳＯＶ",
	"0C（ＫＡＢＯＳＵ　Ｐｕｂｌｉｓｈｅｒ刊　　　",
	"0C　　　　　　「それゆけデスペラード」より）",
	"","",
	"","",
	"","",

	"2Cスタッフ","",
	"2Cゲーム化企画　　　　　　　　荻野　友隆","",
	"2Cプログラム　　　　　　　　　橋本　　寛",
	"2C　　　　　　　　　　　　　　吉田　　力","",
	"2Cモデリング　　　　　　　　　澤田　高志",
	"2C　　　　　　　　　　　　　　大西　隆史",
	"2C　　　　　　　　　　　　　　大矢　　正",
	"2C　　　　　　　　　　　　　　荻野　友隆","",
	"2C３Ｄ作画　　　　　　　　　　荻野　友隆",
	"2C　　　　　　　　　　　　　　吉田　　力","",
	"2C２Ｄ作画　　　　　　　　　　川原　直也","",
	"2C音楽　　　　　　　　　　　　成河　　清",
	"0R（サントラ版ＫＡＢＯＳＵ　ｐｒｏｄｕｃｔｓ）","",
	"2C音響　　　　　　　　　　　　川原　直也","",
	"","","","","","","","",
	"2C総監督","","2C荻野　友隆","","","","","","",
	"2C制作総指揮","","2C青木　学聡","","","","","","","","","","","","",
	"2C制作","","2C京大マイコンクラブ","","",
	"0R（Ｃ）１９９２，１９９３　　　　　","","","","","","","","",

	"!"
};

void	ending(void)
{
	int	endflg = 0;
	int	i, j, x = 0;
	int	writeflg = 0;
	char	*endstrptr = 0;
	int	fonttype = 0;
	int	vx = 0;
	char	*endstrbuf = "  ";
	volatile int	*addr = (volatile int *)0xe00000;
	volatile int	*gptr = (volatile int *)0xc00000;

	endy   = 600;
	endcnt1 =  0;
	endcnt2 =  0;
	endsy  =  0;


	while(endflg == 0) {
		i = endstr[endcnt1][0];
		j = endstr[endcnt1][1];
		writeflg = 0;
		switch(i) {
		  case '\0' :
			scrl[endcnt1] = 31;
			str_x[endcnt1] = SKIP;
			break;
		  case '!' :
			str_x[endcnt1] = END;
			endflg = 1;
			break;
		  case '2' :
			writeflg = 1;
			x = (strlen(endstr[endcnt1]) - 2) * 6;
			scrl[endcnt1] = 31;
			ftype[endcnt1] = 2;
			break;
		  case '0' :
			writeflg = 1;
			x = (strlen(endstr[endcnt1]) - 2) * 3;
			scrl[endcnt1] = 15;
			ftype[endcnt1] = 0;
			break;
		}

		if(writeflg) {
			switch(j) {
			  case 'C' :
				str_x[endcnt1] = 256 - x;
				break;
			  case 'R' :
				str_x[endcnt1] = 512 - x * 2;
				break;
			  case 'L' :
				str_x[endcnt1] = 0;
				break;
			}
		}
		endcnt1++;
	}
	endflg = 0;
	writeflg = 0;
	endcnt1 = 0;

	for(; addr < (volatile int *)0xe80000; addr++) {
		*addr = 0;
	}

	console(0, 31, 0);
	if(dismode) {
		CRTMOD(1);
	} else {
		CRTMOD(0);
	}
	G_CLR_ON();
	window(0, 0, 1023, 1023);

	endy= 600;
	endcnt1 = 0;
	endcnt2 = 0;
	endsy = 0;

	while(endflg == 0) {
		endsy += 1;
		vdispwait();
		home(0, 0, endsy & 1023);
		gptr = (volatile int *)(0xc00000 + ((endsy + 1023) & 1023) * 2048);
		for(i = 0; i < 128; i++) {
			*(gptr++) = 0;	*(gptr++) = 0;
			*(gptr++) = 0;	*(gptr++) = 0;
		}
		vdispwait();
		if (endcnt2) {
			for(i = 0; i < (fonttype == 0 ? 2 : 1); i++) {
				if (writeflg) {
					if (*endstrptr != '\0' && *(endstrptr + 1) != '\0') {
						endstrbuf[0] = endstrptr[0];
						endstrbuf[1] = endstrptr[1];
						endstrptr += 2;
						symbol(x, endy, endstrbuf, 1, 1, fonttype, 15, 0);
						x += vx;
					} else {
						writeflg = 0;
					}
				}
			}
			endcnt2--;
		} else {
			endy = (endsy + 575) & 1023;
			writeflg = 0;
			switch(str_x[endcnt1]) {
			  case END:
				endflg = 1;
				break;
			  case SKIP:
				endcnt2= 31;
				break;
			  default:
				x = str_x[endcnt1];
				endstrptr = endstr[endcnt1] + 2;
				endcnt2 = scrl[endcnt1];
				fonttype = ftype[endcnt1];
				vx = (fonttype == 0 ? 12 : 24);
				writeflg = 1;
				break;
			}
			endcnt1++;
		}
	}
	bgm_fade(24);
	for(i = 0 ; i < 240; i++) 		vdispwait();
	screen(0, 2, 1, 1);
	ScrModeSet();
	SpsInit();
	spaletfade(32, 0);
	dispinit0();

}
