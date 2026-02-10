/*	calc.c : calculation program	*/

#include	<stdio.h>
#include	<iocslib.h>
#include	"calc.h"
#include	"main.h"
#include	"star.h"
#include	"enemy.h"
#include	"matrix.h"
#include	"dim.h"
#include	"txdisp.h"
#include	"sound.h"

#define		DELTA_X		1
#define		DELTA_Y		1
#define		DELTA_MAX	64
#define		DELTA_RANK	8
#define		DELTA_RATE_MAX	(DELTA_MAX / DELTA_RANK)
#define		DELTA_DIV	4
#define		RANGE		(HALF / 2)

#define		ABS(c)		((c) > 0 ? (c) : -(c))
#define		SIGN(c)		((c) > 0 ? 0 : -1)

MEDAT		me;
int		score, hi_score, no_damage_clear, hi_no_damage_clear;
char		hiname[5];
SHOTDAT		shot[SHOT_NUM];
int		range_u, range_r, range_d, range_l;
int		clear_flag, ending_flag;
int		y_org;

/***	1	B(Z)	A(X)	1	R	L	D	U	***/

const	int accel_rate[ DELTA_RATE_MAX + 1 ] =		{ 8, 8, 7, 6, 5, 4, 3, 2, 0 };
const	int unaccel_rate[ DELTA_RATE_MAX + 1 ] =	{ 1, 1, 2, 2, 3, 3, 4, 4, 5 };

const	int key_to_x[16] = {	0,		0,		0,		0,
				DELTA_X,	DELTA_X,	DELTA_X,	DELTA_X,
				-DELTA_X,	-DELTA_X,	-DELTA_X,	-DELTA_X,
				0,		0,		0,		0
			};
int key_to_y_org[16] = {	0,		DELTA_Y,	-DELTA_Y,	0,
				0,		DELTA_Y,	-DELTA_Y,	0,
				0,		DELTA_Y,	-DELTA_Y,	0,
				0,		DELTA_Y,	-DELTA_Y,	0
			};
int key_to_y[16];

void	InitStageCalc(void)
{
	int	i;
	
	me.x = me.dx = me.dy = 0;
	switch (round_) {
		case	2:
			y_org = me.y = (SKY + GROUND) / 2;
			break;
		case	4:
			y_org = me.y = (SKY + TENJOU) / 2;
			break;
		default:
			y_org = me.y = 0;
			break;
	}
	me.shield = SHIELD_MAX;
	me.crash = 0;
	for (i = 0; i < SHOT_NUM; ++i) shot[i].stat = 0;
	InitStar();
	ending_flag = 0;
}

void	InitCalc(int reverse)
{
	int	i;
	
	if (reverse) {
		for (i = 0; i < 16; ++i) key_to_y[i] = key_to_y_org[i];
	} else {
		for (i = 0; i < 16; ++i) key_to_y[i] = -key_to_y_org[i];
	}
}

#define		AUTOSHOT_TIME	7

void	Shoot(int key)
{
	static	int	pushed = 0;	/* 0 - ok     1 < .. -  no   */
	static	int	rightleft = 64;
	
	if (!(key & 32)) {
		if ( !pushed ) {
			int	i;
			
			pushed = 1;
			for (i = 0; i < SHOT_NUM; ++i) if ( !shot[i].stat ) break;
			if (SHOT_NUM == i) return;
			if (rightleft > 0) {
				PlaySE(SE_SHOT_R);
			} else {
				PlaySE(SE_SHOT_L);
			}
			shot[i].stat = 1;
			shot[i].x = me.x + rightleft;
			shot[i].y = me.y + 64;
			shot[i].z = HALF;
			shot[i].dx = (me.x      + 16 * me.dx - shot[i].x) / 24;
			shot[i].dy = (me.y - 64 - 16 * me.dy - shot[i].y) / 24;
			shot[i].dz = -128;
			rightleft = -rightleft;
		}
		if (AUTOSHOT_TIME == ++pushed) pushed = 0;
	} else {
		pushed = 0;
	}
}

