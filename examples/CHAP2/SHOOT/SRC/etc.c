#include <stdio.h>
#include <iocslib.h>
#include <doslib.h>

#include "sub.h"
#include "ship.h"
#include "enemy.h"
#include "graphic.h"
#include "bg.h"
#include "sound.h"
#include "etc.h"

extern SHIP ship_data;
extern int score;

/*
 *	Stage start processing
 */

void stage_start( int stage )
{
	int i, add = DOT_RATE * 5;

	bgm( 0 );	/* Play air combat BGM */

	bgputs(0, 12, 12, 2, "STAGE  ");
	bgputn(0, 18, 12, 2, stage + 1, 2);

	/* Wait a moment */

	for (i = 0; i < 64; i++) {
		wait();
		graphic_move();
		score_disp( score, ship_data.left );
	}

	/* Process when your ship appears */

	ship_data.data.x = 0;
	for (i = 0; i < 64; i++) {
		if ( (add -= 2) < 0) add = 0;
		ship_data.data.x += add;
		ship_data.sp->x = SP_X( ship_data.data.x );
		ship_data.sp->y = SP_Y( ship_data.data.y );
		ship_data.sp->code = 0x144;
		ship_data.sp->prw = 1;

		wait();
		graphic_move();
		score_disp( score, ship_data.left );
		trans_sprite_data();
	}
	bgputs(0, 12, 12, 2, "         ");
}

/*
 *	Processing when a stage is cleared
 */

void stage_end( int stage )
{
	int i, add = DOT_RATE ;

	bgm( -1 );

	bgputs(0, 9, 12, 2, "STAGE    CLEAR");
	bgputn(0, 15, 12, 2, stage + 1, 1);

	/* The process of your ship flying away */

	for (i = 0; i < 120; i++) {
		ship_data.data.x += add++;
/*		if (ship_data.sp->x < X_MIN) ship_data.sp->x = X_MIN;	*/
		if (ship_data.sp->x > X_MAX) {
			ship_data.sp->prw = 0;
		} else {
			ship_data.sp->prw = 1;
		}
		ship_data.sp->x = SP_X( ship_data.data.x );
		ship_data.sp->y = SP_Y( ship_data.data.y );
		ship_data.sp->code = 0x144;

		shot_move();
		wait();
		graphic_move();
		score_disp( score, ship_data.left );
		trans_sprite_data();
	}
	bgputs(0, 9, 12, 2, "              ");
}

/*
 *	Title screen
 */

void	title( void )
{
	static	char	*title_filename = "title.dat";
	FILE	*fp;
	unsigned char *titlebuf;
	int	xsize, ysize, bufsize;
	struct	PUTPTR pp;

	if (!(fp = fopen(title_filename, "rb"))) {
		ende();
	}

	fread(&xsize, sizeof xsize, 1, fp);
	fread(&ysize, sizeof ysize, 1, fp);
	bufsize = xsize * ysize * 2;
	titlebuf = (unsigned char *)MALLOC(bufsize);
	if (titlebuf > (unsigned char *)0x81000000) ende();
	fread(titlebuf, 1, bufsize, fp);
    fclose(fp);

	CRTMOD(0x10e);
	VPAGE(0);
	WIPE();
	BGTEXTCL( 0, 0x100 ) ;		/* BG Clear */
	BGTEXTCL( 1, 0x100 ) ;

	pp.x1 = 128 - xsize / 2;
	pp.x2 = pp.x1 + xsize - 1;
	pp.y1 = 50;
	pp.y2 = pp.y1 + ysize - 1;
	pp.buf_start = titlebuf;
	pp.buf_end = titlebuf + bufsize - 1;
	PUTGRM(&pp);
	MFREE( titlebuf );
	VPAGE(1);

	bgputs( 0 , 6 , 4 , 2 , "YOKO-SCROLL SHOOTING" );
	bgputs( 0 , 6 , 24 , 2 , "(C)1993 K.M.C" ) ;
	bgputs( 0 , 6 , 25 , 2 , "ALL RIGHTS RESERVED." ); 

	while( !( get_key() & KEY_A )){
		if( BITSNS(12) & 2) ende();	/* End with break */
	}
}

/*	Processing such as displaying the game over message */

void	disp_gameover( void )
{
	int i;

	bgm( -1 );

	bgputs(0, 11, 12, 2, "GAME  OVER");
	for (i = 0; i < 128; i++) {
		wait();
	}
}

void pause( void )		/* Press pause. Try to analyze it as you like. */
{
	volatile unsigned char *a = (volatile unsigned char*)(0x800);	/* hint: BITSNS */
	volatile unsigned char *b = (volatile unsigned char*)(0xe9a001);	/* hint: JOYGET */
	unsigned char c;
	unsigned char d[] = "~~}}{w{w_?_?";
	int e;

	while( (*a & 0x2) != 0 );
	bgputs(0, 11, 12, 2, "PAUSE");
	e = 0;
	do{
		c = (*b & 0x7f);
		if( c != 0x7f ){
			while( *b != 0xff ){
				wait();
			}
			if( c == d[e] ){
				e++;
				if( e == 12 ){
					bgputs(0, 11, 12, 2, "     ");
					ship_data.dead_count = 1;
					return;
				}
			}else{
				e = 0;
			}
		}
	} while( (*a & 0x2) == 0 );
	while( (*a & 0x2) != 0 );
	bgputs(0, 11, 12, 2, "     ");
}

