#include	"stick.h"
#include	"battle.h"
#include	"man.h"
#include	"sound.h"
#include	"atari.h"
#include	"item.h"
#include	"textput.h"
#include	"cpu.h"
#include	"etc.h"

#define		SWORDLENGTH	200
#define		SWORDLENGTH2	90	/* Amount of shift to the right */
#define		LASERSPEED	(nowcnf->laser_speed)
#define		MANV	(nowcnf->man_speed)
#define		MANDV	(nowcnf->man_vspeed)
#define		LCHARGE	(nowcnf->charge_to_laser)
#define		SCHARGE	(nowcnf->charge_to_sword)

extern	int	enableshot;
struct	ID	{ int dir; int flg;};
static	struct ID	Idata[ 16 ] = {
	{-1,0},	/* 0 */	{ 0,1},	/* U */	{ 4,1},	/* D */	{-1,0},
	{ 6,1},	/* R */	{ 7,1},	/* RU*/	{ 5,1},	/* RD*/	{ 6,1},
	{ 2,1},	/* L */	{ 1,1},	/* LU*/	{ 3,1},	/* LD*/	{ 2,1},
	{-1,0},	/*LR */	{ 0,1},	/*LRU*/	{ 4,1},	/*LRD*/	{-1,0} };

/*
 *		lasermake
 */

static void lasermake( short x, short y, short dir, short type, short pl, short en)
{
	int	i = 0;
	laser	*lp = &(laserdat[pl]);

	if( lp->cond ) return;
	if( type == LASER ) {
		lp->x = x, lp->y = y;
		lp->vx = COS(dir) * LASERSPEED / SINDIV;
		lp->vy = SIN(dir) * LASERSPEED / SINDIV;
		lp->cond = 64;
	} else {
		lp->x = x + COS(dir) * SWORDLENGTH / SINDIV;
		lp->y = y + SIN(dir) * SWORDLENGTH / SINDIV;
		lp->x += COS(dir +2 ) * SWORDLENGTH2 / SINDIV;
		lp->y += SIN(dir +2 ) * SWORDLENGTH2 / SINDIV;
		lp->vx = lp->vy = 0;
		lp->cond = 20;
	}
	lp->dir = dir;
	lp->type = type;
	lp->energy = en;
}


/*
 *		manmove
 */

