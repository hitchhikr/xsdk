/*
 *		Hit detection processing functions
 */

#include "atari.h"
#include "sub.h"
#include "enemy.h"
#include "ship.h"
#include "bg.h"
#include "sound.h"

#define	BOSS_TYPE	10				/* BOSS Enemy Type */

extern SHIP ship_data;
extern SHOT *shot_used;
extern ENEMY *enemy_used;
extern BOMB *bomb_used;

/*
 *		What happens when an enemy takes damage
 */

int	enemy_damage( ENEMY *enemy, int damage )
{
	int	flg = 0;

	if ( !enemy->atari ) {		/* Is it an enemy with hit detection? */
		if ( ( enemy->data.energy -= damage ) <= 0 ) {
			/* The enemy is dead */
			if (enemy->type == BOSS_TYPE) flg = 1;
			enemy_explode( enemy );
		} else {
			/* The enemy is still alive (glowing process) */
			enemy->flash_count = FLASH_MAX;
			enemy->sp->code = ( enemy->sp->code & 0xf0ff ) | FLASH_PAL;
		}
	}
	return flg;
}

/*
 *		Collision detection functions
 */

int atari( void )
{
	ENEMY *enemy;
	BOMB *bomb;
	SHOT *shot;
	int   flg = 0;		/* Return Value */
	short sx = ship_data.data.x, sy = ship_data.data.y;
	short srx = ship_data.data.rx, sry = ship_data.data.ry;

	for( enemy = enemy_used; enemy; enemy = enemy->next ) {
		register short ex = enemy->data.x, ey = enemy->data.y;
		register short erx = enemy->data.rx,  ery = enemy->data.ry;

		if ( enemy->type < 0 ) continue;	/* If it explodes, ignore it */

		/* Determine enemies and your own aircraft */

		if ( ( - ( erx + srx ) < ex - sx ) && ( ex - sx < erx + srx )
		&& ( - ( ery + sry ) < ey - sy ) && ( ey - sy < ery + sry ) ) {
			/* Your ship dies */
			if ( !ship_data.dead_count ) ship_data.dead_count = 1;

			/* Enemy receives damage */
			flg = enemy_damage( enemy, ship_data.data.str );
			continue;
		}

		/* Enemy and self-fire detection */

		for( shot = shot_used; shot; ) {
			short stx = shot->data.x, sty = shot->data.y;
			short strx = shot->data.rx, stry = shot->data.ry;

			if ( ( - ( erx + strx ) < ex - stx ) && ( ex - stx < erx + strx )
			&& ( - ( ery + stry ) < ey - sty ) && ( ey - sty < ery + stry ) ) {
				/* Enemy receives damage */
				flg = enemy_damage( enemy, shot->data.str );
				shot = shot_erase( shot );	/* Eliminate bullets */
				break;
			} else {
				shot = shot->next;
			}
		}
	}

	for( bomb = bomb_used; bomb; ) {
		short bx = bomb->data.x, by = bomb->data.y;
		short brx = bomb->data.rx, bry = bomb->data.ry;

		/* Distinguishing between enemy bullets and the background */

		if (bgatari(bomb->data.x, bomb->data.y)) {
			/* Eliminate enemy bullets */
			bomb = bomb_erase(bomb);
			continue;
		}

		/* Distinguish between enemy bullets and your own aircraft */

		if ( ( - ( brx + srx ) < bx - sx ) && ( bx - sx < brx + srx )
		&& ( - ( bry + sry ) < by - sy ) && ( by - sy < bry + sry ) ) {
			/* (If your ship is not invincible) Your ship will die. */
			if ( !ship_data.dead_count ) ship_data.dead_count = 1;
			/* Eliminate enemy bullets */
			bomb = bomb_erase( bomb );
			continue;
		} else {
			bomb = bomb->next;
		}
	}

	/* Judge between your own bullet and the background */

	for ( shot = shot_used; shot; ){
		switch( bgatari(shot->data.x, shot->data.y) ) {
		  case	2:				/* Destructible walls */
			bgbreak(shot->data.x, shot->data.y);
		  case	1:				/* Indestructible wall */
			shot = shot_erase(shot);
			break;
		  default:
			shot = shot->next;
		}
	}

	/* Judge between the player's ship and the background */

	if (bgatari(ship_data.data.x, ship_data.data.y)) {
		/* Your ship dies */
		if (!ship_data.dead_count) ship_data.dead_count = 1;
	}

	return flg;
}
