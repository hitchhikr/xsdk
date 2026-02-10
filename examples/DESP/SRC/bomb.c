/*
 *		Bomb.c(Enemy Missile Routine)
 *
 *				Programmed by  chika@kmc 1992
 */

#include	"sps.h"
#include	"dsp.h"
#include	"enemy.h"
#include	"sound.h"

/*
 *	Missile Type Number
 *	0.... Missile
 *	1.... HLaser
 *	2.... VLaser
 *	3.... Normal Bomb
 *	4.... TRIPLE LASER(for Hex)
 */


#define		BOMB_TYPENUM	10
#define		NULL		(void *)0
#define		BOMBLIFE	60
#define		BOMB_SPEED	(64 + rank * 3)	// Energy bullet speed
#define		LASER_SPEED	(112 + rank * 2)	// Laser Speed
#define		VLASER_SPEED	(105 + rank)		// Longitudinal laser speed
#define		BOMB_XSPEED2	(32 + rank)		// Missile initial velocity
#define		BOMB_XSPEEDMAX	(128 + rank * 2)	// Maximum missile speed
#define		BOMB_VVX	(2 + rank / 8)

const static char bombsound[BOMB_TYPENUM] = { 6, 5, 5, -1, 5, };

const static char bombpatnum[BOMB_TYPENUM][2] = {
	{ 0x12, 0x13 },	{ 0x14, 0x14 },
	{ 0x15, 0x15 },	{ 0x16, 0x16 },
	{ 0x17, 0x17 },	{ 0x18, 0x18 },
};

extern DSP	dspdata;

extern short	screen_x0;
extern short	screen_y0;
extern short	bomb_num;
extern BOMB	bombdata[];
extern short	VXtbl[64], VYtbl[64];
extern int	count_;
extern short	rank;					/* Difficulty */

/*
 *		Generates bullets (direction is automatic)
 */


int	makebomb(short x, short y, short type)
{
	if(type != 3) {
		if((x - dspdata.x) & 0x8000) {
			return makebomb2(x, y, type, 1);
		} else {
			return makebomb2(x, y, type, -1);
		}
	} else {
		return makebomb2(x, y, type, CalcDeg(x, y, dspdata.x, dspdata.y));
	}
}

/*
 *		Generates a bullet (specify direction)
 */

int	makebomb2(short x, short y, short type, short dir)
{
	BOMB	*data = bombdata;
	short	i = 0;
	short	deg;


	for (; data->type >= 0 && i < bomb_num; i++, data++);

	if (i < bomb_num) {
		data->x = x;
		data->y = y;

		data->type = type;

		switch(type) {
		  case 0:
			ENE_VY(*data) = 0;
			if(dir > 0) {
				ENE_VX(*data) = BOMB_XSPEED2;
			} else {
				ENE_VX(*data) = -BOMB_XSPEED2;
			}
			break;
		  case 1:
		  case 4:
			ENE_VY(*data) = 0;
			if(dir > 0) {
				ENE_VX(*data) = LASER_SPEED;
			} else {
				ENE_VX(*data) = -LASER_SPEED;
			}
			break;
		  case 2:
			ENE_VX(*data) = 0;
			ENE_VY(*data) = -VLASER_SPEED;
			break;
		  case 3:
			ENE_VX(*data) = VXtbl[dir] * BOMB_SPEED / 128;
			ENE_VY(*data) = VYtbl[dir] * BOMB_SPEED / 128;
		}
		data->sp = NULL;
		data->state = BOMBLIFE;
		ENE_PALET(*data) = 2;
		ENE_ENERGY(*data) = 1;

		if(bombsound[type] > 0) {
			sd_on(bombsound[type]);
		}

		return 0;
	} else {
		return -1;
	}
}

void	BombKill(BOMB *data)
{
	ENE_TYPE(*data) = -1;
	if(ENE_SP(*data) != NULL) {
		SpsFree(ENE_SP(*data));
		ENE_SP(*data) = NULL;
	}
	if(ENE_SHADOW(*data) != NULL) {
		SpsFree(ENE_SHADOW(*data));
		ENE_SHADOW(*data) = NULL;
	}
}

void	BombDisp(BOMB *data, unsigned char sprnum, short scx, short scy, short sx, short sy)
{
	short	x, y;

	x = SCX1x1(ENE_X(*data), scx);

	if(x < (short)-64 || x > (short)320) {
		if(ENE_SP(*data)) {
			SpsFree(ENE_SP(*data));
			ENE_SP(*data) = NULL;
		}
		if(ENE_SHADOW(*data)) {
			SpsFree(ENE_SHADOW(*data));
			ENE_SHADOW(*data) = NULL;
		}
	} else {
		if(!ENE_SP(*data)) {
			ENE_SP(*data) = SpsAlloc(CHR1x1);
			ENE_SP_COLOR(*data) = ENE_PALET(*data);
		}
		if(!ENE_SHADOW(*data)) {
			ENE_SHADOW(*data) = SpsAlloc(CHR1x1);
			ENE_SHADOW_COLOR(*data) = 2;
		}
		y = SCY1x1(ENE_Y(*data), scy);

		if(ENE_SP(*data)) {
			ENE_SP_X(*data) = x;
			ENE_SP_Y(*data) = y;
			ENE_SP_NUM(*data) = sprnum;
		}

		if(!ENE_SHADOW(*data)) return;
		if((sx != 0) && (count_ & 1)) {
			ENE_SHADOW_X(*data) = x + sx;
			ENE_SHADOW_Y(*data) = y + sy;
			ENE_SHADOW_NUM(*data) = 8 + ((count_ & 4) != 0);
		} else {
			ENE_SHADOW_NUM(*data) = 0;
		}

	}
}

void Bomb(BOMB *bomb_dat)
{
	short	i;
	BOMB	*data = bomb_dat;
	short	x;

	for(i = 0; i < bomb_num; i++, data++) {
		x  = SCX4x4(ENE_X(*data), screen_x0) & 0xffff;
		if(x < (short)-24 || x > (short)320) {
			ENE_STATE(*data) = 0;
		}
		if (ENE_TYPE(bomb_dat[i]) < 0) continue;
		if (ENE_ENERGY(*data) <= 0 || ENE_STATE(*data) <= 0 ||
		     ENE_Y(*data) < 0 || ENE_Y(*data) > YMAX) {
			BombKill(data);
			continue;
		}
		ENE_X(*data) += ENE_VX(*data);
		ENE_Y(*data) += ENE_VY(*data);
		if(ENE_VX(*data) < 0) {
			if(ENE_TYPE(*data) == 0) {
				BombDisp(data, bombpatnum[ENE_TYPE(bomb_dat[i])][0]
					, screen_x0, screen_y0, 14, 0);
				if(ENE_VX(*data) > -BOMB_XSPEEDMAX) {
					ENE_VX(*data) -= BOMB_VVX;
				}
			} else {
				BombDisp(data, bombpatnum[ENE_TYPE(bomb_dat[i])][0]
					, screen_x0, screen_y0, 0, 0);
			}
		} else {
			if(ENE_TYPE(*data) == 0) {
				BombDisp(data, bombpatnum[ENE_TYPE(bomb_dat[i])][1]
					, screen_x0, screen_y0, -14, 0);
				if(ENE_VX(*data) < BOMB_XSPEEDMAX) {
					ENE_VX(*data) += BOMB_VVX;
				}
			} else {
				BombDisp(data, bombpatnum[ENE_TYPE(bomb_dat[i])][1]
					, screen_x0, screen_y0, 0, 0);
			}
		}

	}

}
