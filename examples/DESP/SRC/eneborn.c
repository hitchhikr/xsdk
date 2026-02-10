#include		<stdlib.h>
#include		"dsp.h"
#include		"enemy.h"
#include		"eneborn.h"
#include		"txput.h"

/*\
 *   To register a new movement, compile this source again and add it to eneborn.h.
\*/

extern	ENEMY	ene_dat[];
extern	short	ene_num;
extern	short	enemy_left;
extern	short	enemynum;		/* The number of enemies currently present */
extern	short	*pat_buff[];
extern	int	count_;
extern	ENEPATTERN EnePattern[ROUNDMAX][TEKIMAX];
extern	short	rank;
extern	DSP	dspdata;

static	int	pattcnt;
static	int	ptcount;

void	TaraiBorn(ENEMY *data, short x, short y);
void	EnemyBornSub(ENEMY *data, int type, short x, short y, short dir);

static void (*EneMoveFnc[16])(struct ENEMY *data, short x, short y) = {
	EneMove_ao, EneMove_crash, EneMove_ao, EneMove_tarai, EneMove_iron,
	EneMove_bird, EneMove_hober, EneMove_hex, EneMove_sco, EneMove_kame,
	EneMove_ef, EneMove_bee
};


void	pattcntinit(int EnemyLeft, int stage)
{
	short i;
	pattcnt = 0;
	for(i = ENEMYLEFTMAX; i > EnemyLeft; i--){
		while(EnePattern[stage - 1][++pattcnt].type == TARAI_TYPE)
		;
	}
	ptcount = EnePattern[stage - 1][pattcnt].count_ - 32;
}

short	EnemyBorn(ENEMY *data, short scx, short scy, int stage)
{
	short i;
	ENEPATTERN *ep = &EnePattern[stage - 1][pattcnt];

	if (count_ % 1024 == 650) {
		if (ENE_TYPE(*data) < 0) {
			TaraiBorn(data, 200, -1);
		}
	}
	if (enemynum >= ene_num || enemynum >= enemy_left) return -1;

	if (enemynum <= 0) {
		ptcount = ep->count_;
	}

	if (ep->count_ <= ptcount++) {
		data++;
		for(i = 1; ENE_TYPE(*data) >= 0 && i < ene_num; i++, data++)
			;
		if (i < ene_num) {
			EnemyBornSub(data, ep->type, ep->x, ep->y, ep->vx);
			enemynum++;
			pattcnt++;
		}
	}

	return 0;
}

/*		Make enemies appear			*/

void	EnemyBornSub(ENEMY *data, int type, short x, short y, short vx)
{
	short	tmp;
	ENE_TYPE(*data) = type;

	if (y < 0) {
		ENE_EX(*data) = ENE_X(*data) = ((rand() % 32767) * 2) & 0xffff;
		ENE_EY(*data) = ENE_Y(*data) = ((rand() % 32767) / 8) + 1024;
	} else {
		ENE_EX(*data) = ENE_X(*data) = dspdata.x + x * DIV_X;
		tmp = dspdata.y + y * DIV_Y;
		if (tmp < YMIN) tmp = YMIN;
		if (tmp > YMAX) tmp = YMAX;
		ENE_EY(*data) = ENE_Y(*data) = tmp;
	}

	if (vx) {
		ENE_VX(*data) = vx;
	} else {
		ENE_VX(*data) = 0;
	}
	ENE_VY(*data) = 0;
	IS_ENE_MOVE(*data) = EneMoveFnc[type];
	if (rank < 32) {
		ENE_ENERGY(*data) = 1;
	} else {
		ENE_ENERGY(*data) = 2;
	}
	data->MotionType = AppearMotionType;
	data->MotionCnt = 0;
	ENE_BOMBCNT(*data) = 0;

	ENE_SP(*data) = (void *)0;
	ENE_SHADOW(*data) = (void *)0;
	data->count_ = 0;

}



/*		A tub appears		*/

void	TaraiBorn(ENEMY *data, short x, short y)
{
	ENE_SP(*data) = (void *)0;
	ENE_TYPE(*data) = TARAI_TYPE;
	ENE_EX(*data) = ENE_X(*data) = ((rand() % 32767) * 2) & 0xffff;
	ENE_EY(*data) = ENE_Y(*data) = 308;
	ENE_VX(*data) = 0;
	ENE_VY(*data) = 8;
	ENE_PALET(*data) = 3;
	IS_ENE_MOVE(*data) = EneMove_tarai;
	ENE_ENERGY(*data) = 1;
	data->atariflg = 1;
	data->state = 1;

}
