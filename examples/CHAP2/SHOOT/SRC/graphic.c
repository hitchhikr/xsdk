/*
 *		graphic screen draw & move routine for H-Scroll Shooting
 *
 *				programmed by chika1993
 */

#include	<stdlib.h>
#include	<iocslib.h>

#define	__rgb(r, g, b)	((((((g) * 32) + (r)) * 32) + (b)) * 2)
#define	__rgb_r( c )	( ( (c) / 64 ) & 31 )
#define	__rgb_b( c )	( ( (c) / 2 ) & 31 )
#define	__rgb_g( c )	( ( (c) / 2048 ) & 31 )

#define	GPAGENUM	4		/* Number of pages on the graphic screen */

#define STARNUM 	512		/* Number of stars */
#define STARPALETNUM	64		/* Number of star palettes(?) */

static	short	scroll_x = 0;
static	unsigned short	starpalet[ STARPALETNUM ][ 16 ];
void	starwrite();
void	paletset();

void	graphic_init( void )
{
	VPAGE( 0 );
	WINDOW( 0, 0, 511, 511 );	/* Window Settings */
	paletset();
	starwrite();			/* Draw stars */
	VPAGE( 15 );			/* Specify the graphic plane to display */
}

/*		Display processing during the game		*/

void	graphic_move()
{
	static	int	cou = 0;
	int	i;

	scroll_x++;

	/*	Scroll the graphic surface		*/
	/*	You have to write & 511 properly.	*/

	HOME( 1, ( scroll_x ) & 511, 0 );
	HOME( 2, ( scroll_x / 2 ) & 511, 0 );
	HOME( 4, ( scroll_x / 4 ) & 511, 0 );
	HOME( 8, ( scroll_x / 8 ) & 511, 0 );

	/*	Star Palette Burning		*/

	cou = ( cou + 1 ) & ( STARPALETNUM - 1 );

	for (i = 0; i < 16; i++) {
		GPALET(i, starpalet[ cou ][ i ]);
	}

}


/*
 *		Draw stars
 */

void	starwrite()
{
	int	i, page;
	struct	PSETPTR psetp;

	for (page = 0; page < GPAGENUM; page++) {
		APAGE(page);				/* Specify the graphics surface to draw on */
		for (i = 0; i < STARNUM; i++) {
			psetp.x = rand() & 511;
			psetp.y = rand() & 511;
			psetp.color = rand() & 15;
			PSET( &psetp );		/* Draw a dot */
		}
	}
}


/*
 *		Palette Settings
 */

void	paletset()
{
	int i, j, cont;

	static unsigned short starpaletorg[ 16 ] = {
		__rgb(  0,  0,  0 ), __rgb(  4,  7, 10 ),
		__rgb( 15, 20, 31 ), __rgb(  8, 10, 15 ),
		__rgb( 20, 24, 31 ), __rgb( 10, 15, 24 ),
		__rgb( 15, 20, 31 ), __rgb(  8, 10, 15 ),
		__rgb(  0,  0,  0 ), __rgb(  4,  7, 10 ),
		__rgb( 20, 24, 31 ), __rgb( 10, 15, 24 ),
		__rgb( 31, 31, 31 ), __rgb( 25, 18, 31 ),
		__rgb( 30, 30, 10 ), __rgb( 31, 20, 10 ),
	};

	for (i = 0; i < 16; i++) {
		int	r,g,b,c,p;
		GPALET(i, c = starpaletorg[ i ]) ;
		p = rand();
		for (j = 0; j < STARPALETNUM; j++) {

			p = ( p + 1 ) & ( STARPALETNUM - 1 );
			if (p < 33) cont = p;
			else cont = 64 - p;

			r = __rgb_r( c ) - ( 32 - cont ) ;
			g = __rgb_g( c ) - ( 32 - cont ) ;
			b = __rgb_b( c ) - ( 32 - cont ) ;
			r = r > 0 ? r : 0;
			g = g > 0 ? g : 0;
			b = b > 0 ? b : 0;
			starpalet[ j ][ i ] = __rgb( r, g, b );
		}
	}
}
