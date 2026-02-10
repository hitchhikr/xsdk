/*
 *				Dsp.h
 */

#ifndef __SPS__
#include	"sps.h"
#endif

#define	SP_METER_X	23	/* Speedometer center coordinates */
#define	SP_METER_Y	25

#define	XMIN		0
#define	XMAX		65536
#define	YMIN		0
#define	YMAX		9111

#define	dVX		4	/* Sokudo's change of rate */
#define	dVY		16

#define	DIV_X		32
#define	DIV_Y		32

#define	DSP_NUM		7
#define	SHOT_NUM	8
#define	ENE_NUM		16
#define	BOMB_NUM	16

#define	MOTIONMAX	15		//This shouldn't be necessary
#define	MOTIONCNTMAX	256		//This shouldn't be necessary

/*
 *	Define sprite data
 */

#define	PAT_NUM		20
#define	dsp_pat_buff	( pat_buff[ 0 ] )
#define	crash_pat_buff	( pat_buff[ 1 ] )
#define	SHADOWNUM	7	/* Shadow Data */
#define	SHADOWY		9320
#define	SPNULL		0x0

/*
 *	Game Modes
 */

#define	PLAY_DEMO	2	/* Playback based on JOYMEMDATA */
#define	PLAY_RECORD	1	/* Record your gameplay in JOYMEMDATA */
#define	PLAY_NORMAL	0	/* Normal */

/*
 *	On-screen coordinate calculation
 *	dx, dy ..... virtual coordinates of the top left corner of the screen
 */

#define	SCX4x4( x , dx )		( (short)( ( x ) - ( dx ) ) / DIV_X )
#define	SCY4x4( y , dy )		( ( ( y ) - ( dy ) ) / DIV_Y )

#define	SCX1x1( x , dx )		( (short)( ( x ) - ( dx ) ) / DIV_X )
#define	SCY1x1( y , dy )		( ( ( y ) - ( dy ) ) / DIV_Y )

/*
 *	Motion file & Spr file address calculation
 */

#define	SPRADR( buffadr , number )	( ( unsigned char * )( buffadr ) + *(int *)( ( short * )( buffadr )+( number ) * 4 ) )
#define	MOTION( mfadr , mt , mc )	*( ( short * )mfadr +  *( int * )( ( char * )mfadr + 8 * mt ) / 2 + mc * 4 )

/*	My ship's structure (not done properly yet)		*/

typedef	struct	DSP	{
	short		x ;
	short		y ;
	short		vx ;
	short		vy ;
	short		dir ;		/* Direction */

	short		MotionType ;
	short		MotionCnt ;
	short		motion1 ;	/* Posture */

	short		energy ;
	short		hit ;

	SPS  		*sp ;
	unsigned short	palet ;
	SPS  		*shadow_sp ;

	char		lbutton ;
	char		rbutton ;

	short atariflg ;		/* 0...No hit detection
	                		   1...Got hit		*/

}	DSP ;

/*	Self-bullet structure (not yet properly done)		*/

typedef	struct	SHOT	{

	short		condition ;
	short		type ;
	short		x ;
	short		y ;
	short		vx ;
	short		vy ;

	SPS  		*sp ;
	unsigned short	palet ;

}	SHOT ;

void	dsperror(char *errmsg);
void	dspexit(void);

