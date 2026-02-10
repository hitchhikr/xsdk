/*
 *		Enemies moving functions
 */

#include	"sub.h"
#include	"ship.h"
#include	"enemy.h"
#include	"bg.h"
#include	"enemove.h"

extern SHIP ship_data ;

#define __abs( x )	( ( (x) >0 ) ? (x) : -(x) )

#define BOMB_SPEED ( SPEED_RATE * 2 )

#define SCROLL_SPEED ( DOT_RATE / 2 )		/* Speed ??when moving with scrolling */

#define BOMBMAKE_DIR( x, y, dir )	bomb_make( x, y,	\
			VX_DIR(dir, BOMB_SPEED), VY_DIR(dir, BOMB_SPEED), 1 )

#define BOMB_RATIO	120		/* Bullet firing frequency */

/*		Enemy movement sample		*/

int dummy_move( ENEMY *ene )
{
	short x, y, flag = 1;

	ene->count ++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;
	if ( x < X_MIN || x > X_MAX ) {
		ene->data.vx *= -1;
		flag = 0;
	}
	if ( y < Y_MIN || y > Y_MAX ) {
		ene->data.vy *= -1;
		flag = 0;
	}
	if ( flag ) {
		ene->data.x = x;
		ene->data.y = y;
		ene->sp->x = SP_X( x );
		ene->sp->y = SP_Y( y );
		ene->sp->code = ( ene->count / 4 ) % 6 + 0x160 ;
	}

	if ( !( ene->count % 120 ) ) {
		short dir ;
		dir = calc_direction( ene->data.x, ene->data.y,
			 ship_data.data.x, ship_data.data.y );
		bomb_make( ene->data.x, ene->data.y,
			VX_DIR(dir, BOMB_SPEED), VY_DIR(dir, BOMB_SPEED), 1 );
	}

	return TRUE;
}

/*		Small fry formation		*/

int ene1_move( ENEMY *ene )
{
	short	x , y ;

	ene->count ++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;
	if ( x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX ) {
		return FALSE ;
	}
	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ( ene->count / 4 ) % 6 + 0x160 ;

	/* Wrapping process */

	switch( ene->state ) {
	  case 0:
		if( x < 144 * DOT_RATE ) {
			ene->state = 1 ;
			ene->data.vx = -ene->data.vx ;
			ene->data.vy = ( y > 144 * DOT_RATE ) ? DOT_RATE : -DOT_RATE ;
		}
		break ;
	  case 1:
		if( x > 192 * DOT_RATE || y < 32 * DOT_RATE || y > 224 * DOT_RATE ) {
			ene->state = 2 ;
			ene->data.vx = -ene->data.vx ;
			ene->data.vy = 0 ;
		}
	}

	return TRUE ;
}


/*		Enemies chasing your ship		*/

#define	ENE2_SPEED	( SPEED_RATE * 3 / 2 )

int ene2_move( ENEMY *ene )
{
	static unsigned short ene2_spcode[ 16 ] = {
		0x174, 0x175, 0x176, 0x177, 0xc170, 0xc171, 0xc172, 0xc173,
		0xc174, 0xc175, 0xc176, 0xc177, 0x170, 0x171, 0x172, 0x173,
	};

	short x, y, tmp;

	ene->count++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;
	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ene2_spcode[ ene->dir ];

	if (!(ene->count % 16)) {
		tmp = calc_direction( ene->data.x, ene->data.y,
			 ship_data.data.x, ship_data.data.y );

		tmp = (tmp - ene->dir) & (DIRECTION_MAX - 1);
		if (tmp) {
			if (tmp > 7) ene->dir-- ;
			else ene->dir++ ;
			ene->dir &= DIRECTION_MAX - 1;
		}
		ene->data.vx = VX_DIR( ene->dir , ENE2_SPEED );
		ene->data.vy = VY_DIR( ene->dir , ENE2_SPEED );
	}

	return TRUE ;
}


/*		Wobbling small fry		*/

