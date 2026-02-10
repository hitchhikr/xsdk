/*	txdisp.c : text display program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	"txdisp.h"
#include	"main.h"
#include	"calc.h"
#include	"enemy.h"
#include	"timelist.h"
#include	"spdisp.h"
#include	"grdisp.h"
#include	"getkey.h"
#include	"sound.h"
#include	"disp.h"

#define	TXT0		(volatile unsigned short *)0xe00000
#define	TXT1		(volatile unsigned short *)0xe20000
#define	TXT2		(volatile unsigned short *)0xe40000
#define	TXT3		(volatile unsigned short *)0xe60000
#define	TXPAL		(volatile unsigned short *)0xe82200

#define	RGB(r, g, b)	((r) * 0x40 + (g) * 0x800 + (b) * 2)

#define	TOUMEI		0x0000
#define	KURO		0x0001
#define	SHIRO		RGB(31, 31, 31)
#define	RED		RGB(31, 0, 0)
#define	SPACE		RGB(0, 0, 0)
#define	SKYBLUE		RGB(0, 15, 31)
#define	DARKGREEN	RGB(0, 23, 0)
#define	DISPCOL		RGB(0, 23, 0)

#define	SHLD_X		88
#define	SHLD_W		SHIELD_MAX
#define	SHLD_Y		218
#define	SHLD_H		3

#define	BOSS_SHLD_X	104
#define	BOSS_SHLD_Y	42
#define	BOSS_SHLD_H	3

#define	SCORE_Y		2

#define	FINAL_BOSS_MOT	14
#define	FINAL_BOSS_HP	32


#define	START		176
#define	COURSE_RATE	32
#define	COURSE_X	28

#define	ALPNUM		31
#define	SPECIALNUM	3

#define	FADEOUTSPEED	48

static int		disp_shield, disp_boss_shield;
static int		disp_score, disp_course;
static int		boss_hp_disp_flag;
static const int	bin[16] = {1, 2, 4, 8, 16, 32, 64, 128,
			256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
static unsigned long	*patbuf;
static unsigned char	number[10][8];
static unsigned char	alphabet[ALPNUM][8];
static unsigned char	special[SPECIALNUM][8];

void	SetTxPalet(void)
{
	*(TXPAL + 0) = TOUMEI;
	*(TXPAL + 1) = SHIRO;
	*(TXPAL + 2) = SKYBLUE;
	*(TXPAL + 3) = SPACE;

	*(TXPAL + 8) = DISPCOL;
}

void	DamageColor(int flag)
{
	if (flag) {
		*(TXPAL + 8) = RED;
	} else {
		*(TXPAL + 8) = DISPCOL;
	}
}

void	SetPattern(void)
{
	unsigned long	*patptr = patbuf;
	int	i, line, bit, chk, set;

	for (i = 0; i < 10; ++i) {
		for (line = 0; line < 8; ++line) {
			number[i][line] = 0;
			for (bit = 0; bit < 8; ++bit) {
				set = 1 << bit;
				chk = 1 << (bit * 4);
				if (*patptr & chk) number[i][line] += set;
			}
			++patptr;
		}
	}
	for (i = 0; i < ALPNUM; ++i) {
		for (line = 0; line < 8; ++line) {
			alphabet[i][line] = 0;
			for (bit = 0; bit < 8; ++bit) {
				set = 1 << bit;
				chk = 1 << (bit * 4);
				if (*patptr & chk) alphabet[i][line] += set;
			}
			++patptr;
		}
	}
	for (i = 0; i < SPECIALNUM; ++i) {
		for (line = 0; line < 8; ++line) {
			special[i][line] = 0;
			for (bit = 0; bit < 8; ++bit) {
				set = 1 << bit;
				chk = 1 << (bit * 4);
				if (*patptr & chk) special[i][line] += set;
			}
			++patptr;
		}
	}
	MFREE(patbuf);
}

void	PutCha(int x, int y, int cha)
{
	int		adr = (x * 8 + DEFAULT_X) / 16 + (y * 8 + DEFAULT_Y) * (1024 / 16);
	unsigned char	*pat;
	int	i;

	if ('0' > cha) {
		if (' ' != cha) {
			pat = special[cha - '#'];
		} else {
			if (x & 1) {
				for (i = 0; i < 8; ++i) {
					*(TXT3 + adr) &= 0xff00;
					adr += 1024 / 16;
				}
			} else {
				for (i = 0; i < 8; ++i) {
					*(TXT3 + adr) &= 0x00ff;
					adr += 1024 / 16;
				}
			}
			return;
		}
	} else if ('9' >= cha) {
		pat = number[cha - '0'];
	} else {
		pat = alphabet[cha - 'a'];
	}
	if (x & 1) {
		for (i = 0; i < 8; ++i) {
			*(TXT3 + adr) |= (unsigned short)*pat;
			adr += 1024 / 16;	++pat;
		}
	} else {
		for (i = 0; i < 8; ++i) {
			*(TXT3 + adr) |= ( ((unsigned short)*pat) << 8 );
			adr += 1024 / 16;	++pat;
		}
	}
}

void	PutChaLine(int x, int line, int cha)
{
	int		adr = (x * 8 + DEFAULT_X) / 16 + (line + DEFAULT_Y) * (1024 / 16);
	unsigned char	*pat;
	int		i;

	if ('0' > cha) {
		if (x & 1) {
			for (i = 0; i < 8; ++i) {
				*(TXT3 + adr) &= 0xff00;
				adr += 1024 / 16;
			}
		} else {
			for (i = 0; i < 8; ++i) {
				*(TXT3 + adr) &= 0x00ff;
				adr += 1024 / 16;
			}
		}
		return;
	} else if ('9' >= cha) {
		pat = alphabet[29 + cha - '0'];
	} else {
		pat = alphabet[cha - 'a'];
	}
	if (x & 1) {
		for (i = 0; i < 8; ++i) {
			*(TXT3 + adr) |= (unsigned short)*pat;
			adr += 1024 / 16;	++pat;
		}
	} else {
		for (i = 0; i < 8; ++i) {
			*(TXT3 + adr) |= ( ((unsigned short)*pat) << 8 );
			adr += 1024 / 16;	++pat;
		}
	}
}

void	PutString(int x, int y, char *string)
{
	while (*string) {
		PutCha(x++, y, *string++);
	}
}

void	InitTx(void)
{
	FILE	*fp;
	int	size;
	const char	*patname = "text.sp";

	SetTxPalet();
	
	/*	Text Pattern Loading	*/
	if ((fp = fopen(patname, "rb")) == NULL) {
		fprintf(stdout, "file %s open error\n", patname);
		exit(1);
	}
	size = FILELENGTH(fp);
	if ((patbuf = (unsigned long *)MALLOC(size)) >= (unsigned long *)0x81000000) {
		fclose(fp);
		fputs("text pattern out of memory\n", stdout);
		exit(1);
	}
	printf("text pattern %s now reading ...\n", patname);
	fread(patbuf, 1, size, fp);
	fclose(fp);
	SetPattern();
	hiname[0] = 'n';
	hiname[1] = 'u';
	hiname[2] = 'k';
	hiname[3] = 'a';
	hiname[4] = '\0';
}

