/*
 *			DesperadO-68k Main Routine
 *						Programmed by peach & Others.
 */


#include	<stdio.h>
#include	<stdlib.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<basic0.h>

#include	"sps.h"
#include	"dsp.h"
#include	"dspmove.h"
#include	"txput.h"
#include	"display.h"
#include	"disp2.h"
#include	"enemy.h"
#include	"atari.h"
#include	"sound.h"
#include	"vdispint.h"
#include	"graph.h"
#include	"item.h"
#include	"ending.h"
#include	"name.h"
#include	"txput.h"

#define		DSP_PATSIZE		58
#define		CRASH_PATSIZE		8
#define		ABORTNO			0xfff2
#define		MFP_IERB		*(volatile unsigned char *)0xe88009


#define		BESTER_ENERGY_MAX	30 * 8 * 128
#define		STAGEMAX		9

//#define		EBUG

static	void *		ABORTADR;
static	volatile unsigned char	INTDATA;

char	*SprLoadPath = ".";
char	*PicLoadPath = ".";
char	*ZmdLoadPath = ".";
char	*FntLoadPath = ".";

short *pat_buff[PAT_NUM];		//	spr address
short *mf_buff[PAT_NUM];		//	mf address
					//	Sprite pattern filename

/*	.rl || .mf filename	*/

static	char	*patfile[PAT_NUM] = {
				"dsp", "crash", "ao", "tar", "iron", "bird",
				"hober", "hex", "sco", "kame", "ef", "bee"
				};
static	char	is_exist_mf[PAT_NUM] = {
				1, 1, 1, 0, 0, 1, 0, 1,
				1, 1, 1, 1 };


static	char	strbuf[64];		//	String Buffer

DSP dspdata;
SHOT shotdata[SHOT_NUM];
ENEMY enemydata[ENE_NUM];
BOMB bombdata[BOMB_NUM];
//STAR stardata[STAR_NUM];

int	SSP;					/*	For B_SUPER	*/
int	zmusic = 0;				/*	Is zmusic included?	*/
int	ledflag = 1;				/*	Do you want to light up the LED?	*/
short	screen_x0, screen_y0;			/* Virtual coordinates of the top left corner of the screen */
int	score = 0;
short	enemy_left;				//	The number of remaining enemies.
int	count_;					//	Time from the start of the game
int	bester_energy;				//	Bester's Energy
int	whitecnt;				//	Counter with illuminated screen
						//	(0-64: 0..Normal, 64..Pure White)
int	damagecnt;				//	Counter when the tub falls
int	invincible = 0;			//	Invincible Flag
int	startstage = 1;			//	You can select the face
int	DesperadO = 3;
short	gamemode = 0;
short	joymempoint = 0;
short	joymem[32768];
short	rank = 0;				//	Difficulty level (difficulty:64 - 0:easy)

int	difficulty = 1;				// 0:easy - 3:hardest

volatile extern	int	spsflag;
volatile extern	int	VDI_ENE_FLG;
extern	short	GScroll[];
extern	char	*version;

static	int	bester_energy_down[4] = { 1,2,4,6 };

/*	Prototype Declaration		*/

void	init(void);
void	dspexit(void);
void	dsperror(char *);
int	title(void);
void	event(int count_, int stage);
void	GameOver(void);
int	pause(void);
int	dsp_mainloop(int stage);
int     spload(char *file);
int     palload(char *file);

void	usage()
{
	puts("usage  : dsp.x [options]");
	puts("options: -s<path>   .rl & .mf load path");
	puts("         -p<path>   .prl & .dp load path");
	puts("         -f<path>   .fnt load path");
	puts("         -l         Do not light the LED");
	puts("It is not currently possible to specify paths for .zms, dsp.sp, or dsp.pal.");
	puts("Keep them current.");
	exit(1);
}