void	MoveShot(void)
{
	int	i;

	for (i = 0; i < SHOT_NUM; ++i) {
		if (0 == shot[i].stat) continue;
		shot[i].z += shot[i].dz;
		if (shot[i].z < -HALF) {
			shot[i].stat = 0;
			continue;
		} else if (shot[i].z > HALF) {
			if ( 0 == me.crash && shot[i].x > me.x - ME_ZONE && shot[i].x < me.x + ME_ZONE && shot[i].y > me.y - ME_ZONE && shot[i].y < me.y + ME_ZONE) {
				bokan = me.crash = ME_BODY_CRASH_TIME;
				me.shield -= LASER_DAMAGE;
				PlaySE(SE_DAMAGE);
			}
			shot[i].stat = 0;
		}
		switch (round_) {
			case	2:
				if (shot[i].y > GROUND) {
					shot[i].stat = 0;
					continue;
				}
				break;
			case	4:
				if (shot[i].y < TENJOU) {
					shot[i].stat = 0;
					continue;
				}
				break;
			case	6:
				if (shot[i].y > SEEBOT || shot[i].y < SEETOP) {
					shot[i].stat = 0;
					continue;
				}
				break;
		}
		shot[i].y += shot[i].dy;
		shot[i].x += shot[i].dx;
	}
}

#define	ENDING_MOVE	8

void	MoveMe(int key)
{
	int	tmp;
	
	tmp = key_to_x[key & 15];
	if (0 == tmp && me.dx) {
		me.dx += (me.dx < 0 ? unaccel_rate[ABS(me.dx) / DELTA_RANK] : -unaccel_rate[ABS(me.dx) / DELTA_RANK]);
	} else if ( SIGN(me.dx) == SIGN(tmp) ) {
		me.dx += accel_rate[ABS(me.dx) / DELTA_RANK] * tmp;
	} else {
		me.dx += tmp * DELTA_RATE_MAX;
	}
	
	tmp = key_to_y[key & 15];
	if (0 == tmp && me.dy) {
		me.dy += (me.dy < 0 ? unaccel_rate[ABS(me.dy) / DELTA_RANK] : -unaccel_rate[ABS(me.dy) / DELTA_RANK]);
	} else if ( SIGN(me.dy) == SIGN(tmp) ) {
		me.dy += accel_rate[ABS(me.dy) / DELTA_RANK] * tmp;
	} else {
		me.dy += tmp * DELTA_RATE_MAX;
	}
	
	me.x += me.dx / DELTA_DIV;
	me.y -= me.dy / DELTA_DIV;
	
	switch (round_) {
		case	7:
			if (me.x > RANGE / 2) me.x = RANGE / 2;
			range_r = (RANGE / 2 - me.x) / 8;
			if (me.x < -RANGE / 2) me.x = -RANGE / 2;
			range_l = (me.x + RANGE / 2) / 8;
			if (clear_flag) {
				if (me.x > ENDING_MOVE) me.x -= ENDING_MOVE;
				if (me.x < -ENDING_MOVE) me.x += ENDING_MOVE;
				if (me.y > ENDING_MOVE) me.y -= ENDING_MOVE;
				if (me.y < -ENDING_MOVE) me.y += ENDING_MOVE;
			}
			break;
		default:
			if (me.x > RANGE) me.x = RANGE;
			range_r = (RANGE - me.x) / 8;
			if (me.x < -RANGE) me.x = -RANGE;
			range_l = (me.x + RANGE) / 8;
			if (clear_flag) {
				if (me.x > ENDING_MOVE) me.x -= ENDING_MOVE;
				if (me.x < -ENDING_MOVE) me.x += ENDING_MOVE;
				if (me.y > ENDING_MOVE + y_org) me.y -= ENDING_MOVE;
				if (me.y < -ENDING_MOVE + y_org) me.y += ENDING_MOVE;
			}
			break;
	}
	
	switch (round_) {
		case	2:
			if (me.y > RANGE) {
				me.y = RANGE;
			}
			range_d = (RANGE - me.y) / 8;
			if (me.y < SKY) {
				me.y = SKY;
			}
			range_u = (me.y + SKY) / 8 ;
			break;
		case	4:
			if (me.y > SKY) {
				me.y = SKY;
			}
			range_d = (SKY - me.y) / 8;
			if (me.y < -RANGE) {
				me.y = -RANGE;
			}
			range_u = (me.y + RANGE) / 8;
			break;
		case	6:
			if (me.y > RANGE / 2) {
				me.y = RANGE / 2;
			}
			range_d = (RANGE / 2 - me.y) / 8;
			if (me.y < -RANGE / 2) {
				me.y = -RANGE / 2;
			}
			range_u = (me.y + RANGE / 2) / 8;
			break;
		default:
			if (me.y > RANGE) {
				me.y = RANGE;
			}
			range_d = (RANGE - me.y) / 8;
			if (me.y < -RANGE) {
				me.y = -RANGE;
			}
			range_u = (me.y + RANGE) / 8;
			break;
	}
}

void	Calc(int key)
{
	if (clear_flag) key = 0xff;
	MoveMe(key);
	MoveShot();
	MoveStar();
	Shoot(key);
	Enemy();
}
