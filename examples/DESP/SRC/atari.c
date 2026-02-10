#include	"sps.h"

#include	"dsp.h"
#include	"enemy.h"
#include	"atari.h"
#include	"item.h"

#define   SHOT_RANGE   (36*16)
#define   CHR_RANGE   (16*16)
#define   B_RANGE   (32*32)
#define   BOMB_RANGE  (16*16)
#define   FLASH_PALET   15
#define   HIT_COUNT   3
#define   BOMB_AT   1
#define   SHOT_AT   1
#define   ENE_AT   1
#define   DSP_AT   1

#define	BrickVX		128
#define	BrickVX2	64
#define	BrickPalet	14
#define	MakeBrickSpeed	128

#define   ABS(data)   ((data) >= 0 ? (data) : -(data))
extern	int	score;
extern	int	count_;

short
atari(DSP *dspdat, SHOT *shotdat, ENEMY *enedat, BOMB *bombdat) {
	short i;
	short dspx, dspy;

	dspx = dspdat->x;
	dspy = dspdat->y;

	if (dspdat->hit) {
		if (!(-- (dspdat->hit))) {
			(dspdat->sp)->palet = dspdat->palet;
		}
	} else {
		for(i = 0; i < BOMB_NUM; i ++) {
			short bombx, bomby;

			if ((bombdat + i)->type < 0) continue;

			bombx = (bombdat + i)->x;
			bomby = (bombdat + i)->y;

			if (ABS(bombx - dspx) < BOMB_RANGE
				 && ABS(bomby - dspy) < BOMB_RANGE
				 && dspdat->atariflg) {
				// Make shards.
				MakeBricks(bombx, bomby, (bombdat+i)->vx,
					dspdat->vx, dspdat->vy, 1);

				// FLASH !!
				(dspdat->sp)->palet = FLASH_PALET;
				dspdat->hit = HIT_COUNT;
				// ENERGY DOWN
				dspdat->energy -= (dspdat->energy >= BOMB_AT ? BOMB_AT : 0);
			} else if (ABS(bombx - dspx) < B_RANGE && ABS(bomby - dspy) < B_RANGE) {
//	MakeBrick
				if(ABS(dspdat->vx - (bombdat + i) -> vx) > MakeBrickSpeed) {
					if((count_ & 1))
						Make1Brick((bombx+dspx)/2, (bomby+dspy)/2,
							-BrickVX, 0, BrickPalet);
					else
						Make1Brick((bombx+dspx)/2, (bomby+dspy)/2,
							BrickVX, 0, BrickPalet);
				}
			}
		}
	}

	for(i = 0; i < ENE_NUM; i ++) {
		if ((enedat + i)->atariflg == 0 || (enedat+i)->type < 0) continue;
		if ((enedat + i)->hit) {
			if (!(-- (enedat + i)->hit)) {
				if ((enedat + i)->sp) {
					((enedat + i)->sp)->palet = (enedat + i)->palet;
				}
			}
		} else {
			short j, xx, yy;

			xx = (enedat + i)->x;
			yy = (enedat + i)->y;

			if (ABS(xx - dspx) < CHR_RANGE && ABS(yy - dspy) < CHR_RANGE && dspdat->atariflg) {
				// FLASH !!
				(dspdat->sp)->palet = FLASH_PALET;
				if ((enedat + i)->sp) {
					((enedat + i)->sp)->palet = FLASH_PALET;
				}
				dspdat->hit = HIT_COUNT;
				(enedat + i)->hit = HIT_COUNT;

				// ENERGY DOWN
				(enedat + i)->energy -=
					((enedat + i)->energy >= DSP_AT ? DSP_AT : 0);
				dspdat->energy -=
					(dspdat->energy >= ENE_AT ? ENE_AT : 0);
			} else if (ABS(xx - dspx) < B_RANGE && ABS(yy - dspy) < B_RANGE) {
//	MakeBrick
				if(ABS(dspdat->vx - (enedat + i) -> vx) < MakeBrickSpeed) {
/*
					if((count_ & 15) == 0)
						Make1Brick((xx+dspx)/2, (yy+dspy)/2,
							-BrickVX2, 0, BrickPalet);
					if((count_ & 15) == 6)
						Make1Brick((xx+dspx)/2, (yy+dspy)/2,
							BrickVX2, 0, BrickPalet);
*/
				} else {
					if((count_ & 1))
						Make1Brick((xx+dspx)/2, (yy+dspy)/2,
							-BrickVX, 0, BrickPalet);
					else
						Make1Brick((xx+dspx)/2, (yy+dspy)/2,
							BrickVX, 0, BrickPalet);
				}
			}

			for(j = 0; j < SHOT_NUM; j ++) {
				if ((shotdat+ j) -> condition
					 && ABS((shotdat + j)->x - xx) < SHOT_RANGE
					 && ABS((shotdat + j)->y - yy) < SHOT_RANGE) {
					// FLASH !!
					((enedat + i)->sp)->palet = FLASH_PALET;
					(enedat + i)->hit = HIT_COUNT;
					// Make shards.
					MakeBricks(xx, yy, (shotdat+j)->vx,
						(enedat+i)->vx, (enedat+i)->vy, (enedat+i)->palet);

					if((shotdat + j)->condition < 10) {
						score += 20;
						if((enedat + i)->energy ) 
							(enedat + i)->energy = 0;
						if((shotdat + j)->condition < 5) score += 30;
					}
					(shotdat + j)->condition = 0;
					if ((shotdat + j)->sp) {
						SpsFree((shotdat + j)->sp);				//It seems dangerous, but never mind.
					}
					(shotdat + j)->sp = (void*) 0;

					// ENERGY DOWN
					(enedat + i)->energy -=
						((enedat + i)->energy >= SHOT_AT ? SHOT_AT : 0);
				}
			}
		}
	}
	return 0;
}
