/********************************
		  e n e m y . c
********************************/

#include "main.h"
#include "enemy.h"

ENEMY *enemy_data;

void enemy_init( void )
{
	ENEMY *ene;
	short i;

	enemy_data = (ENEMY *) mymalloc( sizeof(ENEMY) * ENEMY_NUM );

	/* All the initial data is gone. */
	for( i = 0, ene = enemy_data; i < ENEMY_NUM; i++, ene++ ) {
		ene->type = ENEMY_DEAD;
	}
}

void enemy_make( short type, short x, short y, short vx, short vy )
{
	short i;
	ENEMY *enemy;

	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i ++, enemy ++ ) {
		if ( enemy->type == ENEMY_DEAD ) break;
	}
	if ( i == ENEMY_NUM ) return;
	enemy->type = type;
	enemy->x = x;
	enemy->y = y;
	enemy->vx = vx;
	enemy->vy = vy;
	enemy->count = 0;
}

void enemy_born( void )
{
	static short make_count = ENEMY_MAKE_COUNT;

	if ( -- make_count > 0 ) return;
	else {
		short y;

		make_count = ENEMY_MAKE_COUNT;
		y = easy_rand() & 0x7f + 64;
		switch( easy_rand() & 1 ) {
			case 0:
				if ( easy_rand() & 1 ) {
					enemy_make( ENEMY_TYPE_A, 250, y, 
								- ENEMY_VX, ENEMY_VY );
				} else {
					enemy_make( ENEMY_TYPE_A, 250, y, 
								- ENEMY_VX, - ENEMY_VY );
				}
				break;
			case 1:
				enemy_make( ENEMY_TYPE_B, 250, y, - ENEMY_VX, 0 );
				break;
		}
	}
}

void enemy_type_a( ENEMY *enemy )
{
	enemy->count ++;

	if ( ( enemy->count & 0x3f ) == 0 )
		bomb_born( enemy->x, enemy->y );

	enemy->x += enemy->vx;
	enemy->y += enemy->vy;
	if ( enemy->x < X_MIN ) {
		enemy->type = ENEMY_DEAD;
	}
	if ( enemy->y > X_MAX || enemy->y < X_MIN ) {
		enemy->vy *= -1;
	}
}

void enemy_type_b( ENEMY *enemy )
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
		enemy->type = ENEMY_DEAD;
	}
}

void enemy_type_expl( ENEMY *enemy )
{
	if ( enemy->count ++ > ENEMY_EXPL_COUNT ) {
		enemy->type = ENEMY_DEAD;
	} else {
		enemy->x += enemy->vx;
		enemy->y += enemy->vy;
		if ( enemy->x > X_MAX || enemy->x < X_MIN
		  || enemy->y > Y_MAX || enemy->y < Y_MIN ) {
			enemy->type = ENEMY_DEAD;
		}
	}
}

void enemy_move(void)
{
	ENEMY *ene;
	short i;

	enemy_born();

	for( i = 0, ene = enemy_data; i < ENEMY_NUM; i++, ene++ ) {
		switch( ene->type ) {
			case ENEMY_DEAD:
				continue;
			case ENEMY_EXPL:
				enemy_type_expl( ene );
				break;
			case ENEMY_TYPE_A:
				enemy_type_a( ene );
				break;
			case ENEMY_TYPE_B:
				enemy_type_b( ene );
				break;
		}
	}
}

