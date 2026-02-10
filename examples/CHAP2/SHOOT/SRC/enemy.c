/*
 *		enemy & bomb functions
 */

#include "sub.h"
#include "ship.h"
#include "enemy.h"
#include "enemove.h"
#include "sound.h"

/*	counter(for enemy_make())	*/
extern	int	count ;
extern	enepat	enepattern[ ROUNDMAX ][ ENEMAX ] ;
static	enepat	*enepatp ;

static ENEMY enemy_data[ ENEMY_NUM ];
static BOMB bomb_data[ BOMB_NUM ];

/* For enemy_alloc() */
static ENEMY *enemy_free = 0;
ENEMY *enemy_used = 0;

/* For bomb_make() */
static BOMB *bomb_free = 0;
BOMB *bomb_used = 0;

extern int score;

/*
 *		Initialize enemy and enemy bullet data
 */

void enemy_init( short scene )
{
	short i;
	ENEMY *ene;
	BOMB *bm;

	enepatp = enepattern[ scene ] ;
	for( i = 0, ene = &enemy_data[ 0 ]; i < ENEMY_NUM; i ++, ene ++ ) {
		ene->type = -1;
		ene->next = ene + 1;
		ene->pre = ene - 1;
	}
	enemy_data[ 0 ].pre = 0;
	enemy_data[ ENEMY_NUM - 1 ].next = 0;
	enemy_free = &enemy_data[ 0 ];
	enemy_used = 0;

	for( i = 0, bm = &bomb_data[ 0 ]; i < BOMB_NUM; i ++, bm ++ ) {
		bm->type = -1;
		bm->next = bm + 1;
		bm->pre = bm - 1;
	}
	bomb_data[ 0 ].pre = 0;
	bomb_data[ BOMB_NUM - 1 ].next = 0;
	bomb_free = &bomb_data[ 0 ];
	bomb_used = 0;
}

/*
 *		Main enemy movement
 */

void enemy_move( void )
{
	ENEMY *ene;

	enemy_make( 0, 0 );				/* Generate an enemy */
	for( ene = enemy_used; ene; ) {
		if ( ene->flash_count ) {
			/* If the count reaches 0, return the palette */
			if ( !( -- ene->flash_count ) ) {
				ene->sp->code = ( ene->sp->code & 0xff ) | ene->data.palet;
			}
		}

		if ( !ene->move ) {
			ende() ;			/* This guy is weird. */
		} else {
							/* Move the enemy */
			if ( !( *( ene->move ) )( ene ) ) {
							/* When it disappeared */
				ene = enemy_erase( ene );
			} else {
							/* When it has not disappeared */
				ene = ene->next;
			}
		}
	}
}

/*
 *		A function to search for an element in an array when an enemy appears
 */

ENEMY	*enemy_alloc( short spsize )
{
	ENEMY *ene;
	SP *sp;
	if ( !enemy_free ) return (ENEMY *)0;

	/* Allocate sprite */
	if ( !( sp = sprite_alloc( spsize ) ) ) return (ENEMY *)0;

	/* Find a vacant one */
	ene = enemy_free;
	enemy_free = enemy_free->next;

	/* Sprite Pointer Settings */
	ene->sp = sp;

	/* Add to list of used items */
	if ( ene->next = enemy_used ) {
		enemy_used->pre = ene;
	}
	ene->pre = 0;
	enemy_used = ene;
	return ene;
}

/*
 *		Function to spawn enemies
 */

void enemy_make( short scene, short loc )
{

	for ( ; enepatp->count * 16 + 48 <= count ; enepatp++ ) {
		int type = enepatp->type;
		ENEMY *ene;

		/* Find a vacant one */
		ene = enemy_alloc(eneinfo[type].spsize);
		if (!ene) continue;

		ene->type = type;

		ene->data.x = enepatp->x * DOT_RATE;
		ene->data.y = enepatp->y * DOT_RATE;

		/* Various settings */

		ene->data.rx = eneinfo[type].rx;
		ene->data.ry = eneinfo[type].ry;
		ene->data.str = eneinfo[type].str;
		ene->data.energy = eneinfo[type].energy;

		ene->dir = enepatp->dir;
		ene->flg = enepatp->flg;

		ene->count = 0;
		ene->state = 0;

		ene->data.vx = enepatp->vx;
		ene->data.vy = enepatp->vy;
		ene->move = eneinfo[type].move;
		ene->sp->prw = 1;
		ene->sp->code = 0x103;
		ene->data.palet = 0x100;
		ene->flash_count = 0;
		ene->atari = 0;

	}
}

