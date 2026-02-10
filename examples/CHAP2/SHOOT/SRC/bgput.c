/*
 *		bgput.c( BG character & string output routines )
 */

#include	<iocslib.h>
#include	"bg.h"


#define	BG_SPACE	0		/* ASCII code 0x20 BG code */
#define	STATUS_BG_PAGE	0
#define	SCORE_X		1		/* SCORE display position */
#define	SCORE_Y		0
#define	ZANKI_X		15		/* Remaining lives display position */
#define	ZANKI_Y		0


/* Macro to display one character on the BG surface */
#define	bgputc( page, x, y, color, char )				\
		*(volatile unsigned short *)( ((page) ? 0xebe000 : 0xebc000)	\
			+ (y) * 128 + (x) * 2 ) = (color) * 256 + (char)

/*
 *	bgputs( page, x, y, color, cp )
 *	Display text in the background
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
		bgputc( page, x++, y, color, -0x20 + BG_SPACE + *cp++ );
	}
}

/*
 *	bgputn( page, x, y, page, num, keta )
 *	Displays num in the background, right-justified,
 *  with the number of digits specified by keta.
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
		bgputc( page, x + keta, y, color,
				(num % 10) + 0x10 + BG_SPACE );
		num /= 10;
	} while( (num > 0) && (--keta >= 0) );

	while( keta-- >= 0 ) {
		bgputc( page, x + keta, y, color, BG_SPACE );
	}
}

/*
 *	bgputn0( page, x, y, color, num, keta )
 *	Displays num in the BG with the number of digits specified by keta, right-justified.
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
		bgputc( page, x + keta, y, color,
			( num % 10 ) + 0x10 + BG_SPACE );
		num /= 10;
	} while( --keta >= 0 ) ;
}

/*
 *	Display the score and remaining lives
 */

void	score_disp( int score , int left )
{
	bgputs( STATUS_BG_PAGE , SCORE_X   , SCORE_Y , 1 , "SCORE " ) ;
	bgputn( STATUS_BG_PAGE , SCORE_X+6 , SCORE_Y , 1 , score , 6 ) ;

	bgputs( STATUS_BG_PAGE , ZANKI_X   , ZANKI_Y , 1 , "LEFT " ) ;
	bgputn( STATUS_BG_PAGE , ZANKI_X+5 , ZANKI_Y , 1 , left , 2 ) ;

}
