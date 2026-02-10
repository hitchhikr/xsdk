/*
 *		sound.h
 */

#ifndef	_SOUND_H_
#define	_SOUND_H_

#define	SOUNDNUM	10
#define	BGMNUM		5
#define	PCMNUM		0

#define		SD_SHOT		0
#define		SD_DAMAGE	1
#define		SD_DOUT		2
#define		SD_ZOUT		3
#define		SD_HOUT		4
#define		SD_JIKIOUT	5

void	sd_init();
void	bgm( int number );
void	bgm_fade( int speed );
void	sd_on( int i );

#endif
