/*
 *		Display.c		for pic2sp.c
 *				Programmed by Momo May 1992
 */

#include	<stdio.h>
#include	<basic.h>
#include	<graph.h>
#include	<basic0.h>
#include	<string.h>
#include	<sprite.h>
#include	"display.h"

extern	unsigned short	*picdata ;		/* Pic file reading buffer */
extern	char ofn[] ;

const	unsigned short	liner[ 16 ] = {	0x7fff , 0x3fff , 0x1fff , 0x0fff ,
					0x07ff , 0x03ff , 0x01ff , 0x00ff ,
					0x007f , 0x003f , 0x001f , 0x000f ,
					0x0007 , 0x0003 , 0x0001 , 0x0000 } ;
const	unsigned short	linel[ 16 ] = {	0x0000 , 0x8000 , 0xc000 , 0xe000 ,
					0xf000 , 0xf800 , 0xfc00 , 0xfe00 ,
					0xff00 , 0xff80 , 0xffc0 , 0xffe0 ,
					0xfff0 , 0xfff8 , 0xfffc , 0xfffe } ;


/*
 *	Rectangle drawing routine
 */

void	momobox( int x , int y , int length , char *str )
{
	fill(	x * 8 , y * 16 ,
		x * 8 + length * 8 - 1 , y * 16 + 15 , 0 ) ;
	box(	x * 8 , y * 16 ,
		x * 8 + length * 8 - 1 , y * 16 + 15 , RGB( 10 , 20 , 31 ) , 0xffff ) ;

	symbol( x * 8 + 2 , y * 16 + 2 , str , 1 , 1 , 0 , 65535  , 0 ) ;
}

/*
 *	Arrow drawing routine
 */
void	momoarrow( int x , int y , int direction )
{
	int vx , vy ;
	vx = x * 8 ;
	vy = y * 16 ;
	box( vx , vy , vx + 15 , vy + 15 , 0xffff , 0xffff ) ;

	if ( direction ) {
		symbol( vx + 2 , vy + 2 , "Åt" , 1 , 1 , 0 , RGB( 31 , 10 , 20 ) , 0 ) ;
	} else {
		symbol( vx + 2 , vy + 2 , "Ås" , 1 , 1 , 0 , RGB( 31 , 10 , 20 ) , 0 ) ;
	}
}


/*
 *	Screen Scratching Routine
 */

void	disp()
{
	int	x , y ;

	screen( 1 , 3 , 1 , 1 ) ;
	console( 0 , 32 , 0 ) ;

	srcbox( 256 , 3 ) ;
	dstbox( 64 , 3 ) ;
//	line( 256 , 256 , 511 , 256 , 0xffff , 0xffff ) ;

	symbol( 258 , 258 , "Palette table" , 1 , 1 , 0 , 65535  , 0 ) ;

	for( y = 1 ; y < 16 ; y++ ) {
		for( x = 0 ; x < 16 ; x++ ) {
			fill( x * 16 + 258 , y * 16 + 258 ,
			      x * 16 + 270 , y * 16 + 270 ,
			      sp_color( x , 'NASI' , y )
			) ;
			box( 241 , y * 16 + 257 , 254 , y * 16 + 270 , 65535 , 65535 ) ;
		}
	}

	momobox( X1 , FILENAME_Y  , WIDE , "    Filename  " ) ;
	momobox( X1 , SRCDOTS_Y   , WIDE , " src: 256 * 256 " ) ;
	momobox( X1 , OBJDOTS_Y   , WIDE , " obj:  64 *  64 " ) ;
	momobox( X1 , TRANS_Y     , WIDE , "        ->    " ) ;
	momobox( X1 , PATSAVE_Y   , WIDE , "    Patt. Save" ) ;
	momobox( X1 , PALETMAKE_Y , WIDE , "   Make Palette" ) ;
	momobox( X1 , AUTO_Y      , WIDE , "       Auto   " ) ;

	symbol( X1 * 8 - 14 , OBJFILE_Y * 16 - 14 , "obj Filename" , 1 , 1 , 0 , 65535  , 0 ) ;
	momobox( X1 , OBJFILE_Y   , WIDE , ofn ) ;

	momoarrow( SL_X , FILENAME_Y , 0 ) ;
	momoarrow( SR_X , FILENAME_Y , 1 ) ;
	momoarrow( SL_X , SRCDOTS_Y , 0 ) ;
	momoarrow( SR_X , SRCDOTS_Y , 1 ) ;
	momoarrow( SL_X , OBJDOTS_Y , 0 ) ;
	momoarrow( SR_X , OBJDOTS_Y , 1 ) ;

}


/*
 *	PIC file reading routine
 */


