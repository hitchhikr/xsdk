/*
 *		sound.h ( sound routine include file for StarCharge )
 *					Programmed by Naoya Kawahara
 */

#define	SOUNDNUM	8
#define	BGMNUM		10
#define	PCMNUM		0

#define		SD_NULL		0
#define		SD_SHOT		1
#define		SD_LASER	2
#define		SD_START	3
#define		SD_DAMAGE	4
#define		SD_DOWN		5
#define		SD_CRASH	6

void	sd_init() ;
void	sd_on( int number ) ;
void	bgm( int number ) ;
void	bgm_fade( int speed ) ;
