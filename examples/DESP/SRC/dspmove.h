/*
 *			Dspmove.h
 */

#define	RIGHT			8
#define	LEFT			4
#define	UP			1
#define	DOWN			2
#define	RIGHT_TRIGGER		32
#define	LEFT_TRIGGER		64

#define	RIGHT_TRIGGER_FLAG		10
#define	LEFT_TRIGGER_FLAG		11
#define	NOT_PRESSED		0
#define	PRESSED		1


short	JOY( int mode ) ;
int	dspmove( DSP *dspdata , SHOT *shotdata , int invflg ) ;
void	shotmove( DSP *dspdata , SHOT *shotdata ) ;
int	DspInit( DSP *dspdata , SHOT *shotdata ) ;
