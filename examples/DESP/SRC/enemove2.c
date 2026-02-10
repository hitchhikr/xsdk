/*
 *		enemove2.c
 *				Programmed by chika@kmc	1992
 */



#include		"sps.h"
#include		"dsp.h"
#include		"enemy.h"
#include		"display.h"
#include		"sound.h"
#include		"item.h"

#define	_ABS(x)	(((x)>=0) ? (x) : -(x))

#define	NULL	(void *)0
#define	MX3(mfadr, mt, mc)	*((char *)mfadr + *(int *)((char *)mfadr + 8*mt) + mc * 8 + 6)
#define	MY3(mfadr, mt, mc)	*((char *)mfadr + *(int *)((char *)mfadr + 8*mt) + mc * 8 + 7)

extern DSP	dspdata;
extern ENEMY	ene_dat[];
extern short	*pat_buff[];
extern short	*mf_buff[];
extern int	count_;
extern int	damagecnt;
extern short	sintbl[];
extern short	rank;						/* Difficulty */

void
EnemyDisp(ENEMY *data, int chrsize, unsigned char *spradr, short scx, short scy);


short	tantbl[32] = { 0, 0, 1, 1, 1, 2, 2, 2, 2, 3,
			 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
			 6, 6, 6, 6, 7, 7, 7, 7, 7, 7,
			 8, 8 };

/*
   Calculates the angle between coordinates (x2,y2) as seen from (x1,y1)).
   The returned value, clockwise, is:

        0
     48 + 16
        32

   However, if the coordinates are too close together,
   the returned value will be strange.
*/

int	CalcDeg(short x1, short y1, short x2, short y2)
{
	short	dx, dy;

	dx = (x2 - x1) & 0xffff;
	dy = y2 - y1;

	if (dx <= 31 && dx >= -31) {
		if (dy > 0) {
			return 32;
		} else {
			return 0;
		}
	} else if (dy <= 31 && dy >= -31) {
		if (dx > 0) {
			return 16;
		} else {
			return 48;
		}
	}

	if (dx > 0) {
		if (dy < 0) {						// 0-16
			dy = -dy;
			if (dx == dy) {
				return 8;
			} else if (dy > dx) {
				return tantbl[dx / (dy / 32)];
			} else {
				return 16 - tantbl[dy / (dx / 32)];
			}
		} else {						// 16-32
			if (dx == dy) {
				return 24;
			} else if (dy > dx) {
				return 32 - tantbl[dx / (dy / 32)];
			} else {
				return 16 + tantbl[dy / (dx / 32)];
			}
		}
	} else {
		dx = -dx;
		if (dy > 0) {						// 32-48
			if (dx == dy) {
				return 40;
			} else if (dy > dx) {
				return 32 + tantbl[dx / (dy / 32)];
			} else {
				return 48 - tantbl[dy / (dx / 32)];
			}
		} else {						// 48-63
			dy = -dy;
			if (dx == dy) {
				return 56;
			} else if (dy > dx) {
				return (64 - tantbl[dx / (dy / 32)]) & 63;
			} else {
				return 48 + tantbl[dy / (dx / 32)];
			}
		}
	}
	return -1;
}

/*
 *		Scorpion
 */

#define		SCO_XSPEED	(80 - rank)
#define		SCO_WAIT	60	//	Storing period
#define		SCO_YMAX	8400

