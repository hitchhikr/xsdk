/*****************************
		  b o m b . c
*****************************/

#include "main.h"
#include "enemy.h"
#include "ship.h"

BOMB *bomb_data;

void bomb_init(void)
{
	BOMB *bomb;
	short i;

	bomb_data = (BOMB *) mymalloc( sizeof(BOMB) * BOMB_NUM );

	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i++, bomb++ ) {
		bomb->type = BOMB_NONEXIST;
	}
}

BOMB *bomb_make( short type )
{
	short i;
	BOMB *bomb;

	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i ++, bomb ++ ) {
		if ( bomb->type < BOMB_EXIST ) break;
	}
	if ( i == BOMB_NUM ) return 0;
	bomb->type = type;
	bomb->sp = sp_alloc();
	bomb->sp->code = BOMB_CODE;
	return bomb;
}

void bomb_erase( BOMB *bomb )
{
	bomb->type = BOMB_NONEXIST;
	sp_free( bomb->sp );
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
	BOMB *bomb;
	short d;

	if ( !( bomb = bomb_make( BOMB_TYPE_A ) ) ) return;
	d = check_dir( x, y, ship_data->x, ship_data->y );
	bomb->x = x;
	bomb->y = y;
	bomb->vx = v[ d ].vx;
	bomb->vy = v[ d ].vy;
}

void bomb_type_a( BOMB *bomb )
{
	if ( ( bomb->x += bomb->vx ) < X_MIN || bomb->x > X_MAX ) {
		bomb_erase( bomb );
	} else if ( ( bomb->y += bomb->vy ) < Y_MIN || bomb->y > Y_MAX ) {
		bomb_erase( bomb );
	}
	bomb->sp->x = SPX( bomb->x );
	bomb->sp->y = SPX( bomb->y );
}

void bomb_move(void)
{
	short i;
	BOMB *bomb;

	for( i = 0, bomb = bomb_data; i < BOMB_NUM; i ++, bomb ++ ) {
		if ( bomb->type < BOMB_EXIST ) continue;
		switch( bomb->type ) {
			case BOMB_TYPE_A:
				bomb_type_a( bomb );
				break;
		}
	}
}
