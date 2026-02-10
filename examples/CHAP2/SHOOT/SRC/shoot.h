
#define		DIV_X	16
#define		DIV_Y	16

#define		XMIN	0
#define		XMAX	( 16 * 256 )

#define		YMIN	0
#define		YMAX	( 16 * 256 )

#define		SHOTNUM	16

/*
 *		Structure of the aircraft
 */

typedef	struct	_ship	{
	int	x ;			/* Coordinates */
	int	y ;
	int	vx ;			/* Speeds */
	int	vy ;

	int	dir ;			/* Direction of the player's ship (variable for switching patterns) */

	int	energy ;		/* Energy */

	int	lbutton ;
	int	rbutton ;

	int	atariflg ;		/* 0... No hit detection
	                	   1... Got hit		*/

	unsigned short	spcode ;

}	ship ;

/*
 *		Structure of your own ship's bullets
 */

typedef	struct	_shot	{
	int	condition ;		/* State (0 means it doesn't exist) */
	int	type ;			/* Kinds */
	int	x ;			/* Coordinates */
	int	y ;
	int	vx ;			/* Speeds */
	int	vy ;

	int	energy ;		/* Energy */

	unsigned short	spcode ;

}	shot ;


#define	IS_OUT_OF_DISP( x , y )	\
	( ( x ) < XMIN || ( x ) > XMAX || ( y ) < YMIN || ( y ) > YMAX )
