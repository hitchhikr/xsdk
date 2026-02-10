/*
 *		bgput.c( BG character & string output routines )
 *
 *				Programmed by chika1993
 */

#include	<iocslib.h>
#include	<stdarg.h>		/* If you remove bgprintf, cut this line too. */


#define	BG_SPACE	0		/* ASCII code 0x20 BG code */

/*
 *	bgputs( page, x, y, page, cp )
 *	Displaying text in the background
 *
 *	int page ;			BG page to display
 *	int x ;				X coordinate
 *	int y ;				Y coordinate
 *	int color ;			Color
 *	char *cp ;			String
 */

void	bgputs( int page, int x, int y, int color, char *cp )
{
	while( *cp != '\0' ) {
		BGTEXTST( page, x++, y, color * 0x100 - 0x20 + BG_SPACE + *cp++ );
	}
}

/*
 *	bgputn( page, x, y, page, num, keta )
 *	Displays num in the background, right-justified, with the number of digits specified by keta.
 *
 *	int page;			BG page to display
 *	int x;				X coordinate
 *	int y;				Y coordinate
 *	int color;			Color
 *	int num;			Number to display
 *	int keta;			Number of digits
 */

void	bgputn( int page, int x, int y, int color, int num, int keta )
{
	keta-- ;
	do {
		BGTEXTST(page, x + keta , y,
		  (num % 10) + 0x10 + BG_SPACE + (color * 256) );
		num /= 10;
	} while( (num > 0) && (--keta >= 0) );

	while( keta-- >= 0 ) {
		BGTEXTST( page, x + keta, y, BG_SPACE + ( color * 256 ) );
	}
}

/*
 *	bgputn0( page, x, y, page, num, keta )
 *	Displays num in the BG with the number of digits specified by keta, justified to the right.
 *  The missing part is filled with 0.
 *
 *	int page;			BG page to display
 *	int x;				X coordinate
 *	int y;				Y coordinate
 *	int color;			Color
 *	int num;			Number to display
 *	int keta;			Number of digits
 */

void	bgputn0( int page, int x, int y, int color, int num, int keta )
{
	keta-- ;
	do {
		BGTEXTST( page, x + keta , y ,
		  ( num % 10 ) + 0x10 + BG_SPACE + ( color * 256 ) );
		num /= 10;
	} while( --keta >= 0 ) ;
}

/*
 *	bgprintf( page, x, y, page, char *fmt, ... )
 *	Printf in BG (?)
 *
 *	int page;			BG page to display
 *	int x;				X coordinate
 *	int y;				Y coordinate
 *	int color;			Color
 *	char *fmt;			A pointer to a format string
 */