void option(int argc, char **argv)
{
	int	i;

	ledflag = 1;
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-' || argv[i][0] == '/') {
			switch (argv[i][1]) {
			 case 'L':	/* spr path */
			 case 'l':
				ledflag = 0;
				break;
			 case 'S':	/* spr path */
			 case 's':
				SprLoadPath = argv[i] + 2;
			 	break;
			 case 'F':	/* spr path */
			 case 'f':
				FntLoadPath = argv[i] + 2;
			 	break;
			 case 'P':	/* spr path */
			 case 'p':
				PicLoadPath = argv[i] + 2;
			 	break;
			 default:
				usage();	break;
			}
		} else {
			usage();
		}
	}
}


/*
 *		Main Ruchin
 */

void	main(int argc, char **argv)
{
	int	gameover, i;
	FILE	*fp;

Momo:	SSP = B_SUPER (0);

	INTDATA = MFP_IERB;

	printf("DesperadO version %s\n", version);
	printf("Presented by Kyoto university Micro computer Club(KMC).\n");
	OS_CUROF();
	B_CUROFF();
	option(argc, argv);

	sd_init();
	MFP_IERB = 0x0f;
	vdisp_intinit();
	ABORTADR = INTVCS(ABORTNO, dspexit);

	/*	Sprite data loading (tentative)	*/

	for(i = 0; i < PAT_NUM; i++) {
		int	filesize;
		if (*patfile[i] == '\0') continue;
									//SPR
		sprintf(strbuf, "%s\\%s.rl", SprLoadPath, patfile[i]);
		if ((fp = fopen(strbuf, "rb")) == NULL) {
			dsperror("Unable to open .rl file.");
		}
		fread(&filesize, sizeof filesize, 1, fp);
		pat_buff[i] = (short *)MALLOC(filesize * 2);
		if (pat_buff[i] >= (short *) 0x81000000) {
			dsperror("Unable to allocate memory.\n");
		}
		decode(fp, pat_buff[i]);
		fclose(fp);

		if (is_exist_mf[i] == 0) continue;
									//MF
		sprintf(strbuf, "%s\\%s.mf", SprLoadPath, patfile[i]);
		if ((fp = fopen(strbuf, "rb")) == NULL) {
			dsperror("Unable to open .mf file.\n");
		}
		filesize = FILELENGTH(fp);
		mf_buff[i] = (short *)MALLOC(filesize);
		if (mf_buff[i] >= (short *) 0x81000000) {
			dsperror("Unable to allocate memory.\n");
		}
		fread(mf_buff[i], sizeof(char), filesize, fp);
		fclose(fp);
	}

	screen(0, 2, 1, 1);
	if (spload("dsp.sp") || palload("dsp.pal")) {
		dsperror("I don't have dsp.sp or dsp.pal.\n");
	}

	sprintf(strbuf, "%s\\dsp.fnt", FntLoadPath);
	tx_init(strbuf);			//Loading data for the text screen

	while(1) {
		int	stage = startstage - 1;

		dispinit0();

		i = title();

		joymempoint = 2;
		rank = 0;
		if (i == 0) {
			srand(joymem[0]);
			stage = joymem[1];
			DesperadO = 1;
			gamemode = PLAY_DEMO;
		} else {
			gamemode = PLAY_RECORD;
			stage = startstage;
			srand(i);
			joymem[0] = i;
			joymem[1] = stage;
			DesperadO = 4;
		}

		gameover = 0;

		stage = startstage;
		enemy_left = 32;
		bester_energy = 0;
		score = 0;

		VDI_ENE_FLG = 1;

		while(gameover == 0) {

			count_ = 0;

			SpsInit();
			ItemInit();
			DspInit(&dspdata, shotdata);

			dispinit1(stage, DesperadO, score);
			EnemyInit(10, enemydata, 10, bombdata);
			pattcntinit(enemy_left, stage);
			if (gamemode == PLAY_DEMO) {
				tx_puts(10, 25, "PRESS TRIGGER", 14);
			}
			rank = stage * (5 + difficulty);

			while(VDI_ENE_FLG);

			if (dsp_mainloop(stage) == 0) {
				stage++;			//Clear
				enemy_left = 32;
				bester_energy = 0;
				if (stage > STAGEMAX) {
					gameover = 1;
					SpsExit();
					bgm(9);
					ending();
				} else VDI_ENE_FLG = 1;
			} else {
				if (gamemode != PLAY_DEMO) {
					if (DesperadO <= 0) {
						gameover = 1;
						GameOver();
					} else {
						DesperadO--;			//Death
						VDI_ENE_FLG = -1120;
					}

				} else {
					gpaletfade(0);
					spaletfade(0, 1);
					gameover = 1;
				}
			}

									//nameinput
			if (gameover && gamemode != PLAY_DEMO) {
				nameinput(score);
				for(i = 32; i >= 0; i--){
					vdispwait();
					gpaletfade(i);
					vdispwait();
					spaletfade(i, 1);
				}
			}

			SpsExit();
			gamemode = PLAY_NORMAL;

		}
	}

}

