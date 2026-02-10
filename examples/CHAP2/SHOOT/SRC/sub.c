/*
 *		Functions for working with sprites and other things
 */


#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>
#include <doslib.h>
#include <math.h>
#include "ship.h"
#include "enemy.h"
#include "sub.h"
#include "bg.h"
#include "sound.h"

/* Velocity array */
short vx_dir[ DIRECTION_MAX ], vy_dir[ DIRECTION_MAX ];
short sin_table[ SINTABLE_MAX ];

/* Register Buffer */
static SP spreg_data[ SP_NUM ];
/* The number currently in use */
static short sp_used_num = 0;

/* for sprite_alloc & sprite_free */
static SP *spreg_free = 0;
static SP *spreg_used = 0;

/* Sprite transfer macro */
#define	my_spset( spno, x, y, code, prw )	\
	{	volatile short	*tmp = ( volatile short * )0xeb0000 + (spno) * 4; \
		*( tmp++ ) = ( x );		*( tmp++ ) = ( y ); \
		*( tmp++ ) = ( code );		*( tmp ) = ( prw ); }

/*
 *		Vertical retrace wait function
 */

void	wait( void )
{
	/* SP_REGST( SP_NUM, 0, 0, 0, 0 ); <- Even with this, you can detect the return line */
	/* Wait for the display period */
	while( !( *( volatile unsigned char * )0xe88001 & 0x10 ) );
	/* Wait for the blanking period */
	while( *( volatile unsigned char * )0xe88001 & 0x10 );
}

/*
 *		Move a sprite
 */

void trans_sprite_data( void )
{
	unsigned int j;
	SP *sp = spreg_data;

	for( j = 0, sp = spreg_used; sp; sp = sp->next ) {
		if ( sp->type < 0 ) continue;
		switch( sp->type ) {
		  case CHR1x1:
			my_spset( j++, sp->x & 511, sp->y & 511, sp->code, sp->prw );
			break ;
		  case CHR2x1:
			my_spset( j++, sp->x-8, sp->y, sp->code, sp->prw );
			my_spset( j++, sp->x+8, sp->y, sp->code+1, sp->prw );
			break ;
		  case CHR2x2:
			my_spset( j++, sp->x-8, sp->y-8, sp->code, sp->prw );
			my_spset( j++, sp->x+8, sp->y-8, sp->code+1, sp->prw );
			my_spset( j++, sp->x-8, sp->y+8, sp->code+2, sp->prw );
			my_spset( j++, sp->x+8, sp->y+8, sp->code+3, sp->prw );
			break ;
		  case CHR3x3:
			my_spset( j++, sp->x-16, sp->y-16, sp->code, sp->prw );
			my_spset( j++, sp->x   , sp->y-16, sp->code+1, sp->prw );
			my_spset( j++, sp->x+16, sp->y-16, sp->code+2, sp->prw );
			my_spset( j++, sp->x-16, sp->y   , sp->code+3, sp->prw );
			my_spset( j++, sp->x   , sp->y   , sp->code+4, sp->prw );
			my_spset( j++, sp->x+16, sp->y   , sp->code+5, sp->prw );
			my_spset( j++, sp->x-16, sp->y+16, sp->code+6, sp->prw );
			my_spset( j++, sp->x   , sp->y+16, sp->code+7, sp->prw );
			my_spset( j++, sp->x+16, sp->y+16, sp->code+8, sp->prw );
			break ;
		}
	}
	for( ; j < SP_NUM; j ++ ) {
		my_spset( j, 0, 0, 0, 0 );
	}
}

SP *sprite_alloc( short type )
{
	SP *sp;

	/* Find a vacant one */
	if ( !spreg_free ) return 0;
	sp = spreg_free;
	spreg_free = spreg_free->next;

	/* Add to list of used items */
	if ( sp->next = spreg_used ) {
		spreg_used->pre = sp;
	}
	sp->pre = 0;
	spreg_used = sp;

	sp->type = type;
	sp_used_num ++;
	return sp;
}

void sprite_free( SP *sp )
{
	sp->type = -1;

	/* Remove from the list of uses */
	if ( sp->pre ) {
		sp->pre->next = sp->next;
		if ( sp->next ) sp->next->pre = sp->pre;
	} else {
		if ( sp->next ) sp->next->pre = 0;
		spreg_used = sp->next;
	}

	/* Add to vacant list */
	sp->pre = 0;
	sp->next = spreg_free;
	spreg_free = sp;

	sp_used_num --;
}

/*
 *		Joy stick & key input routine
 */

