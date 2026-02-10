
#ifndef __SPS__
#include	"sps.h"
#endif

typedef struct BOMB {
	short type ;

	short x , y ;
	short vx , vy ;
	short ex , ey ;			// For example, it is used as the center coordinate when you want to make a circular movement.
	short energy ;
	short hit ;

	SPS *sp ;
	unsigned short palet ;
	SPS *shadow_sp ;

	short state ;

	short dummy1 ;
} BOMB ;

typedef struct ENEMY {
	short type ;

	short x , y ;
	short vx , vy ;
	short energy ;

	short hit ;

	SPS *sp ;			/* Character sps */
	unsigned short palet ;
	SPS *shadow_sp ;		/* Shadow sps */

	short MotionType ;		/* ( MotionType = 99 ) <=> When appearing  */
	                		/* ( MotionType = -1 ) <=> No Motion */
	short MotionCnt ;
	short LastMotion ;		/* Chokzen motion */

	void ( * MoveFnc )( struct ENEMY *data , short x , short y ) ;

	short state ;
	short atariflg ;		/* 0...No hit detection
	                		   1...Got hit		*/

	short bombcnt ;			/* Time until the next missile can be launched ||
                               Counter that is being accumulated while planning to launch a missile */

	short ex , ey ;			// For example, it is used as the center coordinate when you want to make a circular movement.

	short count_ ;			// Time since appearance
	short dummy1 ;

} ENEMY ;



typedef	struct ENEPATTERN {
	short	count_ ;
	short	type ;
	short	x ;
	short	y ;
	short	vx ;		/* Use when the enemy has a set direction */
	short	dummy1 ;
	short	dummy2 ;
	short	dummy3 ;
} ENEPATTERN ;



#define   ENE_TYPE( buff )   (buff).type
#define   ENE_ENERGY( buff )   (buff).energy
#define   ENE_PALET( buff )   (buff).palet

#define   ENE_X( buff )   (buff).x
#define   ENE_Y( buff )   (buff).y
#define   ENE_VX( buff )   (buff).vx
#define   ENE_VY( buff )   (buff).vy
#define   ENE_EX( buff )   (buff).ex
#define   ENE_EY( buff )   (buff).ey
#define   ENE_MT( buff )   (buff).MotionType
#define   ENE_MC( buff )   (buff).MotionCnt
#define   ENE_STATE( buff )   (buff).state

#define   ENE_DUMMY1( buff )   (buff).dummy1
#define   ENE_DUMMY2( buff )   (buff).dummy2

#define   ENE_MOVE( buffPtr , scx , scy ) \
		     ( *( (buffPtr)->MoveFnc ) )( (buffPtr) , (scx) , (scy) )
#define   IS_ENE_MOVE( buff )   (buff).MoveFnc
#define   ENE_BOMBCNT( buff )   (buff).bombcnt


#define   ENE_SP( buff )   (buff).sp
#define   ENE_SP_CHR( buff )   ( ( ENE_SP(buff) )->chr )
#define   ENE_SP_NUM( buff )   ( ( ENE_SP(buff) )->num )
#define   ENE_SP_X( buff )   ( ( ENE_SP(buff) )->x )
#define   ENE_SP_Y( buff )   ( ( ENE_SP(buff) )->y )
#define   ENE_SP_COLOR( buff )   ( ( ENE_SP(buff) )->palet )

#define   ENE_SHADOW( buff )   (buff).shadow_sp
#define   ENE_SHADOW_NUM( buff )   ( ( ENE_SHADOW(buff) )->num )
#define   ENE_SHADOW_X( buff )   ( ( ENE_SHADOW(buff) )->x )
#define   ENE_SHADOW_Y( buff )   ( ( ENE_SHADOW(buff) )->y )
#define   ENE_SHADOW_COLOR( buff )   ( ( ENE_SHADOW(buff) )->palet )



#define   ENE_NUM   16
#define   BOMB_NUM   16

#define	AppearMotionType	99

#define	CRASH_TYPE	1
#define	AO_TYPE		2
#define	TARAI_TYPE	3
#define	IRON_TYPE	4
#define	BIRD_TYPE	5
#define	HOBER_TYPE	6
#define	HEX_TYPE	7
#define	SCO_TYPE	8
#define	KAME_TYPE	9
#define	EF_TYPE		10
#define	BEE_TYPE	11

#define	ROUNDMAX	16
#define	TEKIMAX		64		//Maximum number of enemies that appear on one stage (including tub)
#define	ENEMYLEFTMAX	32		//Number of kills

void Enemy( ENEMY * , BOMB * , int ) ;
short EnemyInit( short , ENEMY * , short , BOMB * ) ;
short EnemyBorn( ENEMY * , short , short , int ) ;
void EnemyKill( ENEMY * , short , short ) ;
void pattcntinit( int , int ) ;
int makebomb( short x , short y , short type ) ;
int makebomb2( short x , short y , short type , short dir ) ;
void Bomb( BOMB *bomb_dat ) ;
int CalcDeg( short x1 , short y1 , short x2 , short y2 ) ;

#define	ENE_YMIN	1344
#define	ENE_YMAX	8576

/*
 *	Match the type as closely as possible to the i in patt_buff[ i ].
 *
 * So far:
 *
 * 0...Dsp
 * 1...Explosion
 * 2...Blue Grasshopper
 * 3...Bathtub
 * 4...Iron
 * 5...Bird
 * 6...Hober
 * 7...HEX
 * 8...Scorpion
 * 9...KAME
 * 10...EFighter
 * 11...Bee
 *
 */

