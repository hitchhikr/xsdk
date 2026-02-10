/*	sound.h : sound header	*/

#define		BGM_MAX		22

enum	bgmdat {
	BGM_ST1,
	BGM_ST2,
	BGM_ST3,
	BGM_ST4,
	BGM_ST5,
	BGM_ST6,
	BGM_ST7,
	BGM_BOSS,
	BGM_TA_MARM,
	BGM_ENDING,
	BGM_STOP,
	
	SE_SHOT_R,
	SE_SHOT_L,
	SE_HIT,
	SE_DESTROY,
	SE_DAMAGE,
	SE_REFLECT,
	SE_PI,
	SE_BYON,

	INIT_SOUND,
	INIT_VOICE,
	INIT_PCM
};

void	InitBGM(void);
void	InitStageBGM(void);
void	PlaySE(int);
void	PlayBGM(int);
void	FadeOut(int);
void	StopFadeOut(void);
