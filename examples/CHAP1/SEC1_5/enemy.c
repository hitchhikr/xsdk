/*******************************
		  e n e m y . c
*******************************/

#include "main.h"
#include "enemy.h"

ENEMY *enemy_data;

void enemy_init( void )
{
	ENEMY *ene;
	short i;

	enemy_data = (ENEMY *) mymalloc( sizeof(ENEMY) * ENEMY_NUM );

	for( i = 0, ene = enemy_data; i < ENEMY_NUM; i++, ene++ ) {
		ene->type = ENEMY_NONEXIST;
	}
}

ENEMY *enemy_make( short type )
{
	short i;
	ENEMY *enemy;

	for( i = 0, enemy = enemy_data; i < ENEMY_NUM; i ++, enemy ++ ) {
		if ( enemy->type < ENEMY_EXIST ) break;
	}
	if ( i == ENEMY_NUM ) return 0;
	enemy->type = type;
	enemy->sp = sp_alloc();
	enemy->sp->code = ENEMY_CODE;

	return enemy;
}

void enemy_erase( ENEMY *enemy )
{
	enemy->type = ENEMY_NONEXIST;
	sp_free( enemy->sp );
}

void enemy_born( void )
{
	/* Determines the interval between enemy spawns */
	static short make_count = ENEMY_MAKE_COUNT;
	/* Specifies the location of the enemy on the map data. */
	static short map_location = 0;
	ENEMY *enemy;
	ENEMY_MAP_DATA *enemy_map;

	if ( -- make_count > 0 ) return;
	else {
		make_count = ENEMY_MAKE_COUNT;
		enemy_map = get_enemy_map_data( map_location ++ ); /* enedat.c */
		if ( !( enemy = enemy_make( enemy_map->type ) ) ) {
			/* There was no space in the data */
			return;
		}
		enemy->move = enemy_map->move;
		enemy->x = enemy_map->x;
		enemy->y = enemy_map->y;
		enemy->count = 0;
	}
}

void enemy_move(void)
{
	ENEMY *ene;
	short i;

	enemy_born();

	for( i = 0, ene = enemy_data; i < ENEMY_NUM; i++, ene++ ) {
		if ( !ene->move || ene->type == ENEMY_NONEXIST ) continue;
		else (*ene->move)( ene );
	}
}