void	EneMove_sco(ENEMY *data, short scx, short scy)
{
	int	number;
	short	deg;
	int	mtype, mcnt;

	deg = CalcDeg(ENE_X(*data), ENE_Y(*data), dspdata.x, dspdata.y);

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	if (ENE_Y(*data) < SCO_YMAX) {
		ENE_VY(*data) = 64;
	} else {
		ENE_VY(*data) = 0;
	}
	mtype = data->MotionType;
	mcnt = data->MotionCnt;
	if (mtype >= 0) {
		number = MOTION(mf_buff[SCO_TYPE], mtype, data->MotionCnt);
		if (mtype == 0) {
			short	tx, ty;
			ENE_VX(*data) = 0;
			tx = ENE_X(*data) +
			  MX3(mf_buff[SCO_TYPE], mtype, data->MotionCnt) * DIV_X;
			ty = ENE_Y(*data) +
			  MY3(mf_buff[SCO_TYPE], mtype, data->MotionCnt) * DIV_Y;
			if (mcnt == 62) makebomb2(tx,ty,3,60);
			if (mcnt == 67) makebomb2(tx,ty,3,56);
			if (mcnt == 72) makebomb2(tx,ty,3,52);
			if (mcnt == 78) makebomb2(tx,ty,3,49);
		} else if (mtype == 1) {
			short	tx, ty;
			ENE_VX(*data) = 0;
			tx = ENE_X(*data) +
			  MX3(mf_buff[SCO_TYPE], mtype, data->MotionCnt) * DIV_X;
			ty = ENE_Y(*data) +
			  MY3(mf_buff[SCO_TYPE], mtype, data->MotionCnt) * DIV_Y;
			if (mcnt == 62) makebomb2(tx,ty,3, 4);
			if (mcnt == 67) makebomb2(tx,ty,3, 8);
			if (mcnt == 72) makebomb2(tx,ty,3,12);
			if (mcnt == 78) makebomb2(tx,ty,3,15);
		}
		data->MotionCnt++;
		if (MOTION(mf_buff[SCO_TYPE], mtype, data->MotionCnt) < 0) {
			switch(mtype) {
				case 0:
				case 1:
					ENE_BOMBCNT(*data) = SCO_WAIT;
					break;
			}
			data->MotionCnt = 0;
			data->MotionType = -1;
		}
	} else {
		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -SCO_XSPEED;
		} else {
			ENE_VX(*data) = SCO_XSPEED;
		}
		if (data->dummy1) {
			number = 0;
			if (deg < 60 && deg >=48 && ENE_BOMBCNT(*data) == 0) {
				data->MotionType = 0;
			}
			if (deg < 32) {
				data->MotionType = 2;
				data->dummy1 = !data->dummy1;
			}
		} else {
			number = 12;
			if (deg >= 0  && deg < 10 && ENE_BOMBCNT(*data) == 0) {
				data->MotionType = 1;
			}
			if (deg > 32) {
				data->MotionType = 3;
				data->dummy1 = !data->dummy1;
			}
		}
	}
	if (ENE_BOMBCNT(*data) > 0) ENE_BOMBCNT(*data)--;
	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	ENE_PALET(*data) = 8;

	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[SCO_TYPE], number), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}



/*
 *		KAME KAME
 */

#define	SHOTRANGE	512
#define	KAME_VX		48
#define	KAME_VY		48
#define	KAME_VVY	2
#define	KAME_WAIT	240
#define	KAME_RADIUS	127 * DIV_X