int get_key( void )
{
	static short key_a_flag = 1;
	int k, s = 0;

	s = ~JOYGET( 0 );

	k = BITSNS( 8 );
	if ( k & 0x10 ) s |= KEY_UP;
	if ( k & 0x80 ) s |= KEY_LEFT;
	k = BITSNS( 9 );
	if ( k & 0x02 ) s |= KEY_RIGHT;
	if ( k & 0x10 ) s |= KEY_DOWN;
	k = BITSNS( 14 );
	if ( k & 0x04 || s & 64 ) {
		if ( key_a_flag ) {
			s |= KEY_A;
			key_a_flag = 0;
		}
	} else {
		key_a_flag = 1;
	}
	if ( k & 0x08 ) s |= KEY_B;

	return s;
}

/*
 *		Initial settings for screen
 */

void init( void )
{
	unsigned int j;
	SP *sp;

	CRTMOD( 6 );	/* 256x256 16/16 512  31kHz */
	G_CLR_ON();
	B_CUROFF();
	C_CUROFF();
	SP_ON();

	BGCTRLST( 0, 0, 1 );			/* Background settings */
	BGCTRLST( 1, 1, 1 );
	BGTEXTCL( 0, BG_NULL );			/* BG Clear */
	BGTEXTCL( 1, BG_NULL );
	BGSCRLST( 0, 0, 0 );
	BGSCRLST( 1, 0, 0 );

	for( j = 0x0; j < 128; j ++ ) {
		my_spset( j, 0, 0, 0, 0 );
	}

	/*	Initial list settings for sprites	*/
	for( j = 0, sp = &spreg_data[ 0 ]; j < SP_NUM; j ++, sp ++ ) {
		sp->type = -1;
		sp->next = sp + 1;
		sp->pre = sp - 1;
	}
	spreg_data[ 0 ].pre = 0;
	spreg_data[ SP_NUM - 1 ].next = 0;
	spreg_free = &spreg_data[ 0 ];
	spreg_used = 0;

	/* Create a table for the enemy movement routine */
	for ( j = 0; j < DIRECTION_MAX; j++ ){
		vx_dir[j] = (short)(cos(j * 6.28318 / DIRECTION_MAX) * SPEED_RATE ) ;
		vy_dir[j] = (short)(sin(j * 6.28318 / DIRECTION_MAX) * SPEED_RATE ) ;
	}
	for ( j = 0; j < SINTABLE_NUM; j++ ){
		sin_table[j] = (sin((double)j * 6.28318 / SINTABLE_NUM) * SINTABLE_MAX) ;
	}
}

void my_error( char *str )
{
	fputs( str, stderr );
	ende();
}

void ende( void )
{
	bgm( -1 );
	KFLUSHIO( 0xff );
	CRTMOD( 4 );
	B_CURON();
	C_CURON();
	exit( 0 );
}

/* Calculates the direction of (x2,y2) from (x1,y1).
 * The returned value is 
 *
 *    12
 *  8 + 0
 *    4 
 *
 * (clockwise).
 */

#define	DIRECTION_RIGHT	0
#define	DIRECTION_LEFT	(DIRECTION_MAX / 2)
#define	DIRECTION_UP	(DIRECTION_MAX * 3 / 4)
#define	DIRECTION_DOWN	(DIRECTION_MAX / 4)

short	calc_direction(short x1, short y1, short x2, short y2)
{
	static short tantbl[ 17 ] = {	 0, 0, 0, 0, 1,
					 1, 1, 1, 1, 1,
					 1, 1, 2, 2, 2,
					 2, 2};

	short	dx = x2 - x1; 
	short	dy = y2 - y1;

	if (dx <= 31 && dx >= -31){
		if(dy > 0) return DIRECTION_DOWN;
		else return DIRECTION_UP;
	} else if (dy <= 31 && dy >= -31){
		if (dx > 0) return DIRECTION_RIGHT;
		else return DIRECTION_LEFT;
	}

	if (dx > 0) {
		if (dy < 0) {						/* 12-16 */
			dy = -dy;
			if (dy > dx) return tantbl[ dx / (dy / 16) ] + 12;
			else return (16 - tantbl[ dy / (dx / 16) ]) & 15;
		} else {						/* 0-4 */
			if (dy > dx) return 4 - tantbl[ dx / (dy / 16) ];
			else return tantbl[ dy / (dx / 16) ];
		}
	} else {
		dx = -dx;
		if (dy > 0) {						/* 4-8 */
			if (dy > dx) return 4 + tantbl[ dx / (dy / 16) ];
			else return 8 - tantbl[ dy / (dx / 16) ];
		} else {						/* 8-12 */
			dy = -dy;
			if (dy > dx) return 12 - tantbl[ dx / (dy / 16) ]; 
			else return 8 + tantbl[ dy / (dx / 16) ];
		}
	}
	return -1;		/* Funny */
}
