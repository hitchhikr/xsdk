#include	"stdmomo.h"
#include	"battle.h"
#include	"bg.h"
#include	"stick.h"
#include	"textput.h"
#include	"title.h"
#include	"etc.h"
#include	"stage.h"
#include	"display.h"
#include	"battle.h"

/* Guide */
#define	G_XMIN	2
#define	G_XMAX	14
#define	G_YMIN	15
#define	G_YMAX	28
extern int battle[];

static void displog(void)
{
	static char strbuf[256];
	sprintf(strbuf, "NORMAL BATTLE    %3d", battle[0]);
	tx_puts(6, 23, strbuf, BASENUM);
	sprintf(strbuf, "EXPERT BATTLE    %3d", battle[1]);
	tx_puts(6, 24, strbuf, BASENUM);
	sprintf(strbuf, "VS COM BATTLE    %3d", battle[2]);
	tx_puts(6, 25, strbuf, BASENUM);
}

static	void draw_guide( void )
{
	int x, y;
	for (x = G_XMIN; x <= G_XMAX; x++) {
		*BGADR(x, G_YMIN) = 0x644;
		*BGADR(x, G_YMAX) = 0x646;
	}
	for (y = G_YMIN; y <= G_YMAX; y++) {
		*BGADR(G_XMIN, y) = 0x645;
		*BGADR(G_XMAX, y) = 0x647;
	}
	*BGADR(G_XMIN, G_YMIN) = 0x640;
	*BGADR(G_XMIN, G_YMAX) = 0x641;
	*BGADR(G_XMAX, G_YMIN) = 0x642;
	*BGADR(G_XMAX, G_YMAX) = 0x643;
}

#define	X_MOVE	(15*8)
#define	GUIDEV	24
int	select_( void )
{
	int	i, sel = 0, x = 0;
	int	flg[2], endf = 0;
	STICK	s;
	flg[0] = flg[1] = 0;

	bgdisp(1);
	for (i = 32; i >= 0; i-=2){	fade(i);wait();}

	draw_guide();

	while (!endf){
		for (i = 0; i < 2; i++) {
			s = JOY(i);
			if (s.flg.left || s.flg.right){
				if (flg[i] == 0) {
					sel = 1 - sel;
				}
				flg[i] = 1;
			} else	flg[i] = 0;
			if( s.flg.lb || s.flg.rb ) endf = 1;
		}
		if( BITSNS( 0 ) & 2 ) endbattle();
		if( x < sel * X_MOVE ) x += GUIDEV;
		if( x > sel * X_MOVE ) x -= GUIDEV;
		wait();
		BGSCRLST(0x80000001, (-x) & 511, 0);
	}
	for( i = 0; i <= 32; i++ ){
		fade(i);wait();
		BGSCRLST(0x80000001, 0, 0);
	}

	return sel;
}

int	title( void )
{
	STICK s0, s1;
	int	i;

	commode[0] = commode[1] = MODE_CPU;
	bgdisp(0);
	sprite_clear();
/*	tx_puts_big( 0, 6, "NKF SUPER BATTLE", BIG_BASENUM );	*/
	tx_puts( 1, 20, "1994 KMC   ALL RIGHTS RESERVED", BASENUM);
	tx_puts(11, 27, "HIT TRIGGER", BASENUM );
	for( i = 32; i >= 0; i-- ){	fade(i);wait();}
	while (-1) {
		s0 = JOY(0);
		s1 = JOY(1);
		if (s0.flg.up && s0.flg.left && s1.flg.down && s1.flg.right && s0.flg.rb) {
			displog();
			continue;
		}
		if (s0.flg.lb || s0.flg.rb) {
			commode[0] = MODE_HUMAN;
			break;
		}
		if (s1.flg.lb || s1.flg.rb) {
			commode[1] = MODE_HUMAN;
			break;
		}
		if( BITSNS( 0 ) & 2 ) endbattle();
		wait();
		disp_push_start();
	}
	tx_puts(11, 27, "           ", BASENUM );
	tx_puts( 0, 20, "                                ", BASENUM );
	for( i = 0; i <= 32; i+=2 ){	fade(i);wait();}

	return 1;
}

void	ending(void)
{
	int	i, j;
	int	exitflg = 0;
	STICK	s;

	bgdisp(10);
	sprite_clear();
	tx_puts(0,  8, "  YOU HAVE CLEARED ALL STAGES.  ", BASENUM);
	tx_puts(0, 10, "                                ", BASENUM);
	tx_puts(0, 14, "             STAFF              ", BASENUM);
	tx_puts(0, 16, " GAME DESIGNER       N.KAWAHARA ", BASENUM);
	tx_puts(0, 18, " PROGRAMMER           C.YOSHIDA ", BASENUM);
	tx_puts(0, 20, " MUSIC PRODUCER           T.OKA ", BASENUM);
	tx_puts(0, 22, " SOUND DESIGNER      N.KAWAHARA ", BASENUM);
	tx_puts(0, 24, "                                ", BASENUM);
	tx_puts(0, 26, "1994  KMC   ALL RIGHTS RESERVED.", BASENUM);

	for( i = 32; i >= 0; i-- ){	fade(i);wait();}
	for (i = 0; i < 600; i++) {
		wait();
		for (j = 0; j < 2; j++) {
			s = JOY(j);
			if ((s.flg.lb || s.flg.rb) && commode[j] == MODE_HUMAN){
				exitflg = 1;
			}
		}
		if (exitflg) break;
	}
	for (i = 0; i <= 32; i++){
		fade(i);
		if (i > 1 && i < 30) {
			tx_puts(0, i, "                                ", BASENUM);
		}
		wait();
	}
}