void	EneMove_kame(ENEMY *data, short scx, short scy)
{

	int	number;
	short	dx;
	int	mtype = data->MotionType;

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	if (mtype >= 0) {
		number = MOTION(mf_buff[KAME_TYPE], mtype, data->MotionCnt);
		switch(mtype) {
		  case 0:
			if (_ABS(ENE_Y(*data) - dspdata.y) < SHOTRANGE) {
				data->MotionCnt = -1;
				data->MotionType = 4;
			} else {
				if (ENE_VY(*data) > -KAME_VY) ENE_VY(*data) -= KAME_VVY;
			}
			break;
		  case 1:
			if (_ABS(ENE_Y(*data) - dspdata.y) < SHOTRANGE) {
				data->MotionCnt = -1;
				data->MotionType = 4;
			} else {
				if (ENE_VY(*data) < KAME_VY) ENE_VY(*data) += KAME_VVY;
			}
			break;
		  case 2:
			if (_ABS(ENE_Y(*data) - dspdata.y) < SHOTRANGE) {
				data->MotionCnt = -1;
				data->MotionType = 5;
			} else {
				if (ENE_VY(*data) > -KAME_VY) ENE_VY(*data) -= KAME_VVY;
			}
			break;
		  case 3:
			if (_ABS(ENE_Y(*data) - dspdata.y) < SHOTRANGE) {
				data->MotionCnt = -1;
				data->MotionType = 5;
			} else {
				if (ENE_VY(*data) < KAME_VY) ENE_VY(*data) += KAME_VVY;
			}
			break;
		  case 4:
			if (ENE_VY(*data) > 0) ENE_VY(*data) -= KAME_VVY;
			if (ENE_VY(*data) < 0) ENE_VY(*data) += KAME_VVY;
			if (data->MotionCnt == 14) {
				makebomb2(ENE_X(*data), ENE_Y(*data)+256, 0, -1);
			}
			break;
		  case 5:
			if (ENE_VY(*data) > 0) ENE_VY(*data) -= KAME_VVY;
			if (ENE_VY(*data) < 0) ENE_VY(*data) += KAME_VVY;
			if (data->MotionCnt == 14) {
				makebomb2(ENE_X(*data), ENE_Y(*data)+256, 0, 1);
			}
			break;
		}
		data->MotionCnt++;
		if (MOTION(mf_buff[KAME_TYPE], mtype, data->MotionCnt) < 0) {
			data->MotionCnt = 0;
			data->MotionType = -1;
			ENE_BOMBCNT(*data) = KAME_WAIT;
		}

	} else {

		ENE_VY(*data) = 0;
		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -KAME_VX;
			number = MOTION(mf_buff[KAME_TYPE], 0, 0);
		} else {
			ENE_VX(*data) = KAME_VX;
			number = MOTION(mf_buff[KAME_TYPE], 2, 0);
		}
		if (ENE_BOMBCNT(*data)) {
			ENE_BOMBCNT(*data) --;
		} else {
			dx = (ENE_X(*data) - dspdata.x) & 0xffff;
								/* Despy is on the left */
			if (dx < KAME_RADIUS && dx >= 0) {
				if (ENE_VX(*data) < 0) {
					if (ENE_Y(*data) > dspdata.y) {
						data->MotionType = 0;
					} else {
						data->MotionType = 1;
					}
				}
								/* Despy is on the right */
			} else if (dx > -KAME_RADIUS && dx <= 0) {
				if (ENE_VX(*data) > 0) {
					if (ENE_Y(*data) > dspdata.y) {
						data->MotionType = 2;
					} else {
						data->MotionType = 3;
					}
				}
			}
		}

	}
	
	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;
	ENE_PALET(*data) = 9;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[KAME_TYPE], number), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}


/*
 *		EFIGHTER
 */

#define	EF_VXMAX	(64 + rank)
#define	EF_VX		(64 + rank)
#define	EF_VYMAX	128
#define	EF_VVY		6
#define	EF_VVX		4
#define	EF_WAIT		240
#define	EF_RADIUS	127 * DIV_X

