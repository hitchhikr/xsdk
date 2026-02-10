/*
	ZMusic Drive Routine	for Yoko-Shoot
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<doslib.h>
#include	"zlib.h"
#include	"sound.h"

short	*seq_tbl;				/* Z-muSiC internal work start address */

static	short	*bgm_data[ BGMNUM ];
static	int	bgm_length[ BGMNUM ];
static	char	*sound_data[ SOUNDNUM ];
#if( PCMNUM )
static	char	*pcm_data[ PCMNUM ];
static	int	pcm_length[ PCMNUM ];
#endif

/*	BGM data filename	*/

static char *bgm_filename[ BGMNUM ] = {	"start.zmd" , "1st.zmd",
					"boss.zmd" , "2nd.zmd",
					"3rd.zmd"
};

/*	Sound effect data filename	*/

static char *sound_filename[ SOUNDNUM ] = {	"sound01.zmd" ,	"sound02.zmd" ,
						"sound03.zmd" ,	"sound04.zmd" ,
						"sound05.zmd" ,	"sound06.zmd" ,
						"sound05.zmd" ,	"sound05.zmd" ,
						"sound05.zmd" ,	"sound05.zmd"
} ;

char sound_channel[ SOUNDNUM ] ;

#if( PCMNUM )
static	char	*pcm_filename[ PCMNUM ] = {}
#endif

/*
		Sound Initialization
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
	seq_tbl = z_get_wk_tbl();

/*		BGM data loading		*/

	for ( i = 0; i < BGMNUM; i++ ) {
		fp=fopen( bgm_filename[i], "rb" ) ;
		if( fp ) {
			bgm_length[ i ] = size = FILELENGTH( fp ) ;
			bgm_data[ i ] = ( short * )MALLOC( bgm_length[ i ] ) ;
			if( bgm_data[ i ] >= ( short * ) 0x81000000 ) {
				printf( "Memory cannot be allocated.\n" ) ;
				exit( 1 ) ;
			}
			printf("loading..%s\n", bgm_filename[i]);
			fread( bgm_data[ i ], 1, bgm_length[ i ], fp ) ;
			fclose( fp ) ;
		} else {
			printf("cannot load %s\n", bgm_filename[i]);
			exit( 1 );
		}
	}

/*		Load sound effect data		*/

	for ( i = 0 ; i < SOUNDNUM ; i++ ) {
		fp=fopen( sound_filename[i] , "rb" ) ;
		if( fp != NULL ) {
			size = FILELENGTH( fp ) ;
			sound_data[ i ] = ( char * )MALLOC( size ) ;
			if ( sound_data[ i ] >= ( char * ) 0x81000000 ) {
				printf( "Memory cannot be allocated.\n" ) ;
				exit( 1 ) ;
			}
			fread( sound_data[ i ]  , 1 , size , fp ) ;
			sound_channel[ i ] = sound_data[ i ][ 17 ] + 1 ;
			fclose( fp ) ;
		}
	}

/*		PCM data reading			*/

#if( PCMNUM )
	for ( i = 0 ; i < PCMNUM ; i++ ) {
		fp=fopen( pcm_filename[i] , "rb" ) ;
		if( fp != NULL ) {
			pcm_length[ i ] = size = FILELENGTH( fp ) ;
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
		i = BGM Number
*/

void	bgm( int i )
{
	z_stop( 0b111111111111111 , 0 , 0 ) ;
	if ( i >= 0 ) {
		play_cnv_data( 0 , bgm_data[ i ] ) ;
	}
}

void	bgm_fade( int speed )
{
	fade_out( speed ) ;
}

void	sd_on( int i )
{
	if( i < 20 ) {
		se_play( sound_channel[ i ] , sound_data[ i ] + 10 ) ;
	} else {
#if( PCMNUM )
		se_adpcm1( pcm_data[ i - 20 ] , pcm_length[ i - 20 ] , 0x0403 ) ;
#endif
	}
}
