/*
 *		Disp.c
 */

#include	<iocslib.h>
#include	<graph.h>
#include	<stdio.h>
#include	<sprite.h>
#include	"bged.h"

unsigned char	bgnum[ 8 ][ 32 ];
static	unsigned char	spbuf[ 256 ] ;

#define	__RGB( r , g , b )	( ( r ) * 64 + ( g ) * 2048 + ( b ) * 2 )

extern	unsigned short	bgbuf[] ;

/*		Mouse cursor (arrow) sprite pattern		*/

unsigned short	arrowpattern1[ 64 ] = {
	0xF000,0x0000,0xFE00,0x0000,0xFEC0,0x0000,0xFCF0,0x0000,
	0xFCCD,0x0000,0xFCCE,0xC000,0xFCFF,0xF000,0xFF00,0x0000,
	0xF000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
} ;

/*		Mouse cursor (box) sprite pattern		*/

unsigned short	arrowpattern2[ 64 ] = {
	0x0FFF,0xFFFF,0xF000,0x0000,0xF000,0x0000,0xF000,0x0000,
	0xF000,0x0000,0xF000,0x0000,0xF000,0x0000,0xF000,0x0000,
	0xF000,0x0000,0x0FFF,0xFFFF,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0xF000,0x0000,0x0F00,0x0000,0x0F00,0x0000,0x0F00,0x0000,
	0x0F00,0x0000,0x0F00,0x0000,0x0F00,0x0000,0x0F00,0x0000,
	0x0F00,0x0000,0xF000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
} ;

#define	COLOR_SEL	250
#define	COLOR_BOX	255
#define	COLOR_CHR	253

/*
 *	Display of status
 */

void	disp_status( int pb , int x , int hinv , int yinv , int mode )
{
	static	char charbuf[ 256 ] ;

	APAGE( 1 ) ;

	fill( 80 , 0 , 199 , 11 , 0 ) ;
	sprintf( charbuf , "pblock %1x   x %03d" , pb , x ) ;
	symbol( 80 , 0 , charbuf , 1 , 1 , 0 , COLOR_CHR  , 0 ) ;

	if( mode == MODE_PUT ) {
		fill( 65 , PT_Y * 16 , 79 , PT_Y * 16 + 15 , COLOR_SEL ) ;
		fill( 65 , CP_Y * 16 , 79 , CP_Y * 16 + 15 , 0 ) ;
	} else {
		fill( 65 , PT_Y * 16 , 79 , PT_Y * 16 + 15 , 0 ) ;
		fill( 65 , CP_Y * 16 , 79 , CP_Y * 16 + 15 , COLOR_SEL ) ;
	}
}

/*
 *	Coordinates display
 */

void	disp_xy( int x, int y)
{
	static	char charbuf[ 256 ] ;

	APAGE( 1 ) ;

	fill( 200 , 0 , 255 , 11 , 0 ) ;
	sprintf( charbuf , "xy %03d,%02d" , x, y ) ;
	symbol( 200 , 0 , charbuf , 1 , 1 , 0 , COLOR_CHR  , 0 ) ;
}
/*
 *	Rectangle drawing routine (not used)
 */

void	drawbox( int x , int y , int length , char *str )
{
	fill(	x * 8 , y * 16 ,
		x * 8 + length * 8 - 1 , y * 16 + 15 , 0 ) ;
	box(	x * 8 , y * 16 ,
		x * 8 + length * 8 - 1 , y * 16 + 15 , 254 , 0xffff ) ;

	symbol( x * 8 + 2 , y * 16 + 2 , str , 1 , 1 , 0 , 255  , 0 ) ;
}

/*
 *	Arrow drawing routine
 */

void	drawarrow( int x , int y , char *cp )
{
	int vx , vy ;
	vx = x * 8 ;
	vy = y * 16 ;
	box( vx , vy , vx + 15 , vy + 15 , COLOR_BOX , 0xffff ) ;

	symbol( vx + 2 , vy + 2 , cp , 1 , 1 , 0 , COLOR_CHR , 0 ) ;
}

void	disp_init()
{
	int	i ;

	CRTMOD( 10 ) ;			/* 256*256 16/256 512 */
	G_CLR_ON() ;

	B_CUROFF() ;
	WINDOW( 0 , 0 , 511 , 511 ) ;	/* Window Settings */

	SP_ON() ;			/* Sprite ON */

	BGCTRLST( 0 , 0 , 1 ) ;		/* Background settings */
	BGCTRLST( 1 , 1 , 1 ) ;
	BGSCRLST( 0 , 0 , 0 ) ;
	BGSCRLST( 1 , 0 , 0 ) ;
	BGTEXTCL( 0 , 0x100 ) ;
	BGTEXTCL( 1 , 0x100 ) ;

	for( i = 0 ; i < 127 ; i++ ) {
		sp_set( i , 0 , 0 , 0 , 0 );
	}

	MS_INIT() ;
	MS_CURST( 128, 128 ) ;
	MS_CURON() ;
	MS_LIMIT( 0 , 0 , 255 , 255 ) ;
	SKEY_MOD( 0 , 0, 0 ) ;

	GPALET( COLOR_SEL , __RGB( 10 , 15 , 26 ) ) ;
	GPALET( COLOR_BOX , __RGB( 31 , 31 , 31 ) ) ;
	GPALET( COLOR_CHR , __RGB( 31 , 15 , 26 ) ) ;

	paletset( 1 ) ;

	VPAGE( 15 ) ;
	HOME( 15 , 0 , 0 ) ;

	patternwrite( 0, 0 );

	drawarrow( 8 , 0 , "s" ) ;
	drawarrow( 8 , 1 , "t" ) ;
	drawarrow( 8 , 2 , "HR" ) ;
	drawarrow( 8 , 3 , "VR" ) ;
	drawarrow( 8 , 4 , "£" ) ;
	drawarrow( 8 , 5 , "¥" ) ;
	drawarrow( 8 , 8 , "st" ) ;
	drawarrow( 8 , 9 , "‚p" ) ;
	drawarrow( 8 , PT_Y , "PT" ) ;
	drawarrow( 8 , CP_Y , "CP" ) ;

	SP_DEFCG( 127 , 1 , arrowpattern1 ) ;
	SP_DEFCG( 126 , 1 , arrowpattern2 ) ;

	APAGE( 1 ) ;
	fill( 0, 0, 80 , 255, 1 ) ;
	GPALET( 1 , 1 ) ;

	drawbg( 0 ) ;
}