void	txdot(int x, int y, int col)
{
	const int	adr = ((x + DEFAULT_X) / 16) + (y + DEFAULT_Y) * (1024 / 16);
	const int	set = bin[15 - (x & 15)], res = 0xffff - set;

	col & 1 ? (*(TXT0 + adr) |= set) : (*(TXT0 + adr) &= res);
	col & 2 ? (*(TXT1 + adr) |= set) : (*(TXT1 + adr) &= res);
	col & 4 ? (*(TXT2 + adr) |= set) : (*(TXT2 + adr) &= res);
	col & 8 ? (*(TXT3 + adr) |= set) : (*(TXT3 + adr) &= res);
}

void	DispScore(void)
{
	int	n1, n2, n3, n4, trace;

	if (score == disp_score) return;
	disp_score = score;
	n4 = score / 1000;
	trace = score - n4 * 1000;
	n3 = trace / 100;
	trace = trace - n3 * 100;
	n2 = trace / 10;
	n1 = trace - n2 * 10;
	PutString(16, SCORE_Y, "    ");
	PutCha(16, SCORE_Y, n4 + '0');
	PutCha(17, SCORE_Y, n3 + '0');
	PutCha(18, SCORE_Y, n2 + '0');
	PutCha(19, SCORE_Y, n1 + '0');
}