void	manmove( int player )
{
	int	flg, dir, tmp;
	short	x, y;
	man	*mp;
	STICK	s;

	mp = &(mandat[ player ]);
	if (mp->hit > 0) {
		mp->hit--;
	}
	if (mp->dead_mode) {
		mp->dir = (mp->dir + 1) & 7;
		mp->dead_mode ++;
		mp->x = mp->x + mp->vx / 2;
		mp->y = mp->y + mp->vy / 2;
		mp->vx = mp->vx * 14  / 16;
		mp->vy = mp->vy * 14  / 16;
		return;
	}
	if (mp->cond > 0) { mp->cond--; return; }

	s = JOY(player);
	switch (commode[player]) {
	  case MODE_HUMAN:		break;
	  case MODE_CPU:
		if( s.flg.lb || s.flg.rb ) { nmiflg = player + 1; return; }
		s = think(player);
		break;
	  default:
		error("something is wrong.");		break;
	}
	dir = Idata[ s.bin & 15 ].dir;
	flg = Idata[ s.bin & 15 ].flg;

	if( mp->hit > nowcnf->inv_time - nowcnf->wait_sword ) {
		x = mp->x + mp->vx;
		y = mp->y + mp->vy;
		mp->vx = mp->vx * 14  / 16;
		mp->vy = mp->vy * 14  / 16;
		if (x < MXMIN) x = MXMIN; else if (x > MXMAX) x = MXMAX;
		if (y < MYMIN) y = MYMIN; else if (y > MYMAX) y = MYMAX;
		if (is_wall(mp->x, mp->y)) {
			mp->dead_mode = 1; mp->hit = 5;
			tx_puts_big(4, 7, "  RING OUT  ", BIG_BASENUM);
			if (mp->energy > 0) {
				mp->life--;	mp->energy = 0;
			}
		}
		mp->x = x, mp->y = y;
		return;
	} else {
		if( dir >= 0 ){
			mp->dir = dir;
		}
		if( flg ){
			mp->Random++;
			x = mp->x + COS(dir) * MANV / SINDIV;
			y = mp->y + SIN(dir) * MANV / SINDIV;
			if (x < MXMIN) x = MXMIN; else if (x > MXMAX) x = MXMAX;
			if (y < MYMIN) y = MYMIN; else if (y > MYMAX) y = MYMAX;
			if( !is_man( x, y, 1 - player ) ) {
				if( !is_wall( x, y ) ) mp->x = x, mp->y = y;
				else if( !is_wall( x, mp->y ) ) mp->x = x;
				else if( !is_wall( mp->x, y ) ) mp->y = y;
			}
		}
	}

	if (mp->hit > 20) return;
	if( s.flg.lb ) {
		if( (!mp->lbflg) && mp->charge > LCHARGE && enableshot) {
			tmp = mp->charge - LCHARGE;
			tmp = tmp * nowcnf->mul_laser / CONFIG_MULDIV +
				nowcnf->d_min_laser;
			lasermake(mp->x, mp->y, mp->dir, LASER, player, tmp);
			mp->cond += nowcnf->wait_laser;
			mp->charge = 0;
			sd_on( SD_LASER );
		}
		mp->lbflg = 1;
	} else mp->lbflg = 0;

	if( s.flg.rb ) {
		if ((!mp->rbflg) && mp->charge > SCHARGE && enableshot) {
			mp->Random = 5;
			tmp = mp->charge - SCHARGE;
			tmp = tmp * nowcnf->mul_sword / CONFIG_MULDIV
				+ nowcnf->d_min_sword;
			lasermake(mp->x, mp->y, mp->dir, SWORD, player, tmp);
			mp->cond += nowcnf->wait_sword;
			mp->charge = 0;
			sd_on( SD_SHOT );
		}
		mp->rbflg = 1;
	} else mp->rbflg = 0;
}

void	lasermove( void )
{
	laser	*lp;
	int	i, pat;

	for( i = 0; i < PLAYERNUM; i++ ){
		lp = &(laserdat[ i ]);
		if( !lp->cond ) continue;
		lp->cond--;
		if( lp->type == LASER ){
/*			make_laseritem( lp->x + lp->vx  , lp->y + lp->vy  , i, lp->dir);	*/
			make_laseritem( lp->x + lp->vx/2, lp->y + lp->vy/2, i, lp->dir);
		}
		lp->x += lp->vx; lp->y += lp->vy;
		if( is_wall2( lp->x, lp->y ) ) lp->cond =0;
	}
}


/*
 *		INITIALIZE
 */

struct location {
	short	x, y, dir;
};

void maninit( void )
{
	struct location initial_location[ PLAYERNUM ] = {
		{ MXMIN, MYMIN, 3 }, { MXMAX, MYMAX, 7 }
	};
	int	i;
	man	*tp = mandat;
	laser	*lp = laserdat;

	for( i = 0; i < PLAYERNUM; i++,tp++,lp++ ){
		tp->cond = 1;
		tp->charge = 0;
		tp->x = initial_location[i].x;
		tp->y = initial_location[i].y;
		tp->dir = initial_location[i].dir;
		tp->lbflg = tp->rbflg = 1;
		tp->energy = nowcnf->energy_max;
		tp->hit = 19;
		tp->dead_mode = 0;

		lp->cond = 0;

	}
}

/*
 *		Add 1 charge
 */
void addcharge( void )
{
	man	*mp = mandat;
	if (++(mp->charge) > nowcnf->charge_max) mp->charge = nowcnf->charge_max;
	mp++;
	if (++(mp->charge) > nowcnf->charge_max) mp->charge = nowcnf->charge_max;
}

