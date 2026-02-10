/***********************************
		  e n e m y . c
***********************************/

#include "main.h"
#include "enemy.h"

ENEMY *enemy_data;

void enemy_init(void)
{
	ENEMY *ene;
	short i;

	enemy_data = (ENEMY *) mymalloc( sizeof(ENEMY) * ENEMY_NUM );

	/* The initial data is correct */
	for( i = 0, ene = enemy_data; i < ENEMY_NUM; i++, ene++ ) {
		ene->flag = ENEMY_LIVE;
		ene->x = 250;
		ene->y = i * 32 + i * 5;
		ene->vx = -1 - i;
		ene->vy = ((rand()&1)*2-1) * i/2;
	}
}

void enemy_move(void)
{
	ENEMY *ene;
	short i;

	for( i = 0, ene = enemy_data; i < ENEMY_NUM; i++, ene++ ) {
		if ( ene->flag == ENEMY_DEAD ) continue;

		/* When I go to the edge, I try to bounce back. */
		if ( ene->x + ene->vx <= X_MAX && ene->x + ene->vx >= X_MIN )
			ene->x += ene->vx;
		else
			ene->vx *= -1;

		if ( ene->y + ene->vy <= Y_MAX && ene->y + ene->vy >= Y_MIN )
			ene->y += ene->vy;
		else
			ene->vy *= -1;
		if( ene->x + ene->vx < 0 )	ene->x = 250;
	}
}
