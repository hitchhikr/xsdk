/*	star.c : star scroll program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"star.h"
#include	"main.h"
#include	"calc.h"
#include	"enemy.h"
#include	"dim.h"

int	delta[STAGEMAX] = {32, 16, 64, 24, 128, 8, 32, 0, 16};
int	bokan = -1;
STARDAT	star[STAR_NUM];

void	InitStar(void)
{
	int	i;

	for (i = 0; i < STAR_NUM; ++i) {
		switch (round_) {
			case	2:
				star[i].x = 896 - (i & 7) * 256;
				star[i].y = GROUND;
				star[i].z = 768 - (i / 8) * 512;
				break;
			case	4:
				star[i].x = 896 - (i & 7) * 256;
				star[i].y = TENJOU;
				star[i].z = 768 - (i / 8) * 512;
				break;
			case	5:
				star[i].x = (rand() & HALF) - HALF / 2;
				star[i].y = (rand() & HALF) - HALF / 2;
				star[i].z = (rand() & EDGE) - HALF;
				break;
			case	6:
				star[i].x = 896 - (i & 7) * 256;
				star[i].y = (i & 8 ? SEEBOT : SEETOP);
				star[i].z = 768 - (i / 8) * 512 + 256 * (i & 1);
				break;
			case	7:
				star[i].x = (i & 8 ? KABE : -KABE);
				star[i].y = 896 - (i & 7) * 256;
				star[i].z = 768 - (i / 8) * 512;
				break;
			default:
				star[i].x = (rand() & EDGE) - HALF;
				star[i].y = (rand() & EDGE) - HALF;
				star[i].z = (rand() & EDGE) - HALF;
				break;
		}
	}
}

void	S3Dto2D(STARDAT *starbuf)	/*	-HALF <= z <= HALF	*/
{
	unsigned short	*pos_ptr = (unsigned short *)pos_ + ((HALF - starbuf->z) / DIV) * (HALF / DIV + 1);
	
	starbuf->rx = ( starbuf->x >= 0
		? 128 + *( pos_ptr + starbuf->x / DIV )
		: 128 - *( pos_ptr - starbuf->x / DIV )
		);
	starbuf->ry = ( starbuf->y >= 0
		? 128 + *( pos_ptr + starbuf->y / DIV )
		: 128 - *( pos_ptr - starbuf->y / DIV )
		);
}

void	MkStar(int no)
{
	switch (round_) {
		case	2:
		case	4:
		case	6:
		case	7:
			star[no].z = -HALF;
			break;
		case	5:
			star[no].x = me.x + (rand() & HALF) - HALF / 2;
			star[no].y = me.y + (rand() & HALF) - HALF / 2;
			star[no].z = -HALF;
			break;
		case	9:
			star[no].x = -HALF;
			break;
		default:
			star[no].x = (rand() & EDGE) - HALF;
			star[no].y = (rand() & EDGE) - HALF;
			star[no].z = -HALF;
			break;
	}
}

void	MoveStar(void)
{
	int	i;
	STARDAT	starbuf;

	for (i = 0; i < STAR_NUM; ++i) {
		star[i].old_rx = star[i].rx;
		star[i].old_ry = star[i].ry;
		star[i].old_box = star[i].box;
		switch (round_) {
			case	4:
			case	6:
				if (0 == boss_appear_flag || 0 == boss_hp) star[i].z += delta[round_ - 1];
				break;
			case	7:
				if (!ending_flag) {
					if (0 == boss_appear_flag || 0 == boss_hp) {
						star[i].z += 32;
					} else {
						star[i].z += 16;
					}
				}
				break;
			case	9:
				star[i].x += delta[8];
				if (star[i].x > HALF) MkStar(i);
				break;
			default:
				star[i].z += delta[round_ - 1];
				break;
		}
		if (star[i].z > HALF) MkStar(i);
		starbuf.x = star[i].x - me.x;
		starbuf.y = star[i].y - me.y;
		starbuf.z = star[i].z;
		if ( starbuf.x > HALF || starbuf.x < -HALF || starbuf.y > HALF || starbuf.y < -HALF) {
			starbuf.rx = -1;
		} else {
			S3Dto2D(&starbuf);
		}
		star[i].rx = starbuf.rx;
		star[i].ry = starbuf.ry;
		if (star[i].z > (EDGE * 2 / 3 - HALF) ) {
			star[i].box = 2;
		} else if (star[i].z > (EDGE / 3 - HALF) ) {
			star[i].box = 1;
		} else {
			star[i].box = 0;
		}
	}
}