int ene3_move( ENEMY *ene )
{
	short	x , y ;

	ene->count ++;
	if( ene->flg ) {
		ene->data.vy = -SIN( ene->count ) / ( SINTABLE_MAX / 8 ) ;
	} else {
		ene->data.vy = SIN( ene->count ) / ( SINTABLE_MAX / 8 ) ;
	}
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;
	if ( x < X_MIN ) {
		return FALSE ;
	}
	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ( ene->count / 4 ) % 6 + 0x166 ;

	if (!(ene->count % BOMB_RATIO) ) {
		BOMBMAKE_DIR( x, y, calc_direction(x, y, ship_data.data.x, ship_data.data.y) );
	}
	return TRUE;
}


/*		Ground-based artillery		*/

#define ENE4_BOMB_RATIO	48		/* Bullet firing frequency */

int ene4_move( ENEMY *ene )
{
	short x, y, tmp;
	static unsigned short ene4_spcode[ 16 ] = {
		0x4378, 0x4378, 0x4378, 0x4378, 0x378, 0x378, 0x378, 0x378,
		0x378, 0x379, 0x37a, 0x37b, 0x437c, 0x437b, 0x437a, 0x4379,
	};
	static unsigned short ene4_spcode_r[ 16 ] = {
		0xc379, 0xc37a, 0xc37b, 0xc37c, 0x837b, 0x837a, 0x8379, 0x8378,
		0x8378, 0x8378, 0x8378, 0x8378, 0xc378, 0xc378, 0xc378, 0xc378,
	};

	ene->count ++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y ;
	if (x < X_MIN) return FALSE ;

	if (!(ene->count % ENE4_BOMB_RATIO)) BOMBMAKE_DIR( x, y, ene->dir );

	tmp = calc_direction(x, y, ship_data.data.x, ship_data.data.y);
	if( !ene->flg ) {
		if (tmp < 4 && tmp != 0) {
			tmp = 0;	ene->count = 0;
		} else if ( tmp < 8 ) {
			tmp = 8;	ene->count = 0;
		}
		if( tmp != ene->dir ) {
			ene->count = 0;
			ene->dir = tmp;
		}
		ene->sp->code = ene4_spcode[ ene->dir ];
	} else {
		if (tmp > 12) {
			tmp = 0;	ene->count = 0;
		} else if ( tmp > 8 ) {
			tmp = 8;	ene->count = 0;
		}
		if( tmp != ene->dir ) {
			ene->count = 0;
			ene->dir = tmp;
		}
		ene->sp->code = ene4_spcode_r[ ene->dir ];
	}

	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );

	return TRUE;
}



/*		Ground hatch		*/

#define SPCODE_ENE5_CLOSE 0x16c
#define SPCODE_ENE5_OPEN 0x16e
#define ZAKO_TYPE 6
#define	ENE6_SPEED 32

int ene5_move( ENEMY *ene )
{
	short x, y;

	ene->count ++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y ;
	if (x < X_MIN) return FALSE ;

	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );

	if (ene->count > 96 && ene->count < 192) {
		if (ene->flg) ene->sp->code = SPCODE_ENE5_OPEN | 0x8000;
		else ene->sp->code = SPCODE_ENE5_OPEN;

		if (!(ene->count & 15)) {
			ENEMY *tmp;
			tmp = enemy_alloc( eneinfo[ZAKO_TYPE].spsize );
			if (!tmp) return TRUE;

			tmp->type = ZAKO_TYPE;
			tmp->data.x = x;
			tmp->data.y = y;

			tmp->data.rx = eneinfo[ZAKO_TYPE].rx;
			tmp->data.ry = eneinfo[ZAKO_TYPE].ry;
			tmp->data.str = eneinfo[ZAKO_TYPE].str;
			tmp->data.energy = eneinfo[ZAKO_TYPE].energy;

			tmp->count = tmp->state = 0;
			tmp->data.vx = -8;
			tmp->data.vy = (ene->flg ? ENE6_SPEED : -ENE6_SPEED) ;
			tmp->move = ene6_move;
			tmp->sp->prw = 1;
			tmp->flash_count = 0;

		}

	} else {
		if (!ene->flg) ene->sp->code = SPCODE_ENE5_CLOSE;
		else ene->sp->code = SPCODE_ENE5_CLOSE | 0x8000;
	}

	return TRUE;
}


/*		Small fish coming out of the hatch		*/