void	InitDispTx(void)
{
	int	x, y;
	char	buf[32];
	int	mes_loc[STAGEMAX] = {7, 8, 8, 9, 7, 9, 10, 0, 0};
	char	*message[STAGEMAX] = {
			"ready navigation",	/*1*/
			"sunset sandlot",	/*2*/
			"orbital flight",	/*3*/
			"flambeau cave",	/*4*/
			"splash lightning",	/*5*/
			"aquatic nest",		/*6*/
			"gatecrasher"		/*7*/
			""			/*title*/
			""			/*ending*/
	};
	for (y = SHLD_Y; y < SHLD_Y + SHLD_H; ++y) txdot(SHLD_X - 1, y, 8);
	for (x = SHLD_X; x < SHLD_X + SHLD_W; ++x) {
		txdot(x, SHLD_Y - 1, 8);
		for (y = SHLD_Y; y < SHLD_Y + SHLD_H; ++y) txdot(x, y, 2);
		txdot(x, y, 8);
	}
	for (y = SHLD_Y; y < SHLD_Y + SHLD_H; ++y) txdot(SHLD_X + SHLD_W, y, 8);
	disp_shield = SHLD_W;
	disp_score = -1;
	disp_course = 0;
	clear_flag = boss_hp_disp_flag = 0;
	PutString(10, SCORE_Y, "score     0");
	sprintf(buf, "%1d", no_damage_clear);
	PutString(21, SCORE_Y, buf);
	DispScore();
	PutString(4, 27, "shield");
	PutChaLine(COURSE_X, START + 4, '0');
	PutChaLine(COURSE_X, START - 4 - 8 - (total_time[round_ - 1] / (COURSE_RATE * 8)) * 8, '1');
	
	sprintf(buf, "stage %1d", round_);
	PutString(12, 7, buf);
	PutString(mes_loc[round_ - 1], 10, message[round_ - 1]);
}

void	ChangeShield(void)
{
	if (me.shield < disp_shield) {
		int	y;
		
		for (y = SHLD_Y; y < SHLD_Y + SHLD_H; ++y) {
			txdot(disp_shield + SHLD_X - 1, y, 3);
		}
		if (0 < disp_shield) --disp_shield;
	}
	if (0 == disp_shield && me.crash < 2) gameover_flag = 1;
}

void	InitBossHpDisp(void)
{
	int	x, y;

	disp_boss_shield = boss_hp;
	
	for (y = BOSS_SHLD_Y; y < BOSS_SHLD_Y + BOSS_SHLD_H; ++y) txdot(BOSS_SHLD_X - 1, y, 8);
	for (x = BOSS_SHLD_X; x < BOSS_SHLD_X + boss_hp; ++x) {
		txdot(x, BOSS_SHLD_Y - 1, 8);
		for (y = BOSS_SHLD_Y; y < BOSS_SHLD_Y + BOSS_SHLD_H; ++y) txdot(x, y, 2);
		txdot(x, y, 8);
	}
	for (y = BOSS_SHLD_Y; y < BOSS_SHLD_Y + BOSS_SHLD_H; ++y) txdot(BOSS_SHLD_X + boss_hp, y, 8);
	if (final_boss_flag) {
		PutString(2, 5, "final boss");
	} else {
		PutString(8, 5, "boss");
	}
	boss_hp_disp_flag = 1;
}

