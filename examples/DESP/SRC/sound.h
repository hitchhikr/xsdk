/*
 *		sound.h
 *					Programmed by Naoya Kawahara
 */

#include	"zlib.h"

#define	SOUNDNUM	10
#define	BGMNUM		10
#define	PCMNUM		8

#define		SD_SHOT		0
#define		SD_REV		1
#define		SD_PIPI		2
#define		SD_CONSOLE	3
#define		SD_NULL		4
#define		SD_LASER	5
#define		SD_MISSILE	6
#define		SD_WARN1	7
#define		SD_PUP		8
#define		SD_GO		9
#define		SD_ENEKILL1	20

void	sd_init();
// void	sd_on(int number);
void	bgm(int number);
void	bgm_fade(int speed);
void	lcd_control(int speed);
void	vdispwait(void);
void	led_control(void);


/*
		Sound effects Start
		i ееее Sound Number
*/

extern	char	*sound_data[ SOUNDNUM ];
extern	char	*pcm_data[ PCMNUM ];
extern	int	pcm_length[ PCMNUM ];
extern	char	sound_channel[ SOUNDNUM ];

static inline void	sd_on(int i)
{
	if (i < 20) se_play(sound_channel[i], sound_data[i] + 10);
	else se_adpcm1(pcm_data[i - 20], pcm_length[i - 20], 0x0403);
}
