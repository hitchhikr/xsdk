/*
 *		EneKill.C
 */

#include		<stdlib.h>
#include		"sps.h"
#include		"enemy.h"
#include		"dsp.h"
#include		"sound.h"
#include		"item.h"
#include		"txput.h"

extern int   whitecnt;						//Screen Flash Counter
extern short enemy_left;					//Add (Peach)
extern short enemynum;						//Add (Peach)
extern int   score;
extern ENEMY ene_dat[];
extern short   ene_num;
extern void EneMove_crash(ENEMY *, short, short);		//Add (Peach)

const	static	int	scoretbl[20] = {
	0, 0, 100, 50, 10, 20, 10, 40, 40, 10, 20, 40,
};

#define   ABS(data)   ((data) > 0 ? data : - (data))

#define   YOKO   (600 * DIV_X)
#define   TATE   (400 * DIV_Y)

/*
 *	I changed it to make it explode. << Momo
 */

void
EnemyKill(ENEMY *data, short scx, short scy) {
	short i;

	for(i = 0; i < ene_num; i ++, data ++) {
		if (ENE_TYPE(*data) < 0) continue;

		if (ENE_ENERGY(*data) <= 0) {
			if (ENE_TYPE(*data) != TARAI_TYPE) {
				enemynum--;
				enemy_left--;
				tx_putn(12, 28, enemy_left, 2, 15);
				sd_on(20);
/*				whitecnt += 20;		*/
			} else {
				sd_on(20);
				whitecnt += 50;
			}
			score += scoretbl[ENE_TYPE(*data)];

			ENE_ENERGY(*data) = 1;
			/*	Changes TYPE to Explosion.	*/

			if (ENE_SP(*data) != (void *)0) {
				SpsFree(ENE_SP(*data));
				ENE_SP(*data) = (void *)0;
			}
			if (ENE_SHADOW(*data) != (void *)0) {
				SpsFree(ENE_SHADOW(*data));
				ENE_SHADOW(*data) = (void *)0;
			}
			ENE_TYPE(*data) = CRASH_TYPE;
			IS_ENE_MOVE(*data) = EneMove_crash;
			data->MotionType = 0;
			data->MotionCnt = 0;

		}
	}
}

