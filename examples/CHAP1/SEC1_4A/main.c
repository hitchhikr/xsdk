/******************************
		  m a i n . c
*******************************/

#include "main.h"
#include "ship.h"
#include "enemy.h"
#include "atari.h"

void init( void )
{
	int i;
	unsigned int j;

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
	for( j = 0x80000000; j < 0x80000000 + 128; j ++ ) {
		SP_REGST( j, 0, 0, 0, 0 );
	}

	/* Other initialization */
	ship_init();
	shot_init();
	enemy_init();
}

void *mymalloc( int size )
{
	void *p;

	if ( !( p = (void *) malloc( size ) ) ) {
		fputs( "cannot alloc memory at mymalloc()", stderr );
		exit( 1 );
	}
}

void trans_sp( void )
{
	extern SHIP *ship_data;
	extern SHOT *shot_data;
	extern ENEMY *enemy_data;
	SHOT *shot;
	ENEMY *enemy;
	int i;

	/* Display of your own aircraft */
	SP_REGST( 0, ship_data->x, ship_data->y, SHIP_CODE, 2 );

	/* Display of own bullet */
	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		if ( shot->flag == TP_DEAD ) {
			SP_REGST( 0x80000000 + i + 1, 0, 0, 0, 0 );
		} else {
			SP_REGST( 0x80000000 + i + 1, shot->x, 
					   shot->y, SHOT_CODE, 2 );
		}
	}

	/* Enemy Display */
	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i ++, enemy ++ ) {
		if ( enemy->flag == ENEMY_DEAD ) {
			SP_REGST( 0x80000000 + i + SHOT_NUM + 1, 0, 0, 0, 0 );
		} else {
			SP_REGST( 0x80000000 + i + SHOT_NUM + 1, 
					   enemy->x, enemy->y, ENEMY_CODE, 2 );
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
	int i;

	init();

	while( 1 ) {
		ship_move();
		shot_move();
		enemy_move();

		trans_sp();

		atari();
	    
		if ( !( JOYGET( 0 ) & 0x40 ) ) { /* B button */
			exit( 0 );
		}
	}
}
