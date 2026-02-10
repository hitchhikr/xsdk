#include	<stdlib.h>
#include	<math.h>
#include	"battle.h"
#include	"cpu.h"

#define	BORDERXMIN	512
#define	BORDERXMAX	3328
#define	BORDERYMIN	768
#define	BORDERYMAX	3072

#define	SWORDLENGTH	200
#define	SWORDLENGTH2	90	/* Amount of shift to the right */
#define	STAGEMAX	8
#define	MAN_WIDTH	( 16 * 6 )
#define	LASER_WIDTH	( 16 * 6 )
#define	ML_WIDTH	(MAN_WIDTH + LASER_WIDTH)
#define	MANV	(nowcnf->man_speed)

typedef struct _cpudata {
	int	totalcount;
	int	count;
	int	count2;
	int	dir;
	int	stage;
	enum	{ TOSWORD, TOLASER, NIGE, HOGE } mode;
	int	laserdelay;
	int	sworddelay;
} cpu_t;

cpu_t	cpudata[PLAYERNUM];

void	cpuinit(int stage)
{
	cpu_t	*cp;
	int i = 0;
	for (i = 0; i < PLAYERNUM; i++) {
		cp = cpudata + i;
		cp->count = 0;
		cp->totalcount = 0;
		cp->stage = stage - 1;
		if ((rand() % 32767) & 1) cp->mode = TOLASER;
		else cp->mode = HOGE;
		if (!i) cp->dir = 3; else cp->dir = 7;
	}
}

static	int	dir2stick[8] = {1, 9, 8, 10,  2, 6, 4, 5};
static	int	iswflg[4][8] = {
	{1, 1, 0, 0, 0, 0, 0, 1},
	{0, 1, 1, 1, 0, 0, 0, 0},
	{0, 0, 0, 1, 1, 1, 0, 0},
	{0, 0, 0, 0, 0, 1, 1, 1}
};

	
/*
 *		Check if you are within the laser's hitbox
 */

int	lasercheck(int x, int y, int ex, int ey)
{
	int	dx, dy;			/* Enemy position as seen from your perspective */
	dx = ex - x, dy = ey - y;
	if (ABS(dx) < ML_WIDTH)
		if (dy < 0) return 0; else return 4;
	if (ABS(dy) < ML_WIDTH)
		if (dx < 0) return 6; else return 2;

	if (ABS(dx - dy) < ML_WIDTH)
		if (dy < 0) return 7; else return 3;
	if (ABS(dx + dy) < ML_WIDTH)
		if (dy < 0) return 1; else return 5;
	return -1;
}


int	lasercheck2(int x, int y, int ex, int ey)
{
	int	dx, dy;			/* Enemy position as seen from your perspective */
	dx = ex - x, dy = ey - y;
	if (ABS(dx) > 96 * DIVX || ABS(dy) > 96 * DIVX)return -1;
	if (ABS(dx) < ML_WIDTH)
		if (dy < 0) return 0; else return 4;
	if (ABS(dy) < ML_WIDTH)
		if (dx < 0) return 6; else return 2;

	if (ABS(dx - dy) < ML_WIDTH)
		if (dy < 0) return 7; else return 3;
	if (ABS(dx + dy) < ML_WIDTH)
		if (dy < 0) return 1; else return 5;
	return -1;
}


int	swordcheck(int x, int y, int ex, int ey)
{
	int i;
	int sx, sy;
	for (i = 0; i < 7; i++) {
		sx = x + COS(i) * SWORDLENGTH / SINDIV;
		sy = y + SIN(i) * SWORDLENGTH / SINDIV;
		sx += COS(i + 2) * SWORDLENGTH2 / SINDIV;
		sy += SIN(i + 2) * SWORDLENGTH2 / SINDIV;
		if (ABS(sx - ex) < ML_WIDTH && ABS(sy - ey) < ML_WIDTH) return i;
	}
	return -1;
}

