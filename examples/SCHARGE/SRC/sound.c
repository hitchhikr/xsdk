/*
	ZMusic Drive Routine	for StarCharge

					Programmed by Naoya Kawahara      1992/1994

*/

#include	"stdmomo.h"
#include	"zlib.h"
#include	"sound.h"

static	char	*bgm_data[ BGMNUM ] ;
static	int	bgm_length[ BGMNUM ] ;
static	char	*sound_data[ SOUNDNUM ] ;
#ifdef	PCMS
static	char	*pcm_data[ PCMNUM ] ;
static	int	pcm_length[ PCMNUM ] ;
#endif

/*	BGM data file name	*/
	
static	char	*bgm_filename[ BGMNUM ] = {	"chargefm.zmd",	"sound01.zmd",
} ;
/*	Sound effect data file name	*/

static	char	*sound_filename[ SOUNDNUM ] = {	"sound00.zmd",	"sound01.zmd",
						"sound02.zmd",	"sound03.zmd",
						"sound04.zmd",	"sound05.zmd",
						"sound06.zmd",	"sound07.zmd",
} ;
static	char	sound_channel[ SOUNDNUM ] ;

#ifdef	PCMS
static	char	*pcm_filename[ PCMNUM ] = {	"pcm00.dat" , "pcm01.dat" ,
						"pcm02.dat" } ;
#endif

/*
		Sound Initial
*/

void sd_init()
{
	FILE	*fp ;
	
	int	i , size ;

/*		Resident check		*/

	if( z_chk_drv() ) {
		puts( "It seems there is no ZmuSiC." ) ;
		puts( "You can't play games like this." ) ;
		exit( 1 ) ;
	}


/*		BGM data loading		*/

	for ( i = 0 ; i < BGMNUM ; i++ ) {
		fp=fopen( bgm_filename[i] , "rb" ) ;
		if( fp != NULL ) {
			bgm_length[ i ] = size = FILELENGTH( fp ) ;
			bgm_data[ i ] = ( char * )MALLOC( bgm_length[ i ] ) ;
			if( bgm_data[ i ] >= ( char * ) 0x81000000 ) {
				printf( "Memory cannot be allocated.\n" ) ;
				exit( 1 ) ;
			}
			fread( bgm_data[ i ]  , 1 , bgm_length[ i ] , fp ) ;
			fclose( fp ) ;
		}
	}

/*		Load sound effect data		*/

	for ( i = 0 ; i < SOUNDNUM ; i++ ) {
		fp=fopen( sound_filename[i] , "rb" ) ;
		if( fp != NULL ) {
			size = FILELENGTH( fp ) ;
			printf( "loading %s..length %d\n",sound_filename[ i ], size ) ;
			sound_data[i] = (char *)MALLOC(size) ;
			if ( sound_data[i] >= (char *) 0x81000000 ) {
				printf( "Memory cannot be allocated.\n" ) ;
				exit( 1 ) ;
			}
			fread( sound_data[ i ]  , 1 , size , fp ) ;
			sound_channel[ i ] = sound_data[ i ][ 17 ] + 1 ;
			fclose( fp ) ;
		}
	}

/*		PCM data reading			*/

#ifdef	PCMS
	for ( i = 0 ; i < PCMNUM ; i++ ) {
		fp=fopen( pcm_filename[i] , "rb" ) ;
		if( fp != NULL ) {
			pcm_length[ i ] = size = FILELENGTH( fp ) ;
			printf( "loading %s..length %d\n",pcm_filename[ i ], size ) ;
			pcm_data[ i ] = ( char * )MALLOC( size ) ;
			if ( pcm_data[ i ] >= ( char * ) 0x81000000 ) {
				printf( "Memory cannot be allocated.\n" ) ;
				exit( 1 ) ;
			}
			fread( pcm_data[ i ]  , 1 , size , fp ) ;
			fclose( fp ) ;
		}
	}
#endif
}

/*
		BGM start
		i ееее BGM Number
*/

void	bgm( int i )
{
	z_stop( 0b111111111111111 , 0 , 0 ) ;
	if ( i >= 0 ) {
		play_cnv_data( 0 , bgm_data[ i-1 ] ) ;
	}
}

void	bgm_fade( int speed )
{
	fade_out( speed ) ;
}

/*
 *		Sound effect start
 */

void	sd_on( int i )
{
	if( i < 30 ) {
		se_play( sound_channel[ i ] , sound_data[ i ] + 10 ) ;
	} else {
#ifdef	PCMS
		se_adpcm1( pcm_data[ i - 30 ] , pcm_length[ i - 30 ] , 0x0403 ) ;
#endif
	}
}