void	EneMove_ef(ENEMY *data, short scx, short scy)
{

	int	number, mcnt;
	short	dx;
	int	mtype = data->MotionType;

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	if (mtype >= 0) {
		mcnt = data->MotionCnt;
		number = MOTION(mf_buff[EF_TYPE], mtype, mcnt);
		switch(mtype) {
		  case 0:
			if (mcnt < 16) {
				if (ENE_VY(*data) >= -EF_VYMAX) {
					ENE_VY(*data) -= EF_VVY;
				}
			} else {
				if (ENE_VX(*data) < 0) ENE_VX(*data) += EF_VVX;
				if (ENE_VY(*data) < 0) ENE_VY(*data) += EF_VVY;
			}
			break;
		  case 1:
			if (mcnt == 10 && rank > 20)
				makebomb(data->x, data->y, 3);
			if (mcnt == 16 && rank > 24)
				makebomb(data->x, data->y, 3);
			if (mcnt < 24) {
				if (ENE_VY(*data) <= EF_VYMAX) {
					ENE_VY(*data) += EF_VVY;
				}
			} else {
				if (ENE_VY(*data) > 0) ENE_VY(*data) -= EF_VVY;
			}
			if (ENE_VX(*data) >= -EF_VXMAX) ENE_VX(*data) -= EF_VVX / 2;
			break;
		  case 2:
			if (ENE_VX(*data) >= -EF_VXMAX) ENE_VX(*data) -= EF_VVX / 2;
			if (mcnt < 12) {
				if (ENE_VY(*data) >= -EF_VYMAX) {
					ENE_VY(*data) -= EF_VVY;
				}
			} else {
				if (ENE_VY(*data) < 0) ENE_VY(*data) += EF_VVY / 2;
			}
			break;

		  case 3:
			if (mcnt < 16) {
				if (ENE_VY(*data) <= -EF_VYMAX) {
					ENE_VY(*data) -= EF_VVY;
				}
			} else {
				if (ENE_VX(*data) > 0) ENE_VX(*data) -= EF_VVX;
				if (ENE_VY(*data) < 0) ENE_VY(*data) += EF_VVY;
			}
			break;
		  case 4:
			if (mcnt == 10 && rank > 20)
				makebomb(data->x, data->y, 3);
			if (mcnt == 16 && rank > 24)
				makebomb(data->x, data->y, 3);
			if (mcnt < 24) {
				if (ENE_VY(*data) <= EF_VYMAX) {
					ENE_VY(*data) += EF_VVY;
				}
			} else {
				if (ENE_VY(*data) > 0) ENE_VY(*data) -= EF_VVY;
			}
			if (ENE_VX(*data) <= -EF_VXMAX) ENE_VX(*data) += EF_VVX / 2;
			break;
		  case 5:
			if (ENE_VX(*data) <= EF_VXMAX) ENE_VX(*data) += EF_VVX / 2;
			if (mcnt < 12) {
				if (ENE_VY(*data) >= -EF_VYMAX) {
					ENE_VY(*data) -= EF_VVY;
				}
			} else {
				if (ENE_VY(*data) < 0) ENE_VY(*data) += EF_VVY / 2;
			}
			break;
		}
		data->MotionCnt++;
		if (MOTION(mf_buff[EF_TYPE], mtype, data->MotionCnt) < 0) {
			data->MotionCnt = 0;
			switch(mtype) {
			  case 0:
			  case 1:
			  case 3:
			  case 4:
				data->MotionType ++;
				break;
			  case 2:
			  case 5:
				data->MotionType = -1;
			}
			ENE_BOMBCNT(*data) = EF_WAIT;
		}

	} else {

		ENE_VY(*data) = 0;
		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -EF_VX;
			number = MOTION(mf_buff[EF_TYPE], 0, 0);
		} else {
			ENE_VX(*data) = EF_VX;
			number = MOTION(mf_buff[EF_TYPE], 3, 0);
		}
		if (ENE_BOMBCNT(*data)) {
			ENE_BOMBCNT(*data) --;
		} else {
			dx = (ENE_X(*data) - dspdata.x) & 0xffff;
								/* Despy is on the left */
			if (dx < EF_RADIUS && dx >= 0) {
				if (ENE_VX(*data) < 0) {
					data->MotionType = 0;
				}
								/* Despy is on the right */
			} else if (dx > -EF_RADIUS && dx <= 0) {
				if (ENE_VX(*data) > 0) {
					data->MotionType = 3;
				}
			}
		}

	}
	
	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;
	ENE_PALET(*data) = 10;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[EF_TYPE], number), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}


/*
 *		BEE
 */

#define	BEE_VXMAX	72
#define	BEE_VX		72
#define	BEE_VY		72
#define	BEE_VY2		96
#define	BEE_VVY		3
#define	BEE_VVX		2
#define	BEE_WAIT	240
#define	BEE_RADIUS	127 * DIV_X
#define	BEE_STOPCNT	60

