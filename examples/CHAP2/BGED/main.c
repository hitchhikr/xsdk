#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<basic0.h>
#include	"bged.h"

unsigned short	bgbuf[ BGBUFSIZE ] ;			/* It's too much hassle to do malloc now */
static	int	x0 = 0 ;
char	*filename ;

#define	swapint( a , b ) do{ int tmp; tmp = a ; a = b ; b = tmp ; }while( 0 )

region	copy_region ;


int	argchk( int argc , char **argv )
{
	FILE	*fp ;
	if( argc != 2 ) {
		puts( "usage:bged <filename>" ) ;
		exit( 1 ) ;
	}
	filename = argv[ 1 ] ;
	fp = fopen( filename , "rb" ) ;
	if( !fp ) return 1 ;
	fread( bgbuf , sizeof( unsigned short ) , BGBUFSIZE , fp ) ;
	fclose( fp ) ;
	return 0 ;
}

int	bgsave( void )
{
	FILE	*fp ;
	fp = fopen( filename , "wb" ) ;
	if( !fp ) return 1 ;
	fwrite( bgbuf , sizeof( unsigned short ) , BGBUFSIZE , fp ) ;
	fclose( fp ) ;
	return 0 ;
}

void	get_copy_region( void )
{
	int	x , y , tmp ;
	while( MS_GETDT() & 0x00ff ) {
		tmp = MS_CURGT() ;
		x = ( tmp / 65536 ) ;
		y = ( tmp & 65535 ) ;
								/* Mouse cursor display */
		SP_REGST( 0x80000000, x + 16 , y + 16 , 0x17f , 3 ) ; 
		SP_REGST( 0x80000001, ( x & 0xff8 ) + 15 , ( y & 0xff8 ) + 15 , 0x17e , 3 ) ;
		x = x / 8 - 10 ;
		y /= 8 ;
		if( x < 0 ) x = 0 ; else if( x > 21 ) x = 21 ;
		if( y < 0 ) y = 0 ; else if( y > 31 ) y = 31 ;

		if( x != copy_region.x2 || y != copy_region.y2 ) {
			erase_copy_region( x0 ) ;
			copy_region.x2 = x + x0, copy_region.y2 = y ;
			disp_copy_region( x0 ) ;
		}
	}
	if( copy_region.x2 < copy_region.x1 ) swapint( copy_region.x1 ,copy_region.x2 ) ;
	if( copy_region.y2 < copy_region.y1 ) swapint( copy_region.y1 ,copy_region.y2 ) ;
}

static	int	mode;
static	int	chrcode ;
static	int	paletblock = 1 ;			/* Selected palette block */
static	int	hinv = 0 , vinv = 0 ;

void	set_mode_to_put( void )
{
	mode = MODE_PUT ;
	erase_copy_region( x0 ) ;
	disp_status( paletblock , x0 , hinv , vinv , mode ) ;
}

void	set_mode_to_copy( void )
{
	mode = MODE_COPY ;
	disp_copy_region( x0 ) ;
	disp_status( paletblock , x0 , hinv , vinv , mode ) ;
}

void	change_x0( int new_x0 )
{
	erase_copy_region( x0 ) ;
	x0 = new_x0 ;
	if( x0 < XMIN ) x0 = XMIN ;
	if( x0 > ( XMAX - DISPLAY_XSIZE ) ) x0 = XMAX - DISPLAY_XSIZE ;
	drawbg( x0 ) ;
	disp_status( paletblock , x0 , hinv , vinv , mode ) ;
	disp_copy_region( x0 ) ;
}

