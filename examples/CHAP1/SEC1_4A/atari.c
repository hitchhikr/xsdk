/*******************************
		  a t a r i . c
*******************************/

/*
  This is a program for hit detection.
  ship<->enemy	shot<->enemy
*/

#include "main.h"
#include "ship.h"
#include "enemy.h"
#include "atari.h"

void atari(void)
{
	extern ENEMY *enemy_data;
	extern SHOT *shot_data;
	extern SHIP *ship_data;
	ENEMY *enemy;
	short i;

	for( i = 0, enemy = enemy_data;
		i < ENEMY_NUM; i ++, enemy++ ) {
		SHOT *shot;
		short j, dx, dy;

		if ( enemy->flag == ENEMY_DEAD ) continue;
		if ( ship_data->flag != TP_DEAD ) {
			dx = enemy->x - ship_data->x;
			dy = enemy->y - ship_data->y;
			if ( dx > - ATARI_YOKO && dx < ATARI_YOKO &&
			  dy > - ATARI_TATE && dy < ATARI_TATE ) {
				/* Your ship and enemies disappear from the screen. */
				/* ship_data->flag = TP_DEAD; */
				enemy->flag = ENEMY_DEAD;
				continue;
			}
		}
		for( j = 0, shot = shot_data; j < SHOT_NUM; j ++, shot ++ ) {
			if ( shot->flag == TP_DEAD ) continue;
			dx = shot->x - enemy->x;
			dy = shot->y - enemy->y;
			if ( dx > - ATARI_YOKO && dx < ATARI_YOKO &&
			  dy > - ATARI_TATE && dy < ATARI_TATE ) {
				/* The enemy disappears from the screen */
				enemy->flag = ENEMY_DEAD;
				shot->flag = TP_DEAD;
				break;
			}
		}
	}
}