void	EneMove_bee(ENEMY *data, short scx, short scy)
{
	int	number, mcnt, bombcnt = ENE_BOMBCNT(*data);
	short	dx = (ENE_X(*data) - dspdata.x) & 0xffff;
	int	mtype = data->MotionType;

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	ENE_VY(*data) = 0;
	ENE_VX(*data) = 0;
	if (mtype >= 0) {
		mcnt = data->MotionCnt;
		number = MOTION(mf_buff[BEE_TYPE], mtype, mcnt);
		switch(mtype) {
		  case 0:
			if (mcnt < BEE_STOPCNT) {
				ENE_VY(*data) = - BEE_VY;
				ENE_VX(*data) = - BEE_VX;
			}
			if (bombcnt < 0){
				ENE_VY(*data) = - BEE_VY2;
				ENE_VX(*data) = 0;
				if (ENE_Y(*data) <= ENE_YMIN) {
					data->MotionType = 4;
					data->MotionCnt = 0;
				}
			}
			break;
		  case 1:
			if (mcnt < BEE_STOPCNT) {
				ENE_VY(*data) =   BEE_VY;
				ENE_VX(*data) = - BEE_VX;
			}
			break;
		  case 2:
			if (mcnt < BEE_STOPCNT) {
				ENE_VY(*data) = - BEE_VY;
				ENE_VX(*data) =   BEE_VX;
			}
			if (bombcnt < 0){
				ENE_VY(*data) = - BEE_VY2;
				ENE_VX(*data) = 0;
				if (ENE_Y(*data) <= ENE_YMIN) {
					data->MotionType = 5;
					data->MotionCnt = 0;
				}
			}
			break;
		  case 3:
			if (mcnt < BEE_STOPCNT) {
				ENE_VY(*data) =   BEE_VY;
				ENE_VX(*data) =   BEE_VX;
			}
			break;
		  case 4:
			switch(mcnt) {
				case 5:
					makebomb2(ENE_X(*data), ENE_Y(*data), 3, 45);
					break;
				case 10:
					makebomb2(ENE_X(*data), ENE_Y(*data), 3, 51);
					break;
				case 15:
					makebomb2(ENE_X(*data), ENE_Y(*data), 3, 48);
					break;
			}
			break;
		  case 5:
			switch(mcnt) {
				case 5:
					makebomb2(ENE_X(*data), ENE_Y(*data), 3, 13);
					break;
				case 10:
					makebomb2(ENE_X(*data), ENE_Y(*data), 3, 16);
					break;
				case 15:
					makebomb2(ENE_X(*data), ENE_Y(*data), 3, 19);
					break;
			}
			break;
		  case 6:
			if (mcnt < BEE_STOPCNT) {
				ENE_VX(*data) =  - BEE_VX;
			}
			break;
		  case 7:
			if (mcnt < BEE_STOPCNT) {
				ENE_VX(*data) =    BEE_VX;
			}
			break;
		}
		data->MotionCnt++;
		if (MOTION(mf_buff[BEE_TYPE], mtype, data->MotionCnt) < 0) {
			data->MotionCnt = 0;
			switch(mtype) {
			  case 0:
				if (bombcnt < 0) {
					data->MotionType = 4;
				} else {
					data->MotionType = 6;
				}
				break;
			  case 2:
				if (bombcnt < 0) {
					data->MotionType = 5;
				} else {
					data->MotionType = 7;
				}
				break;

			  case 1:
				data->MotionType = 6;
				break;
			  case 5:
				data->MotionType = 6;
				data->bombcnt = BEE_WAIT;
				break;
			  case 3:
				data->MotionType = 7;
				break;
			  case 4:
				data->MotionType = 7;
				data->bombcnt = BEE_WAIT;
				break;
			  case 6:
				if (data->LastMotion == 0) {
					data->MotionType = 1;
				} else {
					data->MotionType = 0;
				}
				break;
			  case 7:
				if (data->LastMotion == 2) {
					data->MotionType = 3;
				} else {
					data->MotionType = 2;
				}
				break;
			  default:
				data->MotionType = 0;
			}
			data->LastMotion = mtype;
		}
		if (ENE_BOMBCNT(*data) > 0) {
			ENE_BOMBCNT(*data) --;
		} else {
								/* Despy is on the left */
			if (dx < BEE_RADIUS && dx >= 0) {
				if (ENE_VX(*data) < 0) {
					ENE_BOMBCNT(*data) = -1;
					data->MotionType = 0;
				}
								/* Despy is on the right */
			} else if (dx > -BEE_RADIUS && dx <= 0) {
				if (ENE_VX(*data) > 0) {
					ENE_BOMBCNT(*data) = -1;
					data->MotionType = 2;
				}
			}
		}

	} else {
		number = MOTION(mf_buff[BEE_TYPE], 0, 0);
		if (ENE_VX(*data) > 0) {
			data->MotionType = 2;
		} else {
			data->MotionType = 0;
		}
	}

	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;
	ENE_PALET(*data) = 11;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[BEE_TYPE], number), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}