STICK	randomthink(int pl, int wflg)
{
	int	p;
	STICK	s;
	cpu_t	*cp;
	man	*mp, *emp;
	int	dir, flg, tmp;
	static	int	delay[STAGEMAX] = {20,12, 5, 3, 1, 1, 1, 1};

	cp = cpudata + pl;
	mp = &mandat[pl];	emp = &mandat[1 - pl];
	s.bin = 0;
	flg = 1;

	if (cp->laserdelay) {
		if (!(--cp->laserdelay)) s.flg.lb = 1;
		if (cp->laserdelay < 13) s.bin |= dir2stick[cp->dir];
		return	s;
	}

	if (cp->sworddelay) {
		if (!(--cp->sworddelay)) s.flg.rb = 1;
		s.bin |= dir2stick[cp->dir];
		return	s;
	}

	cp->count++;
	if (!(cp->count & 7)) {
		if ((rand() % 32767) > 16384) cp->dir -= 2;
		do {
			p = 1;
			cp->dir = (cp->dir+1) & 7;
			if (wflg) {
				if (mp->y < BORDERXMIN) p &= iswflg[2][cp->dir];
				if (mp->x > BORDERYMAX) p &= iswflg[3][cp->dir];
				if (mp->y > BORDERXMAX) p &= iswflg[0][cp->dir];
				if (mp->x < BORDERYMIN) p &= iswflg[1][cp->dir];
			}
		} while (!p);
	}

	tmp = lasercheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 144 ){
		cp->dir = tmp;
		cp->laserdelay = delay[cp->stage];
	}
	tmp = swordcheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 78 ){
		cp->dir = tmp;
		cp->sworddelay = 4;
	}
	if (flg) s.bin |= dir2stick[cp->dir];
	return	s;
}

STICK	loopthink(int pl)
{
	int	p;
	STICK	s;
	cpu_t	*cp;
	man	*mp, *emp;
	int	tmp;
	static	int	delay[STAGEMAX] = {1,1, 1, 1, 1, 1, 1, 1};

	cp = cpudata + pl;
	mp = &mandat[pl];	emp = &mandat[1 - pl];
	s.bin = 0;

	if (cp->laserdelay) {
		if (!(--cp->laserdelay)) s.flg.lb = 1;
		if (cp->laserdelay < 13) s.bin |= dir2stick[cp->dir];
		return	s;
	}
	if (cp->sworddelay) {
		if (!(--cp->sworddelay)) s.flg.rb = 1;
		s.bin |= dir2stick[cp->dir];
		return	s;
	}
	cp->count++;
	if (!(cp->count & 7)) cp->dir = (cp->dir+1) & 7;
	tmp = lasercheck2(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 144 ){
		cp->dir = tmp;
		cp->laserdelay = delay[cp->stage];
	}
	tmp = lasercheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 144 && emp->charge < 64){
		cp->dir = tmp;
		cp->laserdelay = delay[cp->stage];
	}
	tmp = swordcheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 78 ){
		cp->dir = tmp;
		cp->sworddelay = 4;
	}
	s.bin |= dir2stick[cp->dir];
	return	s;
}

STICK	fixedthink(int pl, int wflg)
{
	STICK	s;
	cpu_t	*cp;
	man	*mp, *emp;
	int	dir, flg, tmp;
	static	int	delay[STAGEMAX] = {18, 4, 1, 2, 1, 1, 1, 1};

	cp = cpudata + pl;
	mp = &mandat[pl];	emp = &mandat[1 - pl];
	s.bin = 0;
	flg = 1;

	if (cp->laserdelay) {
		if (!(--cp->laserdelay)) s.flg.lb = 1;
		s.bin |= dir2stick[cp->dir];
		return	s;
	}

	if (cp->sworddelay) {
		if (!(--cp->sworddelay)) s.flg.rb = 1;
		s.bin |= dir2stick[cp->dir];
		return	s;
	}
	cp->count++;

	tmp = lasercheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 144 ){
		cp->dir = tmp;
		cp->laserdelay = delay[cp->stage];
	}
	tmp = swordcheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 78 ){
		cp->dir = tmp;
		cp->sworddelay = 4;
	}
	return	s;
}

#define	MAXVALUE	1048576

/*
 *		Calculate the evaluation value for escaping from a laser 45 The closer it is, the bigger it is
 */

