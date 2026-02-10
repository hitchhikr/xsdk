#include	"stdmomo.h"
#include	"battle.h"
#include	"item.h"
#include	"sound.h"
#include	"atari.h"
#include	"stage.h"
#include	"bg.h"

#define	MAN_WIDTH	( 16 * 6 )
#define	LASER_WIDTH	( 16 * 6 )

#define	HITCOUNT	(nowcnf->inv_time)	/* Invincible time after hit */
#define	HITCOUNT2	1000	/* Death */
#define	HITCOUNT3	488	/* Down */
int is_wall( int x, int y )
{
	if (x < XMIN) return 1;
	if (y < YMIN) return 1;
	if (x > XMAX) return 1;
	if (y > YMAX) return 1;
	if (yukadata(x, y) == 0) return 1;
	return 0;
}
int is_wall2( int x, int y )
{
	if (x < XMIN) return 1;
	if (y < YMIN) return 1;
	if (x > XMAX) return 1;
	if (y > YMAX) return 1;
	return 0;
}

int is_man( int x, int y, int manID )
{
	man	*mp = &(mandat[ manID ]);

	if (ABS(mp->x-x) < MAN_WIDTH && ABS(mp->y-y) < MAN_WIDTH) return 1;
	return 0;
}

void	atari( void )
{
	man	*mp = mandat;
	laser	*lp = laserdat;
	int	i, j;

	for (i = 0; i < PLAYERNUM; i++, mp++) {
		if (mp->hit > 20 && (mp->hit & 3) != 0 ) {
			if( mp->hit < 500 || mp->hit > 880 )
			makebrickr( mp->hitx + mp->x, mp->hity + mp->y,
				COS(mp->hitdir)/4, SIN(mp->hitdir)/4 );
		}
		if (mp->hit || mp->dead_mode) continue;
		lp = laserdat;
		for (j = 0; j < PLAYERNUM; j++, lp++) {
			if (i == j) continue;	/* It doesn't matter if it hits me */
			if (!lp->cond) continue;

			if (ABS(mp->x - lp->x) < MAN_WIDTH+LASER_WIDTH
			 && ABS(mp->y - lp->y) < MAN_WIDTH+LASER_WIDTH) {
			 	/* HIT */
				/* HIT PARAM SET */
				mp->hitdir = lp->dir;
				mp->hitx = (lp->x - mp->x) / 2;
				mp->hity = (lp->y - mp->y) / 2;
/**/
				mp->vx = COS(mp->hitdir)/4;
				mp->vy = SIN(mp->hitdir)/4;
/**/
				/* ENERGY DOWN */
				mp->energy -= lp->energy;
				if( mp->energy <= 0 ) {
					if( --mp->life > 0 ) mp->hit = HITCOUNT3;
					else mp->hit = HITCOUNT2;
					sd_on( SD_DOWN );
				} else {
					mp->hit = HITCOUNT;
					sd_on( SD_DAMAGE );
				}
				lp->energy = 0;
				if (lp->type == LASER )	lp->cond = 0;
			}
		}
	}
}
