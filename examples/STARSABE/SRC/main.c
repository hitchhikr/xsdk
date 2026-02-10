/*	main.c : main routine program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	"main.h"
#include	"calc.h"
#include	"star.h"
#include	"dim.h"
#include	"model.h"
#include	"matrix.h"
#include	"enemy.h"
#include	"timelist.h"
#include	"motion.h"
#include	"getkey.h"
#include	"spdisp.h"
#include	"grdisp.h"
#include	"txdisp.h"
#include	"sound.h"
#include	"disp.h"
#include	"interupt.h"

#define	ABS(c)		((c) > 0 ? (c) : -(c))
#define	CLS()		puts("\033*")
#define	LOCATE(x,y)	printf("\033[%d;%dH",y,x)
#define	CUR_ON()	printf("\033[>5l")
#define	CUR_OFF()	printf("\033[>5h")

static	int	speed_timer;
int		debug = OFF, nosound = OFF, stages = OFF;
int		round_, round_init, round_counter, round_keep;
int		gameover_flag, player_level_flag;
int		muteki = OFF;

void	InitAll(int debug, int khz15, int nobackground)
{
	InitPos();
	InitMatrix();
	InitModel();
	InitTimeList();
	InitSetBox();
	InitEnemy();
	InitMotion();
	InitKey();
	InitBGM();
	InitGr(khz15, nobackground);
	InitSp();
	tx_clr();
	InitTx();
	CLS();
	CUR_OFF();
	MS_CUROF();
	SKEY_MOD(0, 0, 0);
	*(volatile unsigned short *)0xe82500 = 0x12e4;	/*	Priority R1	*/
	*(volatile unsigned short *)0xe82600 = 0x007f;	/*	Priority R2	*/
	InitInt();
	hi_score = 100;
	hi_no_damage_clear = 0;
}

void	Sleep(int second)
{
	int	k = second * 60;
	
	while (k--) Vdispwait();
}

void	InitEnd(void)
{
	/*int	i;
	ENEMY	*now_enemy = list_enemy;*/

	speed_timer = (ONTIME() - speed_timer) / 100;
	EndInt();
	tx_clr();
	CRTMOD(16);
	/*for (;;) {
		if ( NULL == now_enemy) break;
		printf("\npx...%4d       py...%4d       pz...%4d       rx...%4d       ry...%4d\n", (now_enemy->pos).x, (now_enemy->pos).y, (now_enemy->pos).z, (now_enemy->rot_d).x, (now_enemy->rot_d).y);
		now_enemy = now_enemy->n;
	}
	for (i = 0; i < SHOT_NUM; ++i) {
		printf("\nstat...%1d       sx...%4d       sy...%4d       sz...%4d\n", shot[i].stat, shot[i].x, shot[i].y, shot[i].z);
	}
	if (speed_timer) printf("speed = %3d(%5d/%5d)\n", round_counter / speed_timer, round_counter, speed_timer);*/
	while (B_KEYSNS()) B_KEYINP();
	SKEY_MOD(-1, 255, 0);
	MS_CURON();
	CUR_ON();
	exit(0);
}

void	functions(void)
{
	static int	pause = 0;	/*	For pause and frame advance functions	*/
	
	ESC_CHECK();
	if ( !(GetKey() & 64) && !pause ) {
		PutString(13, 13, "pause");
		pause = 1;
	}
	while ( pause && ( (KeySns(6) & 32) || !(GetKey() & 64) ) ) {
		ESC_CHECK();
	}
	while (pause && !(KeySns(6) & 32)) {
		while ( pause && !(GetKey() & 64) ) {
			pause = 0;
			ESC_CHECK();
		}
		while ( !pause && !(GetKey() & 64) ) {
			ESC_CHECK();
		}
		if (!pause) {
			PutString(13, 13, "     ");
		}
		ESC_CHECK();
	}
}

void	InitGame(void)
{
	no_damage_clear = score = 0;
	player_level_flag = 2;
	round_ = round_init - 1;
}

