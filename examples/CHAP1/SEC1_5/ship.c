/*****************************
		  s h i p . c
*****************************/

#include <iocslib.h>
#include "main.h"
#include "ship.h"

#define SHIP_X_MIN (20*SPRATE)
#define SHIP_X_MAX (240*SPRATE)
#define SHIP_Y_MIN (20*SPRATE)
#define SHIP_Y_MAX (240*SPRATE)

SHIP *ship_data;
SHOT *shot_data;

void ship_init( void )
{
	ship_data = (SHIP *) mymalloc( sizeof(SHIP) );
	ship_data->x = 40 * SPRATE;
	ship_data->y = 128 * SPRATE;
	ship_data->type = SHIP_EXIST;
	ship_data->sp = sp_alloc();
	ship_data->sp->code = SHIP_CODE;
}

void ship_move( void )
{
	short key;

	key = get_key();
	if ( ( key & KEY_UPPER ) 
		 && ( ship_data->y > SHIP_Y_MIN ) ) { /* Up */
		ship_data->y -= SHIP_VY;
	}
	if ( ( key & KEY_LOWER ) 
		 && ( ship_data->y < SHIP_Y_MAX ) ) { /* Down */
		ship_data->y += SHIP_VY;
	}
	if ( ( key & KEY_LEFT ) 
		 && ( ship_data->x > SHIP_X_MIN ) ) { /* Left */
		ship_data->x -= SHIP_VX;
	}
	if ( ( key & KEY_RIGHT ) 
		 && ( ship_data->x < SHIP_X_MAX ) ) { /* Right */
		ship_data->x += SHIP_VX;
	}
	ship_data->sp->x = SPX( ship_data->x );
	ship_data->sp->y = SPY( ship_data->y );

	/* Make a bullet */
	if ( key & KEY_A ) {
		shot_make( ship_data->x, ship_data->y );
	}
}

void shot_make( short x, short y )
{
	int i;
	SHOT *shot;

	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		if ( shot->type < SHOT_EXIST ) break;
	}
	if ( i < SHOT_NUM ) {
		shot->type = SHOT_EXIST;
		shot->x = x;
		shot->y = y;
		shot->sp = sp_alloc();
		shot->sp->code = SHOT_CODE;
		shot->sp->x = SPX( shot->x );
		shot->sp->y = SPY( shot->y );
	}
}

void shot_erase( SHOT *shot )
{
	sp_free( shot->sp );
	shot->type = SHOT_NONEXIST;
}

void shot_init( void )
{
	short i;
	SHOT *shot;

	shot_data = (SHOT *) mymalloc( sizeof(SHOT) * SHOT_NUM );
	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		shot->type = SHOT_NONEXIST;
	}
}

void shot_move( void )
{
	int i;
	SHOT *shot;

	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		if ( shot->type < SHOT_EXIST ) continue;
		shot->x += SHOT_VX;
		shot->sp->x = SPX( shot->x );
		if ( shot->x >= X_MAX ) {
			shot_erase( shot );
		}
	}
}
