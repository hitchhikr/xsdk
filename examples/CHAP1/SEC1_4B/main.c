/********************************
		  m a i n . c
********************************/

#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>
#include "main.h"
#include "ship.h"
#include "enemy.h"
#include "atari.h"

void init( void )
{
	int j;

	/* Screen Settings */
	CRTMOD( 6 );
	G_CLR_ON();

	/* Erase the cursor */
	B_CUROFF();

	/* Sprites related initializations */
	SP_ON();
	BGCTRLST( 0, 0, 0 );
	BGCTRLST( 1, 1, 0 );

	/* Initialize the sprite registers */
	for( j = 0; j < 128; j ++ ) {
		SP_REGST( j, 0x80000000, 0, 0, 0 );
	}

	/* Other initializations */
	ship_init();
	shot_init();
	enemy_init();
	bomb_init();
}

void *mymalloc( int size )
{
	void *p;

	if ( !( p = (void *) malloc( size ) ) ) {
		fputs( "cannot alloc memory at mymalloc()", stderr );
		exit( 1 );
	}
	return p;
}

/* Random numbers */
short easy_rand( void )
{
	static unsigned long int next = 3749696;

	next = next * 1103515245 + 5963;
	return (short) ( next & 0x7fff );
}

/*
  -direction-
	 0 1 2
	 3 4 5
	 6 7 8
*/
#define DIR_SGN( c )\
	( (c) > 0 ? 2 : ( (c) < 0 ? 0 : 1 ) )
short check_dir( short x1, short y1, short x2, short y2 )
{
	return DIR_SGN( x2 - x1 ) + DIR_SGN( y2 - y1 ) * 3;
}

void trans_sp( void )
{
	extern SHIP *ship_data;
	extern SHOT *shot_data;
	extern ENEMY *enemy_data;
	extern BOMB *bomb_data;
	SHOT *shot;
	ENEMY *enemy;
	BOMB *bomb;
	int i;

	/* Display of your own aircraft */
	SP_REGST( 0, ship_data->x, ship_data->y, SHIP_CODE, 2 );

	/* Display of own bullet */
	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		if ( shot->type == SHOT_DEAD ) {
			SP_REGST( i + 1 + 0x80000000, 0, 0, 0, 0 );
		} else {
			SP_REGST( i + 1 + 0x80000000, shot->x, shot->y, SHOT_CODE, 2 );
		}
	}

	/* Enemy Display */
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i ++, enemy ++ ) {
		if ( enemy->type == ENEMY_DEAD ) {
			SP_REGST( i + SHOT_NUM + 1 + 0x80000000, 0, 0, 0, 0 );
		} else {
			SP_REGST( i + SHOT_NUM + 1 + 0x80000000, enemy->x, enemy->y, ENEMY_CODE, 2 );
		}
	}

	/* Enemy bullet display */
	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i ++, bomb ++ ) {
		if ( bomb->type == BOMB_DEAD ) {
			SP_REGST( i + SHOT_NUM + ENEMY_NUM + 1 + 0x80000000, 0, 0, 0, 0 );
		} else {
			SP_REGST( i + SHOT_NUM + ENEMY_NUM + 1 + 0x80000000, bomb->x, bomb->y, BOMB_CODE, 2 );
		}
	}
}

short get_key( void )
{
	static short repeat_flag = TRUE;
	short ret = 0;

	if ( !( JOYGET( 0 ) & 0x01 ) ) ret |= KEY_UPPER;
	if ( !( JOYGET( 0 ) & 0x02 ) ) ret |= KEY_LOWER;
	if ( !( JOYGET( 0 ) & 0x04 ) ) ret |= KEY_LEFT;
	if ( !( JOYGET( 0 ) & 0x08 ) ) ret |= KEY_RIGHT;
	if ( !( JOYGET( 0 ) & 0x20 ) ) { /* A button */
		if ( repeat_flag ) {
			ret |= KEY_A;
			repeat_flag = FALSE;
		}
	}else{
		repeat_flag = TRUE;
	}

	return ret;
}

void main( void )
{
	extern SHIP *ship_data;

	init();

	while( 1 ) {
		ship_move();
		shot_move();
		enemy_move();
		bomb_move();

		trans_sp();

		atari();
	    
		if( !( JOYGET( 0 ) & 0x40 ) ) { 		/* B button */
			exit( 0 );
		}
	}
}

