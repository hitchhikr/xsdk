/*
 *		Main.c
 *				Programmed by Momo 1992 May
 *		This is the main routine!
 *			It's a little dirty
 */

#include	<stdio.h>
#include	<mouse.h>
#include	<graph.h>
#include	<sprite.h>
#include	<iocslib.h>
#include	"display.h"

#define		Pow( i )	( i ) * ( i ) 
#define		Abs( i )	( ( i ) >= 0 ? ( i ) : -( i ) )

const	short	mpow[ 33 ] = {	0 , 1 , 4 , 9 , 16 , 25 , 36 , 49  ,  64 ,
				81 , 100 , 121 , 144 , 169 , 196 , 225 , 256 ,
				289 , 324 , 361 , 400 , 441 , 484 , 529 , 576 ,
				625 , 676 , 729 , 784 , 841 , 900 , 961 } ;

static	short	colorbuf[ 65536 ] ;
static	short	buf[ 65536 ] ;

char	*makefn( char *fname , int n ) ;
char	*makefn2( char *fname , int n , char *extension ) ;

extern	char	ofn[] ;

void	paletmake( int paletblock ) ;

void	printmes( char *message )
{
	fill(  0 , 500 , 200 , 511 , 0 ) ;
	symbol( 0 , 500 , message , 1 , 1 , 0 , 65535 , 0 ) ;
}

/*
 *	Reduce to 16 colors!
 *		I'm taking it really easy and just using Baslib.
 */

void	crd( int cblock , int dots , int srcdots )
{
	unsigned short	paletbuf[ 16 ] ;
	unsigned short	henkan[ 256 ] ;
//	char		spbuf[ 256 ] ;
	short		*buf_addr ;

	volatile unsigned short	*src ;
	unsigned short	x = 0 , y = 0 ;
	unsigned short	i ;
	unsigned short	sx , sy ;
	int		j ;

	printmes( "Initializing..." ) ;

	i = 128 - srcdots / 2 ;

	for( j = 0 ; j <= dots ; j++ ) {
		henkan[ j ] = srcdots * j / dots + i ;
	}
	buf[ 0 ] = buf[ 1 ] = dots ;
	buf_addr = buf + 2 ;

	for( j = 0 ; j < 65536 ; j++ ) {		/* Color buffer initialization */
		colorbuf[ j ] = -1 ;
	}
	colorbuf[ 0 ] = 0 ;
	colorbuf[ 1 ] = 0 ;

	for( i = 0 ; i < 16 ; i++ ) {			/* Assigning palettes to an array */
		paletbuf[ i ] = sp_color( i , 'NASI' , cblock ) ;
	}

	printmes( "Translating..." ) ;

	for( y = 0 ; y < dots ; y ++ ) {
		src = (volatile unsigned short *) 0xc00000 + henkan[ y ] * 512 ;

		for( x = 0 ; x < dots ; x++ ) {

			unsigned short	r1 = 0 , g1 = 0 , b1 = 0 ;
			unsigned short	cnt = 0 , s = 0 ;

			// Apply anti-aliasing

			for( sy = henkan[ y ] ; sy < henkan[ y + 1 ] ; sy++ ) {
				src = (volatile unsigned short *) 0xc00000 + sy * 512 ;
				for( sx = henkan[ x ] ; sx < henkan[ x + 1 ] ; sx++ ) {
					unsigned short	c ;

					c = *( src + sx ) / 2 ;

					b1 += ( c & 31 ) ; c /= 32 ;
					r1 += ( c & 31 ) ; c /= 32 ;
					g1 += ( c & 31 ) ;
					cnt ++ ;
				}
			}

			if ( cnt ) {
				b1 /= cnt ;
				r1 /= cnt ;
				g1 /= cnt ;
				i = b1 * 2 + r1 * 64 + g1 * 2048 ;
			} else {
				short n ;
				n = ( i = *( (volatile unsigned short * ) 0xc00000
					+ henkan[ x ] + henkan[ y ] * 512 ) ) / 2 ;
				b1 = n & 31 ; n /= 32 ;
				r1 = n & 31 ; n /= 32 ;
				g1 = n & 31 ;
			}

			if ( colorbuf[ i ] == -1 ) {

				short	r2 , b2 , g2 , n ;
				short	min = 32767 , dif ;

							// Find the closest color

				for( j = 0 ; j < 16 ; j++ ) {

					n = paletbuf[ j ] / 2 ;
					b2 = n & 31 ; n /= 32 ;
					r2 = n & 31 ; n /= 32 ;
					g2 = n & 31 ;

					dif =	mpow[ Abs( b1 - b2 ) ] +
						mpow[ Abs( r1 - r2 ) ] +
						mpow[ Abs( g1 - g2 ) ] ;
					if ( min > dif ) {
						min = dif ;
						s = j ;
					}
				}
				colorbuf[ i ] = s ;
			}

			*( buf_addr++ ) = colorbuf[ i ] ;
			fill( x * 2 + 256 , y * 2 , x * 2 + 257 , y * 2 + 1 , paletbuf[ colorbuf[ i ] ] ) ;

		}
	}

/*
	printmes( "Compressing..." ) ;

	sprite_pattern = 0 ;
	for ( y = 0 ; y < dots ; y += 16 ) {
		for ( x = 0 ; x < dots ; x += 16 ) {
			for( j = 0 ; j < 256 ; j++ ) {
				short	h , v ;
				h = x * 2 + ( j & 15 ) * 2 + 256 ;
				v = y * 2 + j / 16 * 2 ;
				spbuf[ j ] = i = point( h , v ) ;
				fill( h , v , h + 1 , v + 1 , paletbuf[ i ] ) ;
			}
			sp_def( sprite_pattern , ( const char * )spbuf , 'NASI' ) ;
			sprite_pattern++ ;
		}
	}
*/

	printmes( " " ) ;

}

