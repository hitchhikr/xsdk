/*	move.c : enemy move program	*/

#include	<stdio.h>
#include	<math.h>
#include	"main.h"
#include	"calc.h"
#include	"matrix.h"
#include	"enemy.h"
#include	"dim.h"
#include	"motion.h"
#include	"motlocal.h"

#define		STICK_LASER_SPEED	(48 * ENEMY_SPEED_DIV)
#define		RING_LASER_SPEED	(24 * ENEMY_SPEED_DIV)
#define		STICK_MODEL		0
#define		RING_MODEL		1

static int	shotkind_to_model[2] = {STICK_MODEL, RING_MODEL};

void	CalcShotDeltaToMe(short px, short py, short pz, short *dx, short *dy, short *dz)
{
	short	x = me.x - px, y = me.y - py, z = HALF - pz;
	int	adjuster = sqrt( (double)x * x + y * y + z * z );

	*dx = (short)((int)x * RING_LASER_SPEED / adjuster);
	*dy = (short)((int)y * RING_LASER_SPEED / adjuster);
	*dz = (short)((int)z * RING_LASER_SPEED / adjuster);
}

void	AppearEnemy(ENEMY *enemy, int flag, short x, short y, short z, unsigned char rx, unsigned char ry, unsigned char rz)
{
	int kind = enemy->mot_kind, sx = enemy->sign_x, sy = enemy->sign_y;

	if (flag) {
		(enemy->pos).x = x;	(enemy->pos).y = y;	(enemy->pos).z = z;
		if (2 == flag) {
			(enemy->rot_m).x = rx;	(enemy->rot_m).y = ry;	(enemy->rot_m).z = rz;
			(enemy->rot_d).x = rx;	(enemy->rot_d).y = ry;
		}
	} else {
		short	xx = sx * (motionbuf[kind]->pos).x, yy = sy * (motionbuf[kind]->pos).y;
		
		/*	If it's an odd number, it's referred to as the difference with your own ship. */
		if (xx & 1) xx = xx + me.x;	if (yy & 1) yy = yy + me.y;
		(enemy->pos).x = xx;	(enemy->pos).y = yy;
		(enemy->pos).z = (motionbuf[kind]->pos).z;
	}
	if (LASER == kind) {
		short	cx = 0, cy = 0, cz = STICK_LASER_SPEED;
		
		if (RING_MODEL == enemy->mod_kind) {
			(enemy->rot_m).y = (enemy->rot_m).x = (enemy->rot_m).z = 0;
			CalcShotDeltaToMe(x, y, z, &cx, &cy, &cz);
		} else {
			(enemy->rot_m).y = ry;	(enemy->rot_m).x = rx;	(enemy->rot_m).z = rz;
			(enemy->rot_d).y = ry;	(enemy->rot_d).x = rx;
			RoundXY(&cx, &cy, &cz, rx, ry);
		}
		(enemy->delta).x = cx;	(enemy->delta).y = cy;	(enemy->delta).z = cz - ENEMY_SPEED_DIV * z_scroll[ round_ - 1 ];
		enemy->counter = 0;
	} else {
		if (2 != flag) {
			(enemy->rot_m).y = sx * (motionbuf[kind]->rot).y;
			(enemy->rot_m).x = sy * (motionbuf[kind]->rot).x;
			(enemy->rot_m).z = sx * (motionbuf[kind]->rot).z;
			(enemy->rot_d).y = sx * (motionbuf[kind]->vec).y;
			(enemy->rot_d).x = sy * (motionbuf[kind]->vec).x;
		}
		enemy->speed = motionbuf[kind]->veloc;
		enemy->counter = motionbuf[kind]->count;
		enemy->step = motionbuf[kind];
	}
}

void	DoMotion(ENEMY *enemy)
{
	int	kind = enemy->mot_kind, sx = enemy->sign_x, sy = enemy->sign_y;
	MOTION	*tmp;

	/*	laser check	*/
	if (LASER == kind) {
		++enemy->counter;
		if (RING_MODEL == enemy->mod_kind) (enemy->rot_m).z += 16;
		return;
	}
	/*	next step check	*/
	if (0 == --enemy->counter) {
		if (loopbuf[kind].end == enemy->step) {
			enemy->step = loopbuf[kind].start;
		}
		enemy->step = (enemy->step)->n;
		enemy->counter = (enemy->step)->count;
	}
	tmp = enemy->step;
	/*	do motion	*/
	enemy->speed += tmp->veloc;
	(enemy->pos).x += sx * (tmp->pos).x;
	(enemy->pos).y += sy * (tmp->pos).y;
	(enemy->pos).z += (tmp->pos).z;
	(enemy->rot_m).x += sy * (tmp->rot).x;
	(enemy->rot_m).y += sx * (tmp->rot).y;
	(enemy->rot_m).z += sx * (tmp->rot).z;
	(enemy->rot_d).x += sy * (tmp->vec).x;
	(enemy->rot_d).y += sx * (tmp->vec).y;
	/*	shot check	*/
	if (enemy->counter == (tmp->shot).count) {
		BornEnemy(shotkind_to_model[(tmp->shot).kind], LASER, 1, 1, 1, (enemy->pos).x, (enemy->pos).y, (enemy->pos).z, (enemy->rot_m).x, (enemy->rot_m).y, (enemy->rot_m).z);
	}
	/*	born check	*/
	if (enemy->counter == (tmp->born).count && enemy->hit_point) {
		BornEnemy( (tmp->born).mod_k, (tmp->born).mot_k, sx * (tmp->born).sign, 1, (tmp->born).flag + 1, (enemy->pos).x, (enemy->pos).y, (enemy->pos).z, (enemy->rot_m).x, (enemy->rot_m).y, (enemy->rot_m).z);
	}
}