int	picread( char *fname )
{
	FILE	*fp ;
	char	sbuf[ 64 ] ;

	unsigned short	*src;
    volatile unsigned short	*dst;
	unsigned short	x = 0 , y = 0 ;
	unsigned short	col , len ;

	if( ( fp = fopen( fname , "rb" ) ) == NULL ) {
		return 1 ;
	}

	sprintf( sbuf , " %14.14s " , fname ) ;
	momobox( X1 , FILENAME_Y , WIDE , sbuf ) ;

	while( *( unsigned char * ) picdata != 0x1a ) {
		fread( picdata , sizeof( char ) , 1 , fp ) ;
	}
	fread( picdata , sizeof( short ) , 131072 , fp ) ;
	fclose( fp ) ;

	src = picdata + 2 ;
	dst = (volatile unsigned short * ) 0xc00000 ;

	while( y <= 255 ) {
		col = *( src ++ ) ;
		len = *( src ++ ) ;

		while( len ) {
			*( dst ++ ) = col ;
			x ++ ;
			len -- ;
			if ( x > 255 ) {
				y ++ ;
				x = 0 ;
				dst = (volatile unsigned short *) 0xc00000 + y * 512 ;
			}
		}
	}
	return 0 ;
}

void	pr_objdots( int dots )
{
	char	sbuf[ 64 ] ;
	sprintf( sbuf , " obj: %3d * %3d" , dots , dots ) ;
	momobox( X1 , OBJDOTS_Y   , WIDE , sbuf ) ;
}

void	pr_srcdots( int dots )
{
	char	sbuf[ 64 ] ;
	sprintf( sbuf , " src: %3d * %3d" , dots , dots ) ;
	momobox( X1 , SRCDOTS_Y   , WIDE , sbuf ) ;
}


/*
	Text box routine
*/

void	tbox( int x1 , int y1 , int x2 , int y2 , int c )
{
	short	i , yp , xp ;
	unsigned short	data1 , data2 , a ;
	unsigned short	x01 , x02 , x11 , x12 ;
	volatile unsigned short	*addr1;
    volatile unsigned short	*addr2;
    volatile unsigned short	*addr3;
    volatile unsigned short	*addr4;



	if ( x1 < 0 )	x1 = 0 ;
	if ( y1 < 0 )	y1 = 0 ;
	if ( x2 > 511 )	x2 = 511 ;
	if ( y2 > 511 )	y2 = 511 ;

	yp = y2 - y1 ;
	x01 = x1 / 16 ;
	x02 = x2 / 16 ;
	x11 = x1 & 15 ;
	x12 = x2 & 15 ;
	xp = x02 - x01 ;

	if ( c ) {
		a = 0xffff ;
		*(volatile unsigned short * ) 0xe8002a = c * 16 + 768 ;
	} else {
		a = 0 ;
		*(volatile unsigned short * ) 0xe8002a = 15 * 16 + 768 ;
	}


	data1 = 65535 - ( 32768 >> x11 ) ;
	data2 = 65535 - ( 32768 >> x12 ) ;

	addr1 = (volatile unsigned short * )0xe00000 + y1 * 0x40 + x01 ;
	addr2 = (volatile unsigned short * )0xe00000 + y1 * 0x40 + x02 ;
	addr3 = addr1 ;
	addr4 = addr1 + yp * 0x40 ;

					//	Vertical line
	*(volatile unsigned short * ) 0xe8002e = data1 ;
	for ( i = 0 ; i < yp ; i += 2 ) {
		*( addr1 += 0x40 ) = a ;
		*( addr1 += 0x40 ) = a ;
	}
	*(volatile unsigned short * ) 0xe8002e = data2 ;
	for ( i = 0 ; i < yp ; i += 2 ) {
		*( addr2 += 0x40 ) = a ;
		*( addr2 += 0x40 ) = a ;
	}
					//	Horizontal line
	if ( x01 == x02 ) {
		*(volatile unsigned short *)0xe8002e = ( linel[ x11 ] | liner[ x12 ] ) ;
		*addr3 = *addr4 = a ;
	} else {
		*(volatile unsigned short *)0xe8002e = linel[ x11 ] ;
		*( addr3++ ) = *( addr4++ ) = a ;

		*(volatile unsigned short *)0xe8002e = 0xffff ;
		for( i = 1 ; i < xp ; i++ ) {
			*( addr3++ ) = *( addr4++ ) = a ;
		}

		*(volatile unsigned short *)0xe8002e = liner[ x12 ] ;
		*addr3 = *addr4 = a ;
	}
}

void	srcbox( int dots , int c )
{
	short	x1 , y1 , x2 , y2 ;

	y1 = x1 = 128 - dots / 2 ;
	y2 = x2 = 127 + dots / 2 ;

	tbox( x1 , y1 , x2 , y2 , c ) ;
}

void	dstbox( int dots , int c )
{
	tbox( 256 , 0 , 255 + dots * 2 , dots * 2 , c ) ;
}
