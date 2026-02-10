/*	sound.c : sound program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	"main.h"
#include	"sound.h"

unsigned char	*bgmdat[BGM_MAX];
int		bgmlen[BGM_MAX];
int		track[BGM_MAX] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						  7, 7, 8, 8, 8, 8, 8, 8, 1, 1, 1};

void	bgmread(char *filename, int bgmno)
{
	FILE	*fp;
	if (NULL == (fp = fopen(filename, "rb"))) {
		puts("bgm file open error");
		exit(1);
	}
	bgmlen[bgmno] = FILELENGTH(fp);
	if ((bgmdat[bgmno] = (unsigned char *)MALLOC(bgmlen[bgmno])) >=
	(unsigned char *)0x81000000) {
		fclose(fp);
		puts("bgm out of memory");
		exit(1);
	}
	printf("B.G.M. data %s now reading ...\n", filename);
	fread(bgmdat[bgmno], 1, bgmlen[bgmno], fp);
	fclose(fp);
}

/*------------------------------------------------------------------------------
Function $12 se_play

Function: Sound effect playback

Argument d2.l = number of tracks to interrupt (1-32)
         a1.l = performance data storage address
n
Return value None
------------------------------------------------------------------------------*/

void	PlaySE(int bgmno)
{
	register int _d1 asm( "%d1" );
	register int _d2 asm( "%d2" );
	register unsigned char *_a1 asm( "%a1" );
	
	if (nosound) return;
	_a1 = bgmdat[bgmno] + 10;
	_d1 = 0x12;
	_d2 = track[bgmno];
/*	asm( "trap #3"
		: // no output
		: "d"(_d1),"d"(_d2),"a"(_a1)
//		: "%d0","%d1","%d2","%d3","%a0","%a1"
	);*/
}

void	PlayBGM(int bgmno)
{
	FILE *fn;

	if (nosound) return;
	if (0 == (fn = fopen("opm", "wb"))) {
		puts("opm open error");
		exit(1);
	}
	if (0 == fwrite(bgmdat[bgmno], 1, bgmlen[bgmno], fn)) {
		puts("opm write error");
		exit(1);
	}
	fclose(fn);
}

void	InitVoice(void)
{
	PlayBGM(INIT_SOUND);
	PlayBGM(INIT_VOICE);
	PlayBGM(INIT_PCM);
	PlayBGM(BGM_STOP);

	MFREE(bgmdat[INIT_SOUND]);
	MFREE(bgmdat[INIT_VOICE]);
	MFREE(bgmdat[INIT_PCM]);
}

void	InitBGM(void)
{
	if (nosound) return;
	bgmread("stage1.zmd", BGM_ST1);
	bgmread("stage2.zmd", BGM_ST2);
	bgmread("stage3.zmd", BGM_ST3);
	bgmread("stage4.zmd", BGM_ST4);
	bgmread("stage5.zmd", BGM_ST5);
	bgmread("stage6.zmd", BGM_ST6);
	bgmread("stage7.zmd", BGM_ST7);
	bgmread("boss.zmd", BGM_BOSS);
	bgmread("ta_marm.zmd", BGM_TA_MARM);
	bgmread("ending.zmd", BGM_ENDING);
	bgmread("stop.zmd", BGM_STOP);

	bgmread("shot_r.zmd", SE_SHOT_R);
	bgmread("shot_l.zmd", SE_SHOT_L);
	bgmread("hit.zmd", SE_HIT);
	bgmread("destroy.zmd", SE_DESTROY);
	bgmread("damage.zmd", SE_DAMAGE);
	bgmread("reflect.zmd", SE_REFLECT);
	bgmread("pi.zmd", SE_PI);
	bgmread("byon.zmd", SE_BYON);

	bgmread("sound.zmd", INIT_SOUND);
	bgmread("voice.zmd", INIT_VOICE);
	bgmread("sc.zpd", INIT_PCM);

	InitVoice();
}

void	InitStageBGM(void)
{
	switch (round_) {
		case	1:
			PlayBGM(BGM_ST1);
			break;
		case	2:
			PlayBGM(BGM_ST2);
			break;
		case	3:
			PlayBGM(BGM_ST3);
			break;
		case	4:
			PlayBGM(BGM_ST4);
			break;
		case	5:
			PlayBGM(BGM_ST5);
			break;
		case	6:
			PlayBGM(BGM_ST6);
			break;
		case	7:
			PlayBGM(BGM_ST7);
			break;
		default:
			PlayBGM(BGM_ST1);
			break;
	}
}

/*------------------------------------------------------------------------------
Function $1a fade_out

Function Fade out

Argument d2.l = Fade-out speed (1 to 255)
         d2.l = 0 to disable fade-out mode

Return value None

Note: d2.l = 0 simply stops the volume from decreasing.
------------------------------------------------------------------------------*/

void	FadeOut(int speed)
{
	register int _d1 asm( "d1" );
	register int _d2 asm( "d2" );
	
	if (nosound) return;
	_d1 = 0x1a;
	_d2 = speed;
/*	asm( "trap #3"
		: // no output 
		: "d"(_d1),"d"(_d2)
//		: "d0","d1","d2"
	);*/
}

void	StopFadeOut(void)
{
	FadeOut(0);
	PlayBGM(BGM_STOP);
}