#define ENE6_RADIUS 32		/* Reaction distance */

int ene6_move( ENEMY *ene )
{
	short	x , y ;

	ene->count ++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;
	if ( x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX ) {
		return FALSE ;
	}
	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ( ene->count / 4 ) % 6 + 0x160 ;

	/* Wrapping process */

	if( !ene->state ) {
		if( __abs( y - ship_data.data.y ) <= ENE6_RADIUS ) {
			ene->state = 1;
			ene->data.vy = 0;
			if( x < ship_data.data.x ) ene->data.vx = ENE6_SPEED ;
			else ene->data.vx = -ENE6_SPEED ;
		}
	}

	return TRUE ;
}

/*		Mobile battery		*/

#define ENE7_TURN_COUNT 32
#define ENE7_SPEED	( SCROLL_SPEED )

int ene7_move( ENEMY *ene )
{
	short	x , y , flag;

	ene->count ++;
	x = ene->data.x + ene->data.vx - ENE7_SPEED;
	y = ene->data.y + ene->data.vy;
	if ( x < X_MIN || x > X_MAX || y < Y_MIN || y > Y_MAX ) {
		return FALSE ;
	}
	/* Wrapping process */

	if (!(ene->count % ENE7_TURN_COUNT)) {
		ene->data.vx = -ene->data.vx;
	}
	if (bgatari(x, y)) {
		ene->data.vx = -ene->data.vx;
		flag = 1;
	}

	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ene->flg ? 0x817d : 0x17d;

	if (!(ene->count % BOMB_RATIO) ) {
		BOMBMAKE_DIR( x, y, calc_direction(x, y, ship_data.data.x, ship_data.data.y) );
	}

	return TRUE ;
}

/*		Wobbling small fry		*/

int ene8_move( ENEMY *ene )
{
	short	x , y ;

	ene->count ++;
	if( ene->flg ) {
		ene->data.vy = -SIN( (ene->count + 32) & 127 ) / ( SINTABLE_MAX / 32 ) ;
	} else {
		ene->data.vy = SIN( (ene->count + 32) & 127 ) / ( SINTABLE_MAX / 32 ) ;
	}
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;
	if ( x < X_MIN ) {
		return FALSE ;
	}
	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ( ene->count / 4 ) % 6 + 0x166 ;

	if (!(ene->count % BOMB_RATIO) ) {
		BOMBMAKE_DIR( x, y, calc_direction(x, y, ship_data.data.x, ship_data.data.y) );
	}
	return TRUE;
}

/*		Mobile battery B		*/

#define ENE9_TURN_COUNT 64
#define ENE9_SPEED	( SCROLL_SPEED )
#define ENE9_VX		DOT_RATE * 2

int ene9_move( ENEMY *ene )
{
	short	x, y, tmp;

	x = ene->data.x + ene->data.vx - ENE9_SPEED;
	y = ene->data.y + ene->data.vy;
	if ( y < Y_MIN || y > Y_MAX ) {
		return FALSE;
	}

	/* Wrapping process */

	if (ene->count) {
		if (ene->count++ > ENE9_TURN_COUNT) ene->count = 0;
		if (ene->count == ENE9_TURN_COUNT / 2)
			BOMBMAKE_DIR( x, y,ene->state);
		ene->data.vx = 0;
	} else {
		tmp = calc_direction(x, y, ship_data.data.x, ship_data.data.y);
		if (ship_data.data.x < X_MAX / 2) {		/* Your ship is on the left half */
			switch( tmp ){
			  case 9: case 8: case 7:
				ene->data.vx = -ENE9_VX;
				break;
			  case 10: case 6:
				ene->count ++;
				ene->state = tmp;
				break;
			  default:
				ene->data.vx = ENE9_VX;
				break;
			}
		} else {
			switch( tmp ){
			  case 15: case 0: case 1:
				ene->data.vx = ENE9_VX;
				break;
			  case 14: case 2:
				ene->count ++;
				ene->state = tmp;
				break;
			  default:
				ene->data.vx = -ENE9_VX;
				break;
			}
		}
	}
	ene->data.vy = 0;
	if (!bgatari(x, y + (ene->flg ? -DOT_RATE*12: DOT_RATE*12) )) {	/* Drop down */
		ene->count = 0;
		ene->data.vy = ene->flg ? -DOT_RATE*4: DOT_RATE*4;
	}
	if (bgatari(x, y + (ene->flg ? -DOT_RATE*8: DOT_RATE*8) )) {
		ene->count = 0;
		ene->data.vy += ene->flg ? DOT_RATE*2: -DOT_RATE*2;
	}

	ene->data.y = y;
	ene->data.x = x;

	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );
	ene->sp->code = ene->flg ? 0x817d : 0x17d;

	return TRUE ;
}


