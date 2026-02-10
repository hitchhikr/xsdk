#include "sub.h"
#include "ship.h"
#include "sound.h"

SHIP ship_data;
static SHOT shot_data[ SHOT_NUM ];

/* Initial value when the player's ship dies */
#define INIT_X DOT_RATE * 64
#define INIT_Y DOT_RATE * 128
#define	INIT_DEAD_COUNT -120

/* Shot List */
SHOT *shot_used = 0;
static SHOT *shot_free = 0;

/*
 *		Initialize your ship and bullet data
 */

void ship_init( void )
{
	short j;
	SHOT *sh;
	
	if ( !( ship_data.sp = sprite_alloc( CHR2x1 ) ) ) {
		my_error( "sprite_alloc() error during ship_init()" );
	}

	/* Setting your own aircraft data */
	ship_data.data.x = INIT_X ;
	ship_data.data.y = INIT_Y ;
	ship_data.data.rx = DOT_RATE * 4;
	ship_data.data.ry = DOT_RATE * 3;
	ship_data.data.str = 100;
	ship_data.data.energy = 10;

	/* Palette Settings */
	ship_data.data.palet = 0x100;

	/* Set data for sprites */
	ship_data.sp->code = 0x101;
	ship_data.sp->prw = 1;

	/* Initialize shot data */
	for( j = 0, sh = &shot_data[ 0 ]; j < SHOT_NUM; j ++, sh ++ ) {
		sh->type = -1;
		sh->next = sh + 1;
		sh->pre = sh - 1;
	}
	shot_data[ 0 ].pre = 0;
	shot_data[ SHOT_NUM - 1 ].next = 0;
	shot_free = &shot_data[ 0 ];
	shot_used = 0;
}

/*
 *		Movement of your own aircraft
 */

#define	SPCODE_SHIP_UU	0x140	/* Sprite pattern code for your own aircraft */
#define	SPCODE_SHIP_U	0x142
#define	SPCODE_SHIP 	0x144
#define	SPCODE_SHIP_L	0x146
#define	SPCODE_SHIP_LL	0x148
#define	SPCODE_NULL		0x14e
#define VX 32			/*  Speed ??of your aircraft */
#define VY 32

#define	SHIP_XMAX	(X_MAX - DOT_RATE * 32 )	/* Movement range of your aircraft */
#define	SHIP_XMIN	(DOT_RATE * 16)
#define	SHIP_YMAX	(Y_MAX)
#define	SHIP_YMIN	(DOT_RATE * 16)

#define	SHIP_DIRMAX	52
#define	SHIP_DIRDIV1	12	/* First pattern */
#define	SHIP_DIRDIV2	36	/* Second pattern */

#define	DEATH_PATNUM	7

/* Death Pattern */
static short death_pattern[ DEATH_PATNUM ] = {
	 0x156 , 0x156, 0x158, 0x15a, 0x15e, 0x15c, 0
} ;