void	InitTitle(void)
{
	round_ = 8;
	round_counter = 0;
	InitStageCalc();
	InitStageModel();
	InitStageTimeList();
	InitStageEnemy();
	InitStageMotion();
}

void	InitEnding(int nobackground)
{
	round_ = 9;
	round_counter = 0;
	InitStageModel();
	InitStageTimeList();
	InitStageEnemy();
	InitStageMotion();
	InitStageGr(nobackground);
	InitStar();
	PlayBGM(BGM_ENDING);
}

void	InitDemo(int rnd, int nobackground)
{
	round_ = rnd;
	round_counter = 0;
	InitStageCalc();
	InitStageModel();
	InitStageTimeList();
	InitStageEnemy();
	InitStageMotion();
	InitStageGr(nobackground);
	InitStar();
}

#define	DEMOENDTIME	1024

void	Demo(int rnd, int nobackground)
{
	int	key, flag = 0;
	
	InitDemo(rnd, nobackground);
	for (;;) {
		ESC_CHECK();
		AppearCheck();
		
		Calc(0xff);
		SetBox();
		
		if ( !(KeySns(5) & 32) ) while (v_int_count < VDISPWAIT_COUNT);
		v_int_count = 0;
		
		SpDisp();
		GrDisp();
		if ((round_counter & 31) > 23) {
			PutString(14,10,"    ");
			PutString(10,23,"            ");
		} else {
			PutString(14,10,"demo");
			PutString(10,23,"push  button");
		}
		if (++round_counter > DEMOENDTIME) break;
		key = GetKey();
		if (96 == (key & 96)) flag = 1;
		if ((96 != (key & 96)) & flag) break;
	}
	SweepEnemy();
	SpClr();
	tx_clr();
}

#define	GOTODEMOTIME	512
#define	SELECT_TIME	(60*15)

int	Title(int nobackground)
{
	int	loc = 25, old_loc, flag = 0, key, muteki_keep = muteki, rnd = round_keep;
	char	buf[32];
	
	DamageColor(0);		ScrollText(0, 0);
	sprintf(buf, "hi score %04d0%1d  %4s", hi_score, hi_no_damage_clear, hiname);
	PutString(5, 20, buf);
	PutString(5, 28, "$ 1995 h#nakanishi%kmc");
	for (;;) {
		ESC_CHECK();
		AppearCheck();
		
		Enemy();
		SetBox();
		
		if ( !(KeySns(5) & 32) ) while (v_int_count < VDISPWAIT_COUNT);
		v_int_count = 0;
		
		SpDisp();
		if ((round_counter & 31) > 23) {
			PutString(10,23,"            ");
		} else {
			PutString(10,23,"push  button");
		}
		if (++round_counter > GOTODEMOTIME) {
			SweepEnemy();
			SpClr();
			tx_clr();
			muteki = ON;
			if (rnd + 1 >= round_init) {
				Demo(rnd + 1, nobackground);
			} else {
				Demo(round_init, nobackground);
			}
			rnd = ++rnd % 6;
			muteki = muteki_keep;
			flag = 0;
			InitTitle();
			sprintf(buf, "hi score %04d0%1d  %4s", hi_score, hi_no_damage_clear, hiname);
			PutString(5, 20, buf);
			PutString(5, 28, "$ 1995 h#nakanishi%kmc");
		}
		key = GetKey();
		if (96 == (key & 96)) flag = 1;
		if ((96 != (key & 96)) & flag) break;
	}
	PlaySE(SE_BYON);
	SweepEnemy();
	SpClr();
	gr_clr();
	tx_clr();
	DispControl();
	PutString(5, 22, "select control mode");
	PutString(12, 25, "normal");
	PutString(12, 27, "reverse");
	PutString(10, 25, "{");
	flag = 0;
	round_counter = 0;
	for (;;) {
		int	t;
		
		ESC_CHECK();
		old_loc = loc;
		key = GetKey();
		t = (SELECT_TIME - round_counter) / 60;
		Vdispwait();
		PutCha(25, 22, ' ');
		PutCha(26, 22, ' ');
		PutCha(25, 22, '0' + (t / 10));
		PutCha(26, 22, '0' + (t % 10));
		
		if (!(key & 1) &&  (key & 2)) {
			loc = 25;
			ChangeControl(0);
		}
		if ( (key & 1) && !(key & 2)) {
			loc = 27;
			ChangeControl(1);
		}
		if (loc != old_loc) {
			PlaySE(SE_PI);
			PutString(10, old_loc, " ");
			PutString(10, loc, "{");
		}
		if (++round_counter > SELECT_TIME) {
			gr_clr();
			loc = -1;
			break;
		}
		if (key & 64) flag = 1;
		if (!(key & 64) & flag) {
			PlaySE(SE_BYON);
			break;
		}
	}
	tx_clr();
	InitGame();
	if (25 == loc) return 0;
	else if (27 == loc) return 1;
	else return -1;
}

