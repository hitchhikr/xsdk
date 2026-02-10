/*****************************
		  a t a r i . c
*****************************/

#include "main.h"
#include "ship.h"
#include "enemy.h"
#include "atari.h"

void atari(void)
{
	extern SHIP *ship_data;
	extern SHOT *shot_data;
	extern ENEMY *enemy_data;
	extern BOMB *bomb_data;
	ENEMY *enemy;
	BOMB *bomb;
	short i;

	for( i = 0, enemy = enemy_data;
		i < ENEMY_NUM; i ++, enemy++ ) {
		SHOT *shot;
		short j, dx, dy;
	    
		if ( enemy->type < ENEMY_EXIST ) continue;
		if ( ship_data->type >= SHIP_EXIST ) {
			dx = enemy->x - ship_data->x;
			dy = enemy->y - ship_data->y;
			if ( dx > - ATARI_YOKO && dx < ATARI_YOKO &&
			  dy > - ATARI_TATE && dy < ATARI_TATE ) {
				/* Your ship and enemies disappear from the screen. */
				/* ship_erase(); */
				enemy_erase( enemy );
				continue;
			}
		}
		for( j = 0, shot = shot_data; j < SHOT_NUM; j ++, shot ++ ) {
			if ( shot->type < SHOT_EXIST ) continue;
			dx = shot->x - enemy->x;
			dy = shot->y - enemy->y;
			if ( dx > - ATARI_YOKO && dx < ATARI_YOKO &&
				dy > - ATARI_TATE && dy < ATARI_TATE ) {
				/* The enemy disappears from the screen */
				enemy_erase( enemy );
				shot_erase( shot );
				break;
			}
		}
	}

	for( i = 0, bomb = bomb_data;
	  i < BOMB_NUM; i ++, bomb++ ) {
		short dx, dy;
	    
		if ( bomb->type < BOMB_EXIST ) continue;
		if ( ship_data->type >= SHIP_EXIST ) {
			dx = bomb->x - ship_data->x;
			dy = bomb->y - ship_data->y;
			if ( dx > - ATARI_YOKO && dx < ATARI_YOKO &&
			  dy > - ATARI_TATE && dy < ATARI_TATE ) {
				/* Your ship and enemy bullets disappear from the screen. */
				/* ship_erase(); */
				bomb_erase( bomb );
				continue;
			}
		}
	}
}