void	paletset( int block )
{
	int	i ;
	int	c ;
	for( i = 0 ; i < 16 ; i++ ) {
		c = SPALET( 0x80000000 + i , block , 0 ) ;
		GPALET( i + 16 , c ) ;
		SPALET( 0x80000000 + i , block , c ) ;
	}
}

void	chrselect( char ch , int hinv , int vinv )
{
	int	x , y ;
	struct PSETPTR	p ;

	SP_GTPCG( ch , 0 , spbuf ) ;

	for( y = 0 ; y < 8 ; y++ ) {
		p.x = ( hinv ? 64 + 7 : 64 ) ;
		if( vinv ) p.y = 255 - y ; else p.y = 248 + y ;
		for( x = 0 ; x < 4 ; x++ ) {
			p.color = spbuf[ y * 4 + x ] / 16 + 16 ;
			PSET( &p ) ;
			if( hinv ) p.x-- ; else p.x++ ;
			p.color = spbuf[ y * 4 + x ] % 16 + 16 ;
			PSET( &p ) ;
			if( hinv ) p.x-- ; else p.x++ ;
		}
	}
}

void	patternwrite( int hinv , int vinv )
{
	int	i , j , y , x ;

	APAGE( 0 ) ;
	for( j = 0 ; j < 32 ; j++ ) {
		for( i = 0 ; i < 8 ; i++ ) {
			struct PSETPTR	p ;
			SP_GTPCG( bgnum[ i ][ j ] , 0 , spbuf ) ;

			for( y = 0 ; y < 8 ; y++ ) {

				p.x = i * 8 + ( hinv ? 7 : 0 ) ;
				if( vinv ) p.y = 7 - y + j * 8 ;
				else p.y = y + j * 8 ;

				for( x = 0 ; x < 4 ; x++ ) {
					p.color = spbuf[ y * 4 + x ] / 16 + 16 ;
					PSET( &p ) ;
					if( hinv ) p.x-- ; else p.x++ ;
					p.color = spbuf[ y * 4 + x ] % 16 + 16 ;
					PSET( &p ) ;
					if( hinv ) p.x-- ; else p.x++ ;
				}
			}
		}
	}
}

/*		Display data in background		*/

void	drawbg( int x )
{
	int	i, j;

	for( i = 0; i < DISPLAY_XSIZE ; i++ ){
		for( j = 0; j < DISPLAY_YSIZE; j++ ){
			BGTEXTST( 0, i + BG_X0, j, bgbuf[ j + ( i + x ) * BG_YSIZE ] ) ;
		}
	}

}


/*
 *		Copy Region
 */

extern	region	copy_region ;

void	disp_copy_region( int x0 )
{
	int	x1 , y1 , x2 , y2 ;

	if( copy_region.x1 > copy_region.x2 )
		x1 = copy_region.x2 - x0 , x2 = copy_region.x1 -x0 ;
	else
		x1 = copy_region.x1 -x0, x2 = copy_region.x2-x0 ;
	if( copy_region.y1 > copy_region.y2 )
		y1 = copy_region.y2 , y2 = copy_region.y1 ;
	else
		y1 = copy_region.y1 , y2 = copy_region.y2 ;

	box( x1 * 8 + 80 , y1 * 8 ,
	     x2 * 8 + 80 + 7 , y2 * 8 + 7 ,
	     COLOR_BOX , 0xffff ) ;
}

void	erase_copy_region( int x0 )
{
	int	x1 , y1 , x2 , y2 ;

	if( copy_region.x1 > copy_region.x2 )
		x1 = copy_region.x2 - x0 , x2 = copy_region.x1 -x0 ;
	else
		x1 = copy_region.x1 -x0, x2 = copy_region.x2-x0 ;
	if( copy_region.y1 > copy_region.y2 )
		y1 = copy_region.y2 , y2 = copy_region.y1 ;
	else
		y1 = copy_region.y1 , y2 = copy_region.y2 ;

	box( x1 * 8 + 80 , y1 * 8 ,
	     x2 * 8 + 80 + 7 , y2 * 8 + 7 ,
	     0 , 0xffff ) ;
}


void	make_bgnum( void )
{
	int	x, y;
	for( x = 0 ; x < 4 ; x++ ){
		for( y = 0 ; y < 16 ; y++ ){
			bgnum[ x * 2     ][ y * 2     ] = ( y * 4 + x ) * 4 ;
			bgnum[ x * 2     ][ y * 2 + 1 ] = ( y * 4 + x ) * 4 + 1;
			bgnum[ x * 2 + 1 ][ y * 2     ] = ( y * 4 + x ) * 4 + 2;
			bgnum[ x * 2 + 1 ][ y * 2 + 1 ] = ( y * 4 + x ) * 4 + 3;
		}
	}
}