/*
 *		Abort processing 1
 */
void	dspexit(void)
{
	MFP_IERB = INTDATA;
	if (zmusic) bgm_fade(84);
	INTVCS(ABORTNO, ABORTADR);

	SpsExit();
	fcloseall();

dexit:	B_SUPER(0);
	txclear();
	screen(2, 0, 1, 0);
	KFLUSHIO(0xff);
	OS_CURON ();
	VDISPST(0, 0, 1);
	if (B_SFTSNS() & 128) LEDMOD(3, 1); else LEDMOD(3, 0);
	exit(1);
}

/*
 *		Abort processing 2
 */

void	dsperror(char *errmsg)
{
	MFP_IERB = INTDATA;
	if (zmusic) bgm_fade(84);
	INTVCS(ABORTNO, ABORTADR);

	VDISPST(0, 0, 1);
	SpsExit();
	fcloseall();
	if (B_SFTSNS() & 128) LEDMOD(3, 1); else LEDMOD(3, 0);

	txclear();
	screen(2, 0, 1, 0);
	puts(errmsg);
	KFLUSHIO(0xff);
	OS_CURON ();
	exit(1);
}

/*
 *		Pose Handling
 */

int	pause(void)
{
	while(BITSNS(0) & 2);
	while(!(BITSNS(0) & 2)) {
		if (BITSNS(2) & 2) {				//‚p
			tx_clr1();
			dspexit();
		}
		if (BITSNS(2) & 1) {				//Tab
			while(BITSNS(2) & 1);
			return 1;
		}
	}
	while(BITSNS(0) & 2);

	return 0;
}

/*
 *		-- Main Loop --
 *
 *	About count_
 *	0 < count_ < 32		    During the fade-in at the start of the stage
 *	count_ > 32		        During the game
 *	CLRCOUNT < count_ < 0	After clearing (CLRCOUNT < 0)
 */

#define		CLRCOUNT		-800		    /* COUNT immediately after clearing a round */
#define		CLRFADECOUNT		-650		/* The screen fades out. COUNT */
#define		STAGEENDCOUNT		-500		/* COUNT to end the first stage completely */