void	get_color( int ccolor )
{
	fill( 255 - 47 , 511 - 31 , 255 - 17 , 510 , ccolor ) ;
}

int	change_cblock( int cblock , int j ) 
{
	box( 257 , cblock * 16 + 257 ,
	     511 , cblock * 16 + 271 , 0 , 0xffff ) ;

	cblock = j ;

	box( 257 , cblock * 16 + 257 ,
	     511 , cblock * 16 + 271 , 65535 , 0xffff ) ;

	return j ;
}

/*
 *		Pattern Save
 */

void	psave( char *fname , int dots , int n ) 
{
	FILE	*fp ;
/*	char	*ad = ( short * ) 0xeb8000 ;
	int	i ;

	buf[ 0 ] = dots ;
	buf[ 1 ] = dots ;

	for( i = 2 ; i < dots * dots + 2 ; i += 2 ) {
		buf[ i     ] = *ad / 16 ;
		buf[ i + 1 ] = *ad & 15 ;
		ad++ ;
	}
*/

	fp = fopen( makefn2( fname , n , ".PAT" ) , "wb" ) ;
	fwrite( buf , 2 , dots * dots + 2 , fp ) ;
	fclose( fp ) ;

	printmes( "Wrote." ) ;

}


/*
 *		Main Loop
 */