int ship_move( void )
{
	int s;
	short x = ship_data.data.x, y = ship_data.data.y;
	short dir = ship_data.dir;

	if( ship_data.dead_count > 0 ) {
		/* Already dead and exploding */

		if( ship_data.dead_count == 1 ) { /* If an explosion starts, play a sound */
			sd_on( SD_JIKIOUT );
		}
		wait();
		if( ++ship_data.dead_count >= DEATH_PATNUM * 4 ) {
			/* The end of the explosion */
			ship_data.dead_count = 0 ;
			/* Reduce the remaining number of your own ships */
			if( ship_data.left-- <= 0 ) {
				/* Game over */
				return 1;
			}
			/* Send out the next player */
			ship_data.data.x = INIT_X ;
			ship_data.data.y = INIT_Y ;
			ship_data.dead_count = INIT_DEAD_COUNT ;
			return 0;
		}
		ship_data.sp->x = SP_X( x );
		ship_data.sp->y = SP_Y( y );
		ship_data.data.x = x - 8 ;
		s = ship_data.sp->code = death_pattern[ ship_data.dead_count / 4 ] ;
		if( s == 0 ) ship_data.sp->prw = 0 ;
		return 0;
	}

	/* When alive */

	s = get_key();

	if ( ( s & KEY_LEFT ) && ( x > SHIP_XMIN ) ) x -= VX;
	if ( ( s & KEY_RIGHT ) && ( x < SHIP_XMAX ) ) x += VX;

	if ( ( s & KEY_UP ) && ( y > SHIP_YMIN ) ) {
		y -= VY;
		if( dir > -SHIP_DIRMAX ) dir-- ;
	} else if ( ( s & KEY_DOWN ) && ( y < SHIP_YMAX ) ) {
		y += VY;
		if( dir < SHIP_DIRMAX ) dir++ ;
	} else {
		if( dir > 0 ) dir--;
		else	dir++;
	}

	if ( s & KEY_A ) shot_make( x, y, 10 * DOT_RATE, 0, 1 );

	/* Set sprite display data */

	ship_data.sp->x = SP_X( x );
	ship_data.sp->y = SP_Y( y );
	ship_data.data.x = x;
	ship_data.data.y = y;
	ship_data.dir = dir;

	if( ship_data.dead_count < 0 ) {		/* Invincibility after death */
		ship_data.dead_count++ ;
	}
	if( ship_data.dead_count & 1 ) {		/* Flashes when invincible */
			ship_data.sp->prw = 0 ;
	} else {
		ship_data.sp->prw = 1 ;
		/*	Determine the pattern by looking at the direction	*/
		if( dir > SHIP_DIRDIV2 ) ship_data.sp->code = SPCODE_SHIP_LL ;
		else if( dir > SHIP_DIRDIV1 ) ship_data.sp->code = SPCODE_SHIP_L ;
		else if( dir > -SHIP_DIRDIV1 ) ship_data.sp->code = SPCODE_SHIP ;
		else if( dir > -SHIP_DIRDIV2 ) ship_data.sp->code = SPCODE_SHIP_U ;
		else ship_data.sp->code = SPCODE_SHIP_UU ;
	}
	return 0;
}

/*
 *		Moving your own bullet
 */

void shot_move( void )
{
	SHOT *shot;

	for( shot = shot_used; shot; ) {
		short x, y;

		x = ( shot->data.x += shot->data.vx );
		y = ( shot->data.y += shot->data.vy );

		/* Disappears when it goes off screen */
		if ( x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX ) {
			shot = shot_erase( shot );
		} else {
			/* Set sprite display data */
			shot->sp->x = SP_X( x );
			shot->sp->y = SP_Y( y );
			shot = shot->next;
		}
	}
}

/*
 *		Self-fire routine
 */

void shot_make( short x, short y, short vx, short vy, short type )
{
	SHOT *shot;
	SP *sp;

	/* Check if it's available */
	if ( !shot_free ) return;

	/* Allocate sprites */
	if ( !( sp = sprite_alloc( CHR1x1 ) ) ) return;

	/* Find a vacant one */
	shot = shot_free;
	shot_free = shot_free->next;

	/* Sprite Pointer Settings */
	shot->sp = sp;

	/* Add to list of used items */
	if ( shot->next = shot_used ) {
		shot_used->pre = shot;
	}
	shot->pre = 0;
	shot_used = shot;
	sd_on( SD_SHOT );		/* Play a sound */

	shot->type = type;
	shot->data.x = x;
	shot->data.y = y;
	shot->data.rx = DOT_RATE * 10;
	shot->data.ry = DOT_RATE * 2;
	shot->data.vx = vx;
	shot->data.vy = vy;
	shot->data.str = 1;
	shot->data.energy = 1;
	shot->sp->prw = 1;
	shot->sp->code = 0x14a;
	shot->data.palet = 0x100;
}

/*
 *		Own bullet disappearance
 */

SHOT *shot_erase( SHOT *shot )
{
	SHOT *next;

	next = shot->next;
	shot->type = -1;

	/* Remove from the list of uses */
	if ( shot->pre ) {
		shot->pre->next = shot->next;
		if ( shot->next ) shot->next->pre = shot->pre;
	} else {
		if ( shot->next ) shot->next->pre = 0;
		shot_used = shot->next;
	}

	/* Add to vacant list */
	shot->pre = 0;
	shot->next = shot_free;
	shot_free = shot;

	/* Free the sprite */
	sprite_free( shot->sp );

	return next;
}
