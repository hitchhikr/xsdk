/********************************
		  s h i p . c
********************************/

#include <iocslib.h>
#include "main.h"
#include "ship.h"

SHIP *ship_data;
SHOT *shot_data;

void shot_make( short x, short y );

void ship_init( void )
{
	ship_data = (SHIP *) mymalloc( sizeof(SHIP) );
	ship_data->x = 8;
	ship_data->y = 128;
	ship_data->type = SHIP_ALIVE;
}

void ship_move( void )
{
	short key;

	key = get_key();
	if ( ( key & KEY_UPPER ) && ( ship_data->y > Y_MIN ) ) { /* Up */
		ship_data->y -= VELO_Y;
	}
	if ( ( key & KEY_LOWER ) && ( ship_data->y < Y_MAX ) ) { /* Down */
		ship_data->y += VELO_Y;
	}
	if ( ( key & KEY_LEFT ) && ( ship_data->x > X_MIN ) ) { /* Left */
		ship_data->x -= VELO_X;
	}
	if ( ( key & KEY_RIGHT ) && ( ship_data->x < X_MAX ) ) { /* Right */
		ship_data->x += VELO_X;
	}

	/* Make a bullet */
	if ( key & KEY_A ) { /* A button */
		int i;
		SHOT *shot;

		for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
			if ( shot->type == SHOT_DEAD ) break;
		}
		if ( i < SHOT_NUM ) {
			shot->type = SHOT_ALIVE;
			shot->x = ship_data->x;
			shot->y = ship_data->y;
		}
	}
}

void shot_make( short x, short y )
{
	int i;
	SHOT *shot;

	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		if ( shot->type == SHOT_DEAD ) break;
	}
	if ( i < SHOT_NUM ) {
		shot->type = SHOT_ALIVE;
		shot->x = x;
		shot->y = y;
	}
}

void shot_init( void )
{
	short i;
	SHOT *shot;

	shot_data = (SHOT *) mymalloc( sizeof(SHOT) * SHOT_NUM );
	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ )
		shot->type = SHOT_DEAD;
}

void shot_move( void )
{
	int i;
	SHOT *shot;

	for( i = 0, shot = shot_data; i < SHOT_NUM; i ++, shot ++ ) {
		if ( shot->type == SHOT_DEAD ) continue;
		shot->x += SHOT_VX;
		if ( shot->x >= X_MAX ) shot->type = SHOT_DEAD;
	}
}

