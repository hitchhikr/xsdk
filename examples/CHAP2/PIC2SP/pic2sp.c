/*
 *		pic2sp.c
 *				Programmed by Momo 1992 May
 */

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<basic.h>
#include	<basic0.h>
#include	<string.h>
#include	<stdlib.h>
#include	"display.h"
#include	"main.h"

unsigned short	*picdata ;

char	fname[ 64 ] ;
char	chrbuf[ 64 ] ;

int	picread( char *fname ) ;
void	pic2sp( int pblock ) ;
void	mloop( char *fname ) ;

char	ofn[ 64 ] = "tmp" ;

void	argchk( int argc , char **argv )
{
	int	flg = 0 ;

	if ( argc == 2 || argc == 3 || argc == 4 ) {
		flg++ ;
	}

	if ( flg == 0 ) {
		printf( "Usage:\n" ) ;
		printf( "pic2sp<pic filename> [<sprite filename>] [<Palette filename>]\n" ) ;
		printf( "Example:pic2sp rot dsp dsp.pal\n" ) ;
		printf( "Here, pic2sp will load \"rot001.pic\".\n" ) ;
		printf( "The files to be exported are \"dsp001.pat\" to \"dsp0xx.pat\".\n" ) ;
		printf( "If you do not set a palette, the internal one will be used.\n" ) ;
		exit( 0 ) ;
	}
}


char	*makefn( char *fname , int n )
{
	char	buf1[ 10 ] = "000" ;
	char	buf2[ 10 ] ;

	strcpy( chrbuf , fname ) ;
	sprintf(buf2, "%d", n);
	strcat( buf1 , buf2 ) ;
	strcat( chrbuf , b_rightS( buf2 , buf1 , 3 ) ) ;

	strcat( chrbuf , ".pic" ) ;

	return chrbuf ;
}


char	*makefn2( char *fname , int n , char *extension )
{
	char	buf1[ 10 ] = "000" ;
	char	buf2[ 10 ] ;

	strcpy( chrbuf , fname ) ;
	sprintf(buf2, "%d", n);
	strcat( buf1 , buf2 ) ;
	strcat( chrbuf , b_rightS( buf2 , buf1 , 3 ) ) ;
	strcat( chrbuf , extension ) ;

	return chrbuf ;
}


void	paletload( char *fname )
{
	FILE	*fp ;
	int	i ;
	unsigned short	pbuf[ 256 ] ;
	unsigned short	*addr = ( unsigned short * )0xe82220 ;

	fp = fopen( fname , "rb" ) ;
	if( fp == NULL ) {
		printf( "I can't find the palette table.\n" ) ;
	}
	fread( pbuf , sizeof( unsigned short ) , 256 , fp ) ;
	fclose( fp ) ;

	for( i = 0 ; i < 240 ; i++ ) {
		*( addr++ ) = pbuf[ i ] ;
	}
}


void	paletsave( char *fname )
{
	FILE	*fp ;
	int	i ;
	unsigned short	pbuf[ 256 ] ;
	unsigned short	*addr = ( unsigned short * )0xe82220 ;

	for( i = 0 ; i < 240 ; i++ ) {
		pbuf[ i ] = *( addr++ ) ;
	}
	fp = fopen( fname , "wb" ) ;
	fwrite( pbuf , sizeof( unsigned short ) , 256 , fp ) ;
	fclose( fp ) ;

}

/*
 *	Main Routine
 */

int	main( int argc , char **argv )
{
	FILE	*fp ;
	int	ssp ;
	char	pfname[ 100 ] ;
	char	ch ;

	argchk( argc , argv ) ;

Momo:	ssp = B_SUPER( 0 ) ;

	if( ( picdata = ( unsigned short * ) MALLOC( 524288 ) )
	   >= ( unsigned short * ) 0x81000000 ) {
		puts( "Too bad! There's not enough memory.\n" ) ;
	}

	screen( 1 , 3 , 1 , 1 ) ;

	if( argc >= 3 ) {
		strcpy( ofn , argv[ 2 ] ) ;
	}

	if( argc == 4 ) {
		paletload( argv[ 3 ] ) ;
	}
	disp() ;
	picread( makefn( argv[ 1 ] , 1 ) ) ;

	mloop( argv[ 1 ] ) ;

	screen( 2 , 0 , 1 , 1 ) ;

	KFLUSHIO( 0xff ) ;			/*	Clear key buffer	*/

	puts( "Do you want to save the palette table (y/n)?" ) ;
	ch = *gets( chrbuf ) ;

	if( ch == 'Y' || ch == 'y' ) {
		puts( "\nWhat's the filename?" ) ;
		gets( pfname ) ;

		if( strlen(pfname) == 0 ) {
			strcpy( pfname , argv[ 3 ] ) ;
		}
		paletsave( pfname ) ;
	}

Reiko:	B_SUPER( ssp ) ;

}