void	main( int argc , char **argv )
{
	int	x , y , tmp ;
	int	rb , lb ;
	int	rbflg = 0 , lbflg = 0 ;
	int	dx , dy ;
	int	dx2, dy2;

	argchk( argc , argv ) ;
	make_bgnum();
	disp_init() ;
	copy_region.x1 = copy_region.x2 = copy_region.y1 = copy_region.y2 = 0 ;

	while( -1 ) {
		rb = lb = 0 ;
		tmp = MS_CURGT() ;
		x = ( tmp / 65536 ) ;
		y = ( tmp & 65535 ) ;

		tmp = MS_GETDT() ;
		if( tmp & 0xff00 ) {
			if( !lbflg ) lb = 1 ;
			lbflg = 1 ;
		} else {
			lbflg = 0 ;
		}
		if( tmp & 0x00ff ) {
			if( !rbflg ) rb = 1 ;
			rbflg = 1 ;
		} else {
			rbflg = 0 ;
		}
		dx = ( x -80 ) / 8 , dy = y / 8 ;

		if( dx2 != dx || dy2 != dy ) {
			disp_xy( dx+ x0, dy );
			dx2 = dx, dy2 = dy;
		}
								/* Mouse cursor display */
		SP_REGST( 0x80000000, x + 16 , y + 16 , 0x17f , 3 ) ; 
		SP_REGST( 0x80000001, ( x & 0xff8 ) + 15 , ( y & 0xff8 ) + 15 , 0x17e , 3 ) ; 

		if( (lb || rb) && x < 64 ) {
			set_mode_to_put();
			chrcode = bgnum[ x / 8 ][ y / 8 ] ;	/* Character Code */
			chrselect( chrcode , hinv , vinv ) ;
			continue ;
		}
		if( lb && x < 80 ) {
			switch( y / 16 ) {
			  case 0:
				change_x0( x0 - 16 );
				continue ;
			  case 1:
				change_x0( x0 + 16 );
				continue ;
			  case 2:
				hinv = !hinv ;
				patternwrite( hinv , vinv );
				disp_status( paletblock , x0 , hinv , vinv , mode ) ;
				continue ;
			  case 3:
				vinv = !vinv ;
				patternwrite( hinv , vinv );
				disp_status( paletblock , x0 , hinv , vinv , mode ) ;
				continue ;

			  case 4:
				paletblock = ( paletblock - 1 ) & 15 ;
				paletset( paletblock ) ;
				disp_status( paletblock , x0 , hinv , vinv , mode ) ;
				continue ;
			  case 5:
				paletblock = ( paletblock + 1 ) & 15 ;
				paletset( paletblock ) ;
				disp_status( paletblock , x0 , hinv , vinv , mode ) ;
				continue ;
			  case 8:
				bgsave() ;
				continue ;
			  case 9:
				screen( 0, 0, 1, 1 ) ;
				exit( 0 ) ;
			  case PT_Y:
				set_mode_to_put();
				continue;
			  case CP_Y:
				set_mode_to_copy();
				continue;
			}
		}
		if( lb && x >= 80 ) {
			if( mode == MODE_PUT ) {
				BGTEXTST( 0 , dx + BG_X0 , dy , 
				  bgbuf[ dy + ( dx + x0 ) * BG_YSIZE ] =
				  chrcode + 0x100 * paletblock + (vinv ? 0x8000:0) + (hinv ? 0x4000:0)
				) ;
			} else {
				int	sx , sy , x , y ;
				x = dx ;
				for( sx = copy_region.x1 ; sx <= copy_region.x2 ; sx++ , x++ ) {
					y = dy ;
					for( sy = copy_region.y1 ; sy <= copy_region.y2 ; sy++, y++ ) {
						BGTEXTST( 0 , x + BG_X0 , y , 
						  bgbuf[ y + ( x + x0 ) * BG_YSIZE ] =
						  bgbuf[ sy + ( sx ) * BG_YSIZE ] 
						) ;
					}
				}
			}
		}

		if( rb && x >= 80 ) {
			set_mode_to_copy();
			erase_copy_region( x0 ) ;
			copy_region.x1 = ( x -80 ) / 8 + x0;
			copy_region.y1 = y / 8 ;
			get_copy_region() ;
		}
	}

}