int	eval_laser( int x, int y, int ex, int ey )
{
	int	dx, dy;			/* Enemy position as seen from your perspective */
	int	dxy, dyx;		/* Enemy position as seen from your perspective */
	int	value = 0;		/* Evaluation value */

	dx = ex - x, dy = ey - y;
	dxy = dx - dy; dyx = dx + dy;
	if( dx == 0 ) value += 131072; else value += 65536 / dx;
	if( dy == 0 ) value += 131072; else value += 65536 / dy;
	if( dxy == 0 ) value += 131072; else value += 65536 / dxy;
	if( dyx == 0 ) value += 131072; else value += 65536 / dyx;
	return value;
}

/*		The opposite of eval_laser. Used for chasing.
 */

int	eval_laser2( int x, int y, int ex, int ey )
{
	return MAXVALUE - eval_laser(x, y, ex, ey);
}

/*
 */
 

STICK	decide_direction(int pl, int (* EvalFnc)(int x, int y, int ex, int ey))
{
	int	i, value, valuei, x, y;
	STICK	s;
	cpu_t	*cp;
	man	*mp, *emp;
	int	dir, flg, tmp;
	static	int	delay[STAGEMAX] = {20,12, 5, 3, 1, 1, 1, 1};

	cp = cpudata + pl;
	mp = &mandat[pl];	emp = &mandat[1 - pl];
	s.bin = 0;
	flg = 1;

	if (cp->laserdelay) {
		if (!(--cp->laserdelay)) s.flg.lb = 1;
		s.bin |= dir2stick[cp->dir];
		return	s;
	}

	if (cp->sworddelay) {
		if (!(--cp->sworddelay)) s.flg.rb = 1;
		s.bin |= dir2stick[cp->dir];
		return	s;
	}

	cp->count++;
	if (!(cp->count & 7)) {
		value = MAXVALUE; valuei = 0;
		for(i = 0; i < 8; i++){
			x = mp->x + COS(i) * MANV *2 / SINDIV;
			y = mp->y + SIN(i) * MANV *2 / SINDIV;
			if (x < MXMIN) x = MXMIN; else if (x > MXMAX) x = MXMAX;
			if (y < MYMIN) y = MYMIN; else if (y > MYMAX) y = MYMAX;
			tmp = (*EvalFnc)( x, y, emp->x, emp->y );
			if (value > tmp) {
				value = tmp;
				valuei = i;
			}
		}
		cp->dir = valuei;
	}

	tmp = lasercheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 144 ){
		cp->dir = tmp;
		cp->laserdelay = delay[cp->stage];
	}
	tmp = swordcheck(mp->x, mp->y, emp->x, emp->y);
	if( tmp >= 0 && mp->charge >= 78 ){
		cp->dir = tmp;
		cp->sworddelay = 4;
	}
	if (flg) s.bin |= dir2stick[cp->dir];
	return	s;
}


STICK	think(int pl)
{
	cpu_t	*cp;
	man	*mp, *emp;

	cp = cpudata + pl;
	mp = &mandat[pl];	emp = &mandat[1 - pl];

	switch (cpudata[pl].stage + 1) {
	  case 4:
	  case 7:
		return randomthink(pl,1);
		break;
	  case 2:
		if (cp->count & 16) return fixedthink(pl, 1);
		else	return randomthink(pl, 1);
		break;
	  case 3:
		if (cp->count & 4) return fixedthink(pl, 1);
		else	return randomthink(pl, 1);
		break;
	  case 6:
		loopthink(pl);
		break;
	  case 5:
		if (cp->mode == HOGE){
			if ((rand() % 32767) < 800) cp->mode = NIGE;
			return randomthink(pl, 1);
		} else {
			if ((rand() % 32767) < 2000) cp->mode = HOGE;
			return fixedthink(pl, 1);
		}
		break;
	  case 1:
	  case 8:
		if (mp->charge > 144) {
			if (emp->charge < 144) return decide_direction(pl, eval_laser2);
			else {
				if (cp->mode == HOGE){
					if ((rand() % 32767) < 800) cp->mode = LASER;
					return randomthink(pl, 1);
				} else {
					return decide_direction(pl, eval_laser2);
				}
			}
		} else {
			cp->mode = HOGE;
			if (emp->charge > 144) return decide_direction(pl, eval_laser);
			else return randomthink(pl, 1);
		}
		break;
	}
}