void	mloop( char *fname )
{
	int	x , y , ccolor = 0 , cblock = 1 ;
	int	dum , bl , br ;
	int	i , j ;
	int	picnum = 1 ;
	int	vx ,vy ;
	int	dots = 64 ;		// Number of dots in the sprite
	int	srcdots = 256 ;		// Pic dot count

	mouse( 0 ) ;
	mouse( 4 ) ;	// Initialize mouse and disable soft keyboard
	mouse( 1 ) ;

	change_cblock( 1 , 1 ) ;

	while( !( BITSNS( 0 ) & 2 ) ) {
		mspos( &x , &y ) ;
		msstat( &dum , &dum , &bl , &br ) ;
		vx = x / 16 ;
		vy = y / 16 ;

							/*	Right button	*/
		if ( br ) {
			if ( x <= 255 && y <= 255 ) {
				get_color( ccolor = point( x , y ) ) ;
			} else 
			if ( x > 255 && y > 271 ) {
				i = vx - 16 ;
				j = vy - 16 ;
				get_color( ccolor = sp_color( i , 'NASI' , j ) ) ;
				cblock = change_cblock( cblock , j ) ;
			}
		}

							/*	Left button	*/
		if ( bl ) {
			if ( x > 255 && y > 271 ) {
				i = vx - 16 ;
				j = vy - 16 ;
				sp_color( i , ccolor , j ) ;
				fill( i * 16 + 258 , j * 16 + 258 ,
				      i * 16 + 270 , j * 16 + 270 ,
				      sp_color( i , 'NASI' , j ) ) ;
				cblock = change_cblock( cblock , j ) ;
			} else
			if ( vx == 15 && y > 271 ) {
				cblock = change_cblock( cblock , vy - 16 ) ;

			} else
			if (  XL < vx && XR > vx && vy == TRANS_Y ) {	// Conversion
				crd( cblock , dots , srcdots ) ;

			} else 
			if (  XL < vx && XR > vx && vy == PALETMAKE_Y ) {// Make a palette
				paletmake( cblock ) ;

			} else
			if (  XL < vx && XR > vx && vy == PATSAVE_Y ) {	// Pattern Save
				psave( ofn , dots , picnum ) ;

			} else
			if (  XL < vx && XR > vx && vy == AUTO_Y ) {	// Auto
				picnum = 0 ;

				while( picread( makefn( fname , ++picnum ) ) == 0 ) {
					crd( cblock , dots , srcdots ) ;
					psave( ofn , dots , picnum ) ;
				}
				picnum -- ;

			} else
			if ( vx == SL_X2 && vy == FILENAME_Y ) {	//Pic Left
				if( picread( makefn( fname , picnum - 1 ) ) == 0 ) {
					picnum-- ;
				}

			} else
			if ( vx == SL_X2 && vy == OBJDOTS_Y ) {		//ObjDots Left
				if ( dots > 16 ) {
					dstbox( dots , 0 ) ;
					dots -= 16 ;
					pr_objdots( dots ) ;
					dstbox( dots , 3 ) ;
				}
			} else
			if ( vx == SL_X2 && vy == SRCDOTS_Y ) {		//SrcDots Left
				if ( srcdots > 32 ) {
					srcbox( srcdots , 0 ) ;
					srcdots -= 8 ;
					pr_srcdots( srcdots ) ;
					srcbox( srcdots , 3 ) ;
				}

			} else
			if ( vx == SR_X2 && vy == FILENAME_Y ) {	//Pic Right
				if( picread( makefn( fname , picnum + 1 ) ) == 0 ) {
					picnum++ ;
				}
			} else
			if ( vx == SR_X2 && vy == OBJDOTS_Y ) {		//ObjDots Right
				if ( dots < 128 ) {
					dstbox( dots , 0 ) ;
					dots += 16 ;
					pr_objdots( dots ) ;
					dstbox( dots , 3 ) ;
				}
			} else
			if ( vx == SR_X2 && vy == SRCDOTS_Y ) {		//SrcDots Rightt
				if ( srcdots < 256 ) {
					srcbox( srcdots , 0 ) ;
					srcdots += 8 ;
					pr_srcdots( srcdots ) ;
					srcbox( srcdots , 3 ) ;
				}
			}

		while( bl )
			msstat( &dum , &dum , &bl , &br ) ;

		}

	}
}


void	paletmake( int paletblock )
{

	volatile unsigned short	*src ;
	unsigned short	x = 0 , y = 0 ;
	int		i , j ;

	printmes( "Analysing..." ) ;

	for( j = 0 ; j < 65536 ; j++ ) {		/* Color buffer initialization */
		colorbuf[ j ] = 0 ;
	}

	for ( y = 0 ; y < 256 ; y ++ ) {
		src = (volatile unsigned short * ) 0xc00000 + y * 512 ;
		for ( x = 0 ; x < 256 ; x ++ ) {
			colorbuf[ *( src++ ) ] ++ ;
		}
	}

	printmes( "Making..." ) ;

	colorbuf[ 1 ] = colorbuf[ 0 ] = 0 ;
	for( i = 1 ; i <= 16 ; i++ ) {
		int	max = 0 , color = 5 ;
		for( j = 2 ; j < 65536 ; j++ ) {
			if ( max < colorbuf[ j ] ) {
				color = j ;
				max = colorbuf[ j ] ;
			}
		}
		sp_color( i , color , paletblock ) ;
		fill( i * 16 + 258 , paletblock * 16 + 258 ,
		      i * 16 + 270 , paletblock * 16 + 270 ,
		      sp_color( i , 'NASI' , paletblock )
		) ;
		colorbuf[ color ] = 0 ;
	}

	printmes( " " ) ;

}