/*
 *		Enemy disappearance
 */

ENEMY *enemy_erase( ENEMY *ene )
{
	ENEMY *next;

	next = ene->next;
	ene->type = -1;

	/* Remove from the list of uses */
	if ( ene->pre ) {
		ene->pre->next = ene->next;
		if ( ene->next ) ene->next->pre = ene->pre;
	} else {
		if ( ene->next ) ene->next->pre = 0;
		enemy_used = ene->next;
	}

	/* Add to vacant list */
	ene->pre = 0;
	ene->next = enemy_free;
	enemy_free = ene;

	/* Free the sprite */
	sprite_free( ene->sp );

	return next;
}

/*
 *		Enemy Explosion Function
 */

int enemy_move_explode( ENEMY *ene )
{
	if ( ++ ene->count >= 10 ) {
		return FALSE;
	} else {
		ene->data.x += ene->data.vx;
		ene->data.y += ene->data.vy;
		ene->sp->x = SP_X( ene->data.x );
		ene->sp->y = SP_Y( ene->data.y );
		ene->sp->code = 0x150 + ene->count / 2;
		return TRUE;
	}
}

/*
 *		A function to be called when an enemy dies.
 */

void enemy_explode( ENEMY *ene )
{
	score += eneinfo[ene->type].points;

	sd_on( eneinfo[ene->type].crash_sdno );		/* Generates sound effects */
	ene->type = -1;					/* Change the type to "Explosion" */
	ene->count = 0;
	ene->move = enemy_move_explode;
	ene->sp->type = CHR1x1;
	ene->sp->code = 0x150;
}

/*
 *		Enemy bullet movement
 */

void bomb_move( void )
{
	BOMB *bomb;

	for( bomb = bomb_used; bomb; ) {
		bomb->data.x += bomb->data.vx;			/* Move */
		bomb->data.y += bomb->data.vy;
		if ( bomb->data.x < X_MIN || bomb->data.x > X_MAX ||
			bomb->data.y < Y_MIN || bomb->data.y > Y_MAX ) {
								/* It's off screen. */
			bomb = bomb_erase( bomb );
		} else {
			bomb->sp->x = SP_X( bomb->data.x );
			bomb->sp->y = SP_X( bomb->data.y );
			bomb->sp->code = 0x155 + (count & 0xc) * 0x1000;
			bomb = bomb->next;
		}
	}
}

/*
 *		Enemy bullets
 */

void bomb_make( short x, short y, short vx,  short vy, short type )
{
	BOMB *bomb;
	SP *sp;

	/* Check if it's available */
	if ( !bomb_free ) return;

	/* Allocate sprite */
	if ( !( sp = sprite_alloc( CHR1x1 ) ) ) return;

	/* Find a vacant one */
	bomb = bomb_free;
	bomb_free = bomb_free->next;

	/* Sprite Pointer Settings */
	bomb->sp = sp;

	/* Add to list of used items */
	if ( bomb->next = bomb_used ) {
		bomb_used->pre = bomb;
	}
	bomb->pre = 0;
	bomb_used = bomb;

	/* Various settings */
	bomb->type = type;
	bomb->data.x = x;
	bomb->data.y = y;
	bomb->data.rx = DOT_RATE * 10;
	bomb->data.ry = DOT_RATE * 2;
	bomb->data.vx = vx;
	bomb->data.vy = vy;
	bomb->sp->prw = 1;
	bomb->sp->code = 0x155;
	bomb->data.palet = 0x100;
}

/*
 *		Enemy bullets disappear
 */

BOMB *bomb_erase( BOMB *bomb )
{
	BOMB *next;

	next = bomb->next;
	bomb->type = -1;

	/* Remove from the list of uses */
	if ( bomb->pre ) {
		bomb->pre->next = bomb->next;
		if ( bomb->next ) bomb->next->pre = bomb->pre;
	} else {
		if ( bomb->next ) bomb->next->pre = 0;
		bomb_used = bomb->next;
	}

	/* Add to vacant list */
	bomb->pre = 0;
	bomb->next = bomb_free;
	bomb_free = bomb;

	/* Free the sprite */
	sprite_free( bomb->sp );

	return next;
}
