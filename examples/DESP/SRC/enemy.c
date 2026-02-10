#include        <stddef.h>
#include		"sps.h"
#include		"enemy.h"
#include		"math.h"

//#define	EBUG

extern short screen_x0;
extern short screen_y0;

short	sintbl[128];
short	VXtbl[64], VYtbl[64];

short	ene_num = 4, bomb_num = 4;
short	enemynum;

void	ENE_APPEAR(ENEMY *data, short scx, short scy);

void
Enemy(ENEMY *ene_dat, BOMB *bomb_dat, int stage) {
	short i;

	EnemyBorn(ene_dat, screen_x0, screen_y0, stage);

	for(i = ene_num - 1; i >= 0; i--) {
		if (ENE_TYPE(ene_dat[i]) < 0) continue;
		ene_dat[i].count_ ++;
		if (ene_dat[i].MotionType == AppearMotionType) {
			ENE_APPEAR(ene_dat + i, screen_x0, screen_y0);
			continue;
		}
		ENE_MOVE(ene_dat + i, screen_x0, screen_y0);
	}
	EnemyKill(ene_dat, screen_x0, screen_y0);
}

short
EnemyInit(short ene, ENEMY *enedat, short bomb, BOMB *bombdat) {
	short i;

	for(i = 0; i < 128; i++) {
		sintbl[i] = sin(2 * PI / 128 * i) * 1024;
	}
	for(i = 0; i < 64; i++) {
		VXtbl[i] = cos(2 * PI / 64 * i - PI/2) * 64;
		VYtbl[i] = sin(2 * PI / 64 * i - PI/2) * 64;
	}

	if (ene > ENE_NUM) {
		ene_num = ENE_NUM;
	} else {
		ene_num = ene;
	}
	if (bomb > BOMB_NUM) {
		bomb_num = BOMB_NUM;
	} else {
		bomb_num = bomb;
	}

	for(i = 0; i < ene_num; i ++) {
		IS_ENE_MOVE(enedat[i]) = 0;
		ENE_TYPE(enedat[i]) = -1;
		ENE_SP(enedat[i]) = NULL;
		ENE_SHADOW(enedat[i]) = NULL;
	}
	for(i = 0; i < bomb_num; i ++) {
		ENE_TYPE(bombdat[i]) = -1;
		ENE_SP(bombdat[i]) = NULL;
		ENE_SHADOW(bombdat[i]) = NULL;
	}
	enemynum = 0;
	return 0;
}

