/********************************
		  b o m b . c
********************************/

#include "main.h"
#include "enemy.h"
#include "ship.h"

BOMB *bomb_data;

void bomb_init(void)
{
	BOMB *bomb;
	short i;

	bomb_data = (BOMB *) mymalloc( sizeof(BOMB) * BOMB_NUM );

	/* In the initial data, all enemy bullets are dead. */
	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i++, bomb++ ) {
		bomb->type = BOMB_DEAD;
	}
}

void bomb_make( short type, short x, short y, short vx, short vy )
{
	short i;
	BOMB *bomb;

	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i ++, bomb ++ ) {
		if ( bomb->type == BOMB_DEAD ) break;
	}
	if ( i < BOMB_NUM ) {
		bomb->type = type;
		bomb->x = x;
		bomb->y = y;
		bomb->vx = vx;
		bomb->vy = vy;
	}
}

void bomb_born( short x, short y )
{
	extern SHIP *ship_data;
	struct {
		short vx, vy;
	} v[ 9 ] = {
		{ -BOMB_VX, -BOMB_VY }, { 0, -BOMB_VY }, { BOMB_VX, -BOMB_VY },
		{ -BOMB_VX, 0 },		{ 0, 0 },		 { BOMB_VX, 0 },
		{ -BOMB_VX, BOMB_VY },	{ 0, BOMB_VY },  { BOMB_VX, BOMB_VY }
	};
	short d;

	d = check_dir( x, y, ship_data->x, ship_data->y );
	bomb_make( BOMB_TYPE_A, x, y, v[ d ].vx, v[ d ].vy );
}

void bomb_type_a( BOMB *bomb )
{
	if ( ( bomb->x += bomb->vx ) < X_MIN || bomb->x > X_MAX )
	  bomb->type = BOMB_DEAD;
	if ( ( bomb->y += bomb->vy ) < Y_MIN || bomb->y > Y_MAX )
	  bomb->type = BOMB_DEAD;
}

void bomb_move(void)
{
	short i;
	BOMB *bomb;

	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i ++, bomb ++ ) {
		if ( bomb->type == BOMB_DEAD ) continue;
		switch( bomb->type ) {
			case BOMB_TYPE_A:
				bomb_type_a( bomb );
				break;
		}
	}
}
