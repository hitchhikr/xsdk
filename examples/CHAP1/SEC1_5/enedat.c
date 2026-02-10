/*********************************
		  e n e d a t . c
*********************************/

#include "main.h"
#include "enemy.h"

#define TYPE_NUM 16

static void enemy_type_expl( ENEMY *enemy );
static void enemy_type_a( ENEMY *enemy );
static void enemy_type_b( ENEMY *enemy );

ENEMY_MAP_DATA enemy_map_data[] = {
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_b },
	{ 1, 256*SPRATE, 128*SPRATE, enemy_type_a },
	{ 0, 0, 0, 0 }
};

ENEMY_MAP_DATA *get_enemy_map_data( short loc )
{
	return &enemy_map_data[ loc & ( TYPE_NUM - 1 ) ];
}

static void enemy_type_a( ENEMY *enemy )
{
	if ( enemy->count == 0 ) { /* The very beginning */
		enemy->vx = - ENEMY_VX;
		enemy->vy = ENEMY_VY;
	}
	enemy->count ++;

	if ( ( enemy->count & 0x3f ) == 0 )
		bomb_born( enemy->x, enemy->y );

	enemy->x += enemy->vx;
	enemy->y += enemy->vy;
	enemy->sp->x = SPX( enemy->x );
	enemy->sp->y = SPX( enemy->y );
	if ( enemy->x < X_MIN ) {
		enemy_erase( enemy );
		return;
	}
	if ( enemy->y > Y_MAX || enemy->y < Y_MIN ) {
		enemy->vy *= -1;
	}
}

static void enemy_type_b( ENEMY *enemy )
{
	enemy->count ++;

	if ( ( enemy->count & 0x3f ) == 0 )
		bomb_born( enemy->x, enemy->y );

	if ( enemy->count < 200 ) {
		enemy->x -= ENEMY_VX;
		enemy->vx = - ENEMY_VX;
		enemy->vy = 0;
	} else if ( enemy->count < 230 ) {
		enemy->y += ENEMY_VY;
		enemy->vx = 0;
		enemy->vy = ENEMY_VY;
	} else if ( enemy->count < 380 ) {
		enemy->x += ENEMY_VX;
		enemy->vx = ENEMY_VX;
		enemy->vy = 0;
	} else if ( enemy->count < 410 ) {
		enemy->y += ENEMY_VY;
		enemy->vx = 0;
		enemy->vy = ENEMY_VY;
	} else {
		enemy->x -= ENEMY_VX;
		enemy->vx = - ENEMY_VX;
		enemy->vy = 0;
	}
	if ( enemy->x > X_MAX || enemy->x < X_MIN
	  || enemy->y > Y_MAX || enemy->y < Y_MIN ) {
		enemy_erase( enemy );
		return;
	}
	enemy->sp->x = SPX( enemy->x );
	enemy->sp->y = SPX( enemy->y );
}

static void enemy_type_expl( ENEMY *enemy )
{
	if ( enemy->count ++ > ENEMY_EXPL_COUNT ) {
		enemy_erase( enemy );
	} else {
		enemy->x += enemy->vx;
		enemy->y += enemy->vy;
		enemy->sp->x = SPX( enemy->x );
		enemy->sp->y = SPX( enemy->y );
		if ( enemy->x > X_MAX || enemy->x < X_MIN
		  || enemy->y > Y_MAX || enemy->y < Y_MIN ) {
			enemy_erase( enemy );
		}
	}
}