int	dsp_mainloop(int stage)
{
	int	pauseflg = 0;
	int	endflg = 0;
	int	i;

	whitecnt = 0;
	damagecnt = 0;
	while(endflg == 0) {
		if (pauseflg || (BITSNS(0) & 2)) {
			pauseflg = pause();
		}
		if (((BITSNS(0xe) & 2) && (BITSNS(4) & 4)) || bester_energy == 0) {
			DesperadO = 0;
			return 1;
		}

#ifdef EBUG
		GPALET(0, 63);
#endif
		spsflag = 1;
		if (count_ < 0) {
			endflg = dspmove(&dspdata, shotdata, 1);
		} else {
			endflg = dspmove(&dspdata, shotdata, 0);
		}
		shotmove(&dspdata, shotdata);
#ifdef EBUG
		GPALET(0, 31);
#endif
		Enemy(enemydata, bombdata, stage);
		Bomb(bombdata);
		ItemMove();

#ifdef EBUG
		GPALET(0, 31 * 2048);
#endif
		Spsys();
		atari(&dspdata, shotdata, enemydata, bombdata);

		GScroll[3] = screen_y0 / DIV_Y * 3 / 4;
		GScroll[2] = screen_x0 / DIV_X & 511;
		GScroll[1] = screen_y0 / DIV_Y;
		GScroll[0] = (screen_x0 / DIV_X * 3 / 2) & 511;
		GScroll[4] = GScroll[6] = screen_x0 / DIV_X / 2 & 511;
		GScroll[5] = GScroll[7] = screen_y0 * 3 / DIV_Y / 4;
#ifdef EBUG
		GPALET(0, 0);
#endif
		while (spsflag);

		event(count_, stage);

		scoredisp(score);
		if (count_ > 0) {
			if ((bester_energy -= bester_energy_down[difficulty]) < 0)
				bester_energy = 0;
			energy_disp2(bester_energy);
		}

		if (enemy_left == 0 && count_ > 0)
        {
            count_ = CLRCOUNT;
        }
		count_++;
		if (count_ == STAGEENDCOUNT)
        {
            return 0;
        }

		if (gamemode == PLAY_DEMO && (count_ > 2000 || JOY(0) != 255)) {
			bgm_fade(32);
			endflg = 1;
		}

	}

						/* I got beaten so I faded out */

	if (DesperadO == 0)
    {
        return 1;
    }
	for(i = 32; i >= 0; i--){
		vdispwait();
		gpaletfade(i);
		spaletfade(i, 1);
	}
	return 1;

}

/*
 *		Event
 */

#define	DIV_CLRFADE	3

void	event(int count_, int stage)
{
	static short bonus = 0;
	/* At the start of the round */

	if (count_ == 0) {
		bonus = 0;
	}
	if (count_ >= 0 && count_ <= 64) {
		gpaletfade(count_ / 2);
		spaletfade(count_ / 2, 1);
	}
	if (count_ == 66) {
		tx_puts(12, 10, "         ", 14);
		tx_puts(12, 13, "         ", 14);
	}

	/* When the round is cleared */

	if (count_ == CLRCOUNT + 30) {
		sd_on(3);
		bgm_fade(32);
		warn(1);
		tx_puts(9, 8, "STAGE   CLEAR!", 14);
		tx_putn(15, 8, stage, 1, 14);
		tx_puts(6, 13, "ENERGY BONUS  xxxxx0", 14);
		tx_putn(20, 13, bester_energy / 16, 5, 14);
	} else if (count_ == CLRCOUNT + 90) {
		bonus = bester_energy / 16;
	}
	if (count_ < 0 && bonus) {
		if (count_ & 1) sd_on(2);
		bonus -= 12;
		score += 12;
		if (bonus < 0) bonus = 0;
		tx_putn(20, 13, bonus, 5, 14);
		energy_disp2(bonus * 16);
	}
	if (count_ == CLRFADECOUNT+64) {
		tx_puts(9, 10, "              ", 14);
	}
	if (count_ > CLRFADECOUNT && count_ <= CLRFADECOUNT + 32 * DIV_CLRFADE) {
		gpaletfade((CLRFADECOUNT + 32 * DIV_CLRFADE - count_) / DIV_CLRFADE);
		spaletfade((CLRFADECOUNT + 32 * DIV_CLRFADE - count_) / DIV_CLRFADE, 1);
	} else {
		if (damagecnt) {			/*  The screen lights up */
			damagecnt--;
			bester_energy -= 256;
			if ((damagecnt & 7) == 0 ) {
				fadewhite(127);
				whitecnt = 127;
			} else {
				fadewhite(whitecnt -= 8);
			}
		} else if (whitecnt) {
			fadewhite(--whitecnt);
		}
	}

	warn(0);

}


void GameOver(void)
{
	int i;

	bgm(5);
	tx_clr2();
	tx_puts(11, 8, "GAME  OVER", 14);

	for(i = 110; i >= 0; i--){
		vdispwait();
	}

}