void	ChangeBossShield(void)
{
	if (boss_hp < disp_boss_shield) {
		int	y;
		
		for (y = BOSS_SHLD_Y; y < BOSS_SHLD_Y + BOSS_SHLD_H; ++y) {
			txdot(disp_boss_shield + BOSS_SHLD_X - 1, y, 3);
		}
		--disp_boss_shield;
	}
}

void	DispCourse(void)
{
	if (round_counter < total_time[round_ - 1]) {
		if (disp_course + COURSE_RATE * 8 <= round_counter) {
			disp_course = round_counter;
			PutChaLine(COURSE_X, START - disp_course / COURSE_RATE, '}');
		}
	}
}

#define		ERASE_TIME	96
#define		CLEAR_TIME	32
#define		END_TIME	96

#define		BOSS5_1		136
#define		BOSS5_2		(BOSS5_1 + 48)
#define		BOSS2_1		(BOSS5_2 + 128)
#define		BOSS2_2		(BOSS2_1 + 48)
#define		BOSS2_3		(BOSS2_2 + 48)
#define		BOSS1		(BOSS2_3 + 96)

#define		FINAL_BOSS_TIME	660
#define		END_TIME_7	160
#define		ENDING_TIME	256

int	TxDisp(void)
{
	char	buf[32];

	ChangeShield();
	DispScore();
	DispCourse();
	if (ERASE_TIME == round_counter) {
		PutString(12, 7, "       ");
		PutString(7, 10, "                ");
	} else if (total_time[round_ - 1] - 80 == round_counter) {
		FadeOut(FADEOUTSPEED);
	}
	if (boss_appear_flag) {
		if (0 == boss_hp_disp_flag) {
			StopFadeOut();
			if (final_boss_flag) {
				PlayBGM(BGM_TA_MARM);
			} else {
				PlayBGM(BGM_BOSS);
			}
			InitBossHpDisp();
		} else {
			ChangeBossShield();
		}
	}
	if (!count_after_boss) return 0;
	if ( 7 != round_ ) {
		switch (count_after_boss) {
			case	1:
				FadeOut(FADEOUTSPEED);
				break;
			case	CLEAR_TIME:
				sprintf(buf, "stage %1d  clear", round_);
				PutString(9, 9, buf);
				if (SHLD_W == me.shield) {
					PutString(8, 12, "no damage bonus 1");
					++no_damage_clear;
				}
				clear_flag = 1;
				break;
			case	END_TIME:
				if (me.shield < SHLD_W && player_level_flag) {
					player_level_flag = 1;
				}
				tx_clr();
				BgClr();
				StopFadeOut();
				return 1;
		}
	} else {
		switch (count_after_boss) {
			case	CLEAR_TIME:
				FadeOut(FADEOUTSPEED);
				if (final_boss_flag) clear_flag = 1;
				break;
				
			case	END_TIME:
				StopFadeOut();
				break;
			case	BOSS5_1:
				if (!final_boss_flag) {
					BornEnemy(9, 21,
						1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
				}
				break;
			case	BOSS5_2:
				if (!final_boss_flag) {
					BornEnemy(8, 23,
						1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
				}
				break;
			case	BOSS2_1:
				if (!final_boss_flag) {
					BornEnemy(11, 21,
						1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
				}
				break;
			case	BOSS2_2:
				if (!final_boss_flag) {
					BornEnemy(10, 21,
						1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
				}
				break;
			case	BOSS2_3:
				if (!final_boss_flag) {
					BornEnemy(11, 21,
						-1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
				}
				break;
			case	BOSS1:
				if (!final_boss_flag) {
					BornEnemy(12, 22,
						1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
				}
				break;
			case	FINAL_BOSS_TIME:
				if (!final_boss_flag) {
					count_after_boss = 0;
					boss_hp_disp_flag = 0;
					boss_hp = FINAL_BOSS_HP;
					BornEnemy(BOSS_MODEL, FINAL_BOSS_MOT,
						1, 1,
						0,	/*flag = 0*/
						0, 0, 0, 0, 0, 0/*dummy*/ );
					final_boss_flag = 1;
				}
				break;
			case	END_TIME_7:
				if (final_boss_flag) {
					ending_flag = 1;
					tx_clr();
					BgClr();
					StopFadeOut();
					BornEnemy(30, 20,
						1, 1,
						1,	/*flag = 0*/
						0, 0, 1150, 0, 0, 0/*dummy*/ );
				}
				break;
			case	ENDING_TIME:
				if (final_boss_flag) {
					if (SHLD_W == me.shield) ++no_damage_clear;
						if (me.shield < SHLD_W && player_level_flag) {
						player_level_flag = 1;
					}
					Flushing();
					return 1;
				}
				break;
		}
	}
	return 0;
}

#define	NAMELINE	10
#define	INITCT		16
#define	NAMECOL		5

void	EnterName(void)
{
	char	cha[2] = " ", *string = " abcdefghijklmnopqrstuvwxyz";
	int	c = 1, old_c, loc = 0, key, flag = 0, ct = 0;
	
	hiname[0] = 'a';
	hiname[1] = 'a';
	hiname[2] = 'a';
	hiname[3] = 'a';
	hiname[4] = '\0';
	PutString(NAMECOL, NAMELINE, "enter your name  aaaa");
	PutString(NAMECOL + 17 + loc, NAMELINE + 1, "|");
	for (;;) {
		ESC_CHECK();
		Vdispwait();
		old_c = c;
		key = GetKey();
		if (!(key & 1) &&  (key & 2)) {
			if (ct < 0) {
				++ct;
			} else {
				c = ( c + 26 ) % 27;
				ct = -INITCT;
			}
		}
		if ( (key & 1) && !(key & 2)) {
			if (ct > 0) {
				--ct;
			} else {
				c = ( c + 1  ) % 27;
				ct = INITCT;
			}
		}
		if (3 == (key & 3)) ct = 0;
		if (96 == (key & 96)) flag = 1;
		if (!(key & 64) && flag) {
			flag = 0;
			PutString(NAMECOL + 17 + loc, NAMELINE + 1, " ");
			if (++loc > 3) break;
			PutString(NAMECOL + 17 + loc, NAMELINE + 1, "|");
			if (' ' == hiname[loc]) {
				old_c = c = 0;
			} else {
				old_c = c = hiname[loc] - 'a' + 1;
			}
		}
		if (!(key & 32)&& flag) {
			flag = 0;
			PutString(NAMECOL + 17 + loc, NAMELINE + 1, " ");
			if (--loc < 0) loc = 0;
			PutString(NAMECOL + 17 + loc, NAMELINE + 1, "|");
			if (' ' == hiname[loc]) {
				old_c = c = 0;
			} else {
				old_c = c = hiname[loc] - 'a' + 1;
			}
		}
		cha[0] = string[c];
		if (c != old_c) {
			PutString(NAMECOL + 17 + loc, NAMELINE, " ");
			PutString(NAMECOL + 17 + loc, NAMELINE, cha);
			hiname[loc] = cha[0];
		}
	}
}

#define	DECISION_TIME	(60*30)

int	GameOver(void)
{
	int	key, loc = 19, old_loc, flag = 0;
	
	BgClr();
	DamageColor(0);
	if (hi_score < score || (hi_score == score && hi_no_damage_clear < no_damage_clear)) {
		hi_score = score;
		hi_no_damage_clear = no_damage_clear;
		EnterName();
	}
	PutString(10, 16, "game  over");
	PutString(12, 19, "continue");
	PutString(12, 21, "end");
	PutString(10, 19, "{");
	round_counter = 0;
	for (;;) {
		int	t;
		
		ESC_CHECK();
		old_loc = loc;
		key = GetKey();
		
		t = (DECISION_TIME - round_counter) / 60;
		
		if (!(key & 1) &&  (key & 2)) loc = 19;
		if ( (key & 1) && !(key & 2)) loc = 21;
		Vdispwait();
		PutCha(21, 16, ' ');
		PutCha(22, 16, ' ');
		PutCha(21, 16, '0' + (t / 10));
		PutCha(22, 16, '0' + (t % 10));
		if (loc != old_loc) {
			PlaySE(SE_PI);
			PutString(10, old_loc, " ");
			PutString(10, loc, "{");
		}
		if (++round_counter > DECISION_TIME) {
			loc = -1;
			break;
		}
		if (key & 64) flag = 1;
		if (!(key & 64) & flag) {
			PlaySE(SE_BYON);
			break;
		}
	}
	SpClr();
	tx_clr();
	if (19 == loc) return 0;
	else if (21 == loc) return 1;
	else return -1;
}

void	GameEnd(void)
{
	int	i;
	
	if (hi_score < score || (hi_score == score && hi_no_damage_clear < no_damage_clear)) {
		hi_score = score;
		hi_no_damage_clear = no_damage_clear;
		EnterName();
	}
	PutString(11, 16, "game  over");
	while (!(GetKey() & 64)) ESC_CHECK();
	while (GetKey() & 64) ESC_CHECK();
	me.x = me.y = 0;
	for (i = 0; i < SHOT_NUM; ++i) shot[i].stat = 0;
	SpClr();
	tx_clr();
}

#define	MES1	128
#define	MES2	160
#define	MES3	192
#define	MES4	512
#define	MES5	560
#define	MES6	920
#define	MES7	944
#define	MES8	1072
#define	MES9	1148

void	EndingMessage(void)
{
	char	buf[32];
	
	switch (round_counter) {
		case	MES1:
			PutString(8,7,"mission complete");
			break;
		case	MES2:
			sprintf(buf, "score   %04d0%1d", score, no_damage_clear);
			PutString(9,14,buf);
			break;
		case	MES3:
			switch (player_level_flag) {
				case	0:
					PutString(5,21,"you are normal player");
					PutString(4,24,"challenge no miss clear");
					break;
				case	1:
					PutString(6,21,"you are good player");
					PutString(3,24,"challenge no damage clear");
					break;
				case	2:
					PutString(6,21,"you are super player");
					PutString(8,24,"congratulations");
					break;
			}
			break;
		case	MES4:
			PutString(8,7,"                ");
			PutString(9,14,"                ");
			PutString(5,21,"                     ");
			PutString(3,24,"                         ");
			break;
		case	MES5:
			PutString(8, 7, "produce");
			PutString(10, 8, "and program");
			PutString(16, 10, "h#nakanishi");
			PutString(8, 13, "graphic");
			PutString(16, 15, "t#ooya");
			PutString(8, 18, "music");
			PutString(16, 20, "t#oka");
			PutString(8, 23, "sound effect");
			PutString(16, 25, "c#yoshida");
			break;
		case	MES6:
			PutString(8, 7, "       ");
			PutString(10, 8, "           ");
			PutString(16, 10, "           ");
			PutString(8, 13, "       ");
			PutString(16, 15, "      ");
			PutString(8, 18, "     ");
			PutString(16, 20, "     ");
			PutString(8, 23, "            ");
			PutString(16, 25, "         ");
			break;
		case	MES7:
			PutString(8, 10, "special thanks");
			PutString(16, 13, "t#takeuchi");
			PutString(16, 15, "t#ogino");
			PutString(16, 17, "y#abe");
			break;
		case	MES8:
			PutString(8, 10, "              ");
			PutString(16, 13, "          ");
			PutString(16, 15, "       ");
			PutString(16, 17, "     ");
			FadeOut(FADEOUTSPEED);
			break;
		case	MES9:
			StopFadeOut();
			break;
	}
}
