/*
	ZMusic Drive Routine	for DesperadO-68k

					Programmed by Naoya Kawahara 1992

*/

#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	"dsp.h"
#include	"zlib.h"
#include	"sound.h"

short	*seq_tbl;

short	sdstat = 0;
extern	int	zmusic;
static	char	*bgm_data[BGMNUM];
static	int	bgm_length[BGMNUM];
	char	*sound_data[SOUNDNUM];
	char	*pcm_data[PCMNUM];
	int	pcm_length[PCMNUM];

/*	BGM data filename	*/
	
static	char	*bgm_filename[BGMNUM] = {	"dsp00.zmd",	"dsp01.zmd",
						"dsp02.zmd",	"dsp03.zmd",
						"dsp04.zmd",	"dsp05.zmd",
						"dsp06.zmd",	"dsp07.zmd",
						"dsp08.zmd",	"dsp09.zmd"
};

/*	Sound effect data file name */

static	char	*sound_filename[SOUNDNUM] = {	"sound01.zmd",	"sound02.zmd",
						"sound03.zmd",	"sound04.zmd",
						"sound05.zmd",	"sound06.zmd",
						"sound07.zmd",	"sound08.zmd",
						"sound09.zmd",	"sound10.zmd",
};

	char	sound_channel[SOUNDNUM];

static	char	*pcm_filename[PCMNUM] = {	"pcm00.dat", "pcm01.dat",
						"pcm02.dat" };


/*
		Sound Initial
*/

void sd_init()
{
	FILE	*fp;
	
	int	i, size;

/*		Resident check		*/

	if (z_chk_drv()) dsperror("There is no ZmuSiC");
	seq_tbl = z_get_wk_tbl();
	zmusic = 1;

/*		BGM data loading		*/

	for (i = 0; i < BGMNUM; i++) {
		fp=fopen(bgm_filename[i], "rb");
		if (fp != NULL) {
			bgm_length[i] = size = FILELENGTH(fp);
			bgm_data[i] = (char *)MALLOC(bgm_length[i]);
			if (bgm_data[i] >= (char *) 0x81000000) {
				dsperror("Memory cannot be allocated.\n");
			}
			fread(bgm_data[i] , 1, bgm_length[i], fp);
			fclose(fp);
		}
	}

/*		Load sound effect data		*/

	for (i = 0; i < SOUNDNUM; i++) {
		fp=fopen(sound_filename[i], "rb");
		if (fp != NULL) {
			size = FILELENGTH(fp);
/*
			printf("loading %s..length %d\n",sound_filename[i], size);
*/
						sound_data[i] = (char *)MALLOC(size);
			if (sound_data[i] >= (char *) 0x81000000) {
				dsperror("Memory cannot be allocated.\n");
			}
			fread(sound_data[i] , 1, size, fp);
			sound_channel[i] = sound_data[i][17] + 1;
			fclose(fp);
		}
	}

/*		PCM data reading			*/

	for (i = 0; i < PCMNUM; i++) {
		fp=fopen(pcm_filename[i], "rb");
		if (fp != NULL) {
			pcm_length[i] = size = FILELENGTH(fp);
/*
			printf("loading %s..length %d\n",pcm_filename[i], size);
*/
			pcm_data[i] = (char *)MALLOC(size);
			if (pcm_data[i] >= (char *) 0x81000000) {
				dsperror("Memory cannot be allocated.\n");
			}
			fread(pcm_data[i] , 1, size, fp);
			fclose(fp);
		}
	}

}

/*
		BGM start
		i ееее BGM Number
*/

void	bgm(int i)
{
	z_stop(0x7fff, 0, 0);
	if (i >= 0) play_cnv_data(0, bgm_data[i]);
}

void	bgm_fade(int speed)
{
	fade_out(speed);
}

void	led_control(void)
{
	int	i;
	unsigned char	j = 0;
	static	int	trno[7] = {6, 5, 0, 3, 4, 1, 2};
	for(i = 0; i < 7; i++) {
		j *= 2;
		if (!(*(seq_tbl + trno[i] * 128 + 1) > 2) ||
			(*((unsigned char *)seq_tbl + trno[i] * 256 + 0x42) & 0x80)) {
			j++;
		}
	}
	*(volatile unsigned char *)0xe8802f = j | 0x80;
	*(volatile unsigned char *)0xe8802d = 0x05;
}

void	vdispwait(void)
{
	BGSCRLST(0, 0, 0);
}