#define	ENDING_TIME	1150

void	Ending(int nobackground)
{
	InitEnding(nobackground);
	for (;;) {
		ESC_CHECK();
		AppearCheck();
		
		Calc(0xff);
		SetBox();
		
		if ( !(KeySns(5) & 32) ) while (v_int_count < VDISPWAIT_COUNT);
		v_int_count = 0;
		
		EndingMessage();
		
		SpDisp();
		GrDisp();
		if (++round_counter > ENDING_TIME) break;
	}
	tx_clr();
}

void	InitStage(int nobackground)
{
	++round_;
	round_counter = 0;
	gameover_flag = 0;
	InitStageCalc();
	InitStageModel();
	InitStageTimeList();
	InitStageEnemy();
	InitStageMotion();
	InitStageGr(nobackground);
	InitDispTx();
	DispTarget();
	DrawTarget();
	InitStageBGM();
}

void	main(int ac, char **av)
{
	int	i = 0, vdisp = OFF, speed = OFF, khz15 = ON, nobackground = 0;
	
dummy:	B_SUPER(0);
	round_init = 1;
	round_keep = 0;
	while (--ac) {
		++i;
		switch (**(av + i)) {
			case	'v':
				vdisp = ON;
				break;
			case	's':
				speed = ON;
				break;
			case	'd':
				debug = ON;
				break;
			case	'n':
				nosound = ON;
				break;
			case	'p':
				round_init = *(*(av + i) + 1) - '0';
				if (round_init < 1 || round_init > STAGEMAX) round_init = 1;
				break;
			case	'm':
				muteki = ON;
				break;
			case	'h':
				khz15 = OFF;
				break;
			case	'b':
				nobackground = *(*(av + i) + 1) - '0';
				break;
			case	'c':
				stages = ON;
				break;
		}
	}
	InitAll(debug, khz15, nobackground);
	speed_timer = ONTIME();
	for (;;) {
		if (ON == stages) {
			int	c_mode;
			
			for (;;) {
				InitTitle();
				if (-1 != (c_mode = Title(nobackground))) break;
			}
			InitCalc(c_mode);
		} else {
			InitGame();
			InitCalc(1);
		}
		for (;;) {
			int	goto_title = 0;
			
			InitStage(nobackground);
			for (;;) {
				functions();
				AppearCheck();
				
				Calc( GetKey() );
				SetBox();
				
				if ( vdisp && !(KeySns(5) & 32) ) while (v_int_count < VDISPWAIT_COUNT);
				v_int_count = 0;
				
				SpDisp();
				DispTarget();
				GrDisp();
				if (TxDisp()) break;
				
				++round_counter;
				if (gameover_flag) {
					int	fg;
					
					PlayBGM(BGM_STOP);
					fg = GameOver();
					SweepEnemy();
					if (fg) goto_title = 1;
					round_keep = round_ % 6;
					--round_;
					no_damage_clear = score = 0;
					player_level_flag = 0;
					break;
				}
				/*if (speed) if (ONTIME() - speed_timer >= 30 * 100) InitEnd();*/
			}
			if ( OFF == stages && !gameover_flag) InitEnd();
			if (goto_title) break;
			if (7 == round_) {
				Ending(nobackground);
				GameEnd();
				SweepEnemy();
				break;
			}
		}
	}
}