/*		Boss		*/

#define	ENE10_VYMAX	(DOT_RATE * 2) 
#define	ENE10_VYMIN	(DOT_RATE / 2) 
#define	ENE10_VVY	(DOT_RATE / 8) 
#define	ENE10_RADIUS	(DOT_RATE * 32 )
#define	ENE10_BOMB_RATIO	128
#define	ENE10_SPCODENUM	8
#define	ENE10_SUKITIME	16

int ene10_move( ENEMY *ene )
{
	static	short	ene10_spcode[ 8 ] = {
		0x110, 0x119, 0x122, 0x12b, 0x134, 0x160, 0x169, 0x172 };
	short	x , y , tmp;

	ene->count ++;
	x = ene->data.x + ene->data.vx;
	y = ene->data.y + ene->data.vy;

	if (!ene->state) {				/* Straight ahead mode */
		ene->atari = 1;
		ene->count = 0;
		ene->sp->code = ene10_spcode[ 0 ];
		if (x < (192 * DOT_RATE)) {
			ene->state = 1 ;
			ene->data.vx = 0;
		}
	} else {					/* Chase Mode */
		tmp = __abs(y - ship_data.data.y);
		if (  y > ship_data.data.y ) {
			if (tmp > ENE10_RADIUS) {
				if (ene->data.vy > -ENE10_VYMAX) ene->data.vy -= ENE10_VVY;
			} else {
				if (ene->data.vy < -ENE10_VYMIN) ene->data.vy += ENE10_VVY ;
				else ene->data.vy -= ENE10_VVY ;
			}
		} else {
			if (tmp > ENE10_RADIUS) {
				if (ene->data.vy < ENE10_VYMAX) ene->data.vy += ENE10_VVY;
			} else {
				if (ene->data.vy > ENE10_VYMIN) ene->data.vy -= ENE10_VVY ;
				else ene->data.vy += ENE10_VVY ;
			}
		}
							/* Opening and firing */
		tmp = ene->count % ENE10_BOMB_RATIO ;
		if( tmp < ENE10_SPCODENUM ) {
			ene->atari = 0;
			ene->sp->code = ene10_spcode[ tmp ];
		} else if( tmp < ENE10_SPCODENUM + ENE10_SUKITIME ) {
			ene->atari = 0;
			ene->sp->code = ene10_spcode[ ENE10_SPCODENUM - 1 ];
			if (!( tmp & 3 ) ) {
				int	vy = ( tmp - ENE10_SPCODENUM - 2) * 4;
				bomb_make(x, y - DOT_RATE * 10, -DOT_RATE*3,-vy, 1);
				bomb_make(x, y + DOT_RATE * 10, -DOT_RATE*3, vy, 1);
			}
		} else if( tmp < ENE10_SPCODENUM * 2 + ENE10_SUKITIME ) {
			ene->atari = 0;
			ene->sp->code =
			  ene10_spcode[ ENE10_SPCODENUM * 2 + ENE10_SUKITIME - 1 - tmp ];
		} else {
			ene->atari = 1;
			ene->sp->code = ene10_spcode[ 0 ];
			if (tmp == 64 || tmp == 0) {
				bomb_make(x, y , -DOT_RATE*3, 0, 1);
				bomb_make(x, y - DOT_RATE * 7, -DOT_RATE*11/4, -8, 1);
				bomb_make(x, y + DOT_RATE * 7, -DOT_RATE*11/4, +8, 1);
			}
		}

	}

	ene->data.x = x;
	ene->data.y = y;
	ene->sp->x = SP_X( x );
	ene->sp->y = SP_Y( y );

	return TRUE ;
}
