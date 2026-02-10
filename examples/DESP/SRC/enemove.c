#include        <stdlib.h>
#include        <stddef.h>
#include		"sps.h"
#include		"dsp.h"
#include		"enemy.h"
#include		"display.h"
#include		"sound.h"
#include		"item.h"

#define	WORD	short
#define	UWORD	unsigned short

extern DSP   dspdata;
extern ENEMY ene_dat[];
extern short *pat_buff[];
extern short *mf_buff[];
extern int count_;
extern int damagecnt;
extern short sintbl[];
extern short rank;			/* Difficulty */

/*
 *		Enemy display subroutine
 */

void
EnemyDisp(ENEMY *data, int chrsize, unsigned char *spradr, short scx, short scy)
{
	short	x;

	x = SCX4x4(ENE_X(*data), scx) & 0xffff;

	if (x < (short)-32 || x > (short)320) {
		if (ENE_SP(*data)) {
			SpsFree(ENE_SP(*data));
			ENE_SP(*data) = NULL;
		}
		if (ENE_SHADOW(*data)) {
			SpsFree(ENE_SHADOW(*data));
			ENE_SHADOW(*data) = NULL;
		}
	} else {
		if (!ENE_SP(*data)) {
			ENE_SP(*data) = SpsAlloc(chrsize);
			ENE_SP_COLOR(*data) = ENE_PALET(*data);
		}
		if (!ENE_SHADOW(*data)) {
			ENE_SHADOW(*data) = SpsAlloc(CHR1x1);
			ENE_SHADOW_COLOR(*data) = 2;
		}

		if (ENE_SP(*data)) {
			ENE_SP_X(*data) = x;
			ENE_SP_Y(*data) = SCY4x4(ENE_Y(*data), scy);
			ENE_SP_CHR(*data) = spradr;
		}

		if (!ENE_SHADOW(*data)) return;

		ENE_SHADOW_X(*data) = x;
		ENE_SHADOW_Y(*data) = SCY1x1(SHADOWY, scy);
		if (count_ & 1) {
			ENE_SHADOW_NUM(*data) = SPNULL;
		} else {
			ENE_SHADOW_NUM(*data) = SHADOWNUM;
		}

	}
}


/*
 *			HOBER
 */

#define		HOBER_XSPEED	(16 + rank / 2)
#define		HOBER_WAIT	(140 - rank)		//Storing period
#define		HOBER_WAIT2	(100 - rank)		//Storage period 2
#define		HOBER_VVX	(1 + rank / 16)
#define		HOBER_VXMAX	24
#define		HOBER_MODE	24
#define		HOBER_LY	512

void	EneMove_hober(ENEMY *data, short scx, short scy)
{
	short dx = (WORD)(((UWORD)ENE_X(*data) - (UWORD)dspdata.x) & 0xffff);
	short px;

	//Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	if (ENE_EY(*data) < ENE_YMAX - 320) {
		ENE_VY(*data) = 64;
	} else {
		ENE_VY(*data) = 0;
	}

	if (ENE_BOMBCNT(*data)) {
		if (ENE_BOMBCNT(*data) > HOBER_WAIT2 + 14 &&
			ENE_BOMBCNT(*data) < HOBER_WAIT2 + 32) {
			px = ENE_X(*data) + ENE_VX(*data) * 8;
			if (count_ & 1)
				Make1Light(px, ENE_Y(*data)-HOBER_LY, -1, -1);
			else
				Make1Light(px, ENE_Y(*data)-HOBER_LY, 1, -1);
		}
		if (ENE_BOMBCNT(*data)-- == HOBER_WAIT2) {
			px = ENE_X(*data) + ENE_VX(*data) * 3;
			makebomb(px, ENE_Y(*data) -256, 2);
		}
	} else {
		if (rank >= HOBER_MODE) {				/* Tracking mode */
			if (dx < 0) {
				if (ENE_VX(*data) > -HOBER_VXMAX)
					ENE_VX(*data) -= HOBER_VVX;
			} else {
				if (ENE_VX(*data) < HOBER_VXMAX)
					ENE_VX(*data) += HOBER_VVX;
			}
		} else {						/* Straight mode */
			if (ENE_VX(*data) < 0) {
				ENE_VX(*data) = -HOBER_XSPEED;
			} else {
				ENE_VX(*data) = HOBER_XSPEED;
			}
		}
		if (dx > -1024 && dx < 1024 && dspdata.y >= 3800) {
			ENE_BOMBCNT(*data) = HOBER_WAIT;
		}
	}


	ENE_X(*data) += ENE_VX(*data);
	ENE_EY(*data) += ENE_VY(*data);
	ENE_Y(*data) = ENE_EY(*data) + sintbl[count_ & 127] / 4;
	ENE_PALET(*data) = 6;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[HOBER_TYPE], count_ & 31), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}

/*	Iron Maiden	*/

#define		IRON_XSPEED	64
#define		IRON_RADIUS	(127 * DIV_X)
#define		IRON_WAIT	(84 - (rank / 2 & 0xfc))	// Storing period
#define		IRON_LR		(rank)
#define		IRON_MODE	24

void	EneMove_iron(ENEMY *data, short scx, short scy)
{
	short	dx;

	dx = (ENE_X(*data) - dspdata.x) & 0xffff;

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);
	if (rank < IRON_MODE) {				/* Normal mode */
		ENE_Y(*data) = ENE_EY(*data) + sintbl[count_ & 127] / 4;
	} else {						/* Madness mode */
		ENE_Y(*data) = ENE_EY(*data)
				+ sintbl[data->count_ & 127] * IRON_LR / 32;
		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -IRON_XSPEED;
		} else {
			ENE_VX(*data) = IRON_XSPEED;
		}
		ENE_X(*data) += ENE_VX(*data);
	}
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;
	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	ENE_PALET(*data) = 4;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[IRON_TYPE], (count_ & 0x7fff) % 20), scx, scy);
	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar


	if (dx < IRON_RADIUS && dx >= -IRON_RADIUS) {
		if (ENE_BOMBCNT(*data) > 0) {
			ENE_BOMBCNT(*data) --;
		} else {
			makebomb2(ENE_X(*data), ENE_Y(*data), 3,(data->count_) & 63);
			ENE_BOMBCNT(*data) = IRON_WAIT;
		}
	} else {
		ENE_BOMBCNT(*data) = IRON_WAIT;
	}

}

/*
 *		HEX
 */

#define		HEX_XSPEED	(40 + rank / 2)
#define		HEX_RADIUS	(127 * DIV_X)
#define		HEX_WAIT	256			// Accumulation period
#define		HEX_LX		512
#define		HEX_LY		((rand() % 32767) / 64  - 256)

void	EneMove_hex(ENEMY *data, short scx, short scy)
{
	short	sc = (data->count_ / 2) & 255;
	int	mtype, number;
	short	dx;

	// Remove the radar dot.

	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	mtype = data->MotionType;

	if (mtype >= 0) {
		number = MOTION(mf_buff[HEX_TYPE], mtype, data->MotionCnt);
		ENE_VX(*data) = ENE_VY(*data) = 0;
		switch (mtype) {
		  case 1:
			if (number == 40) {
				makebomb2(ENE_X(*data), ENE_Y(*data), 4, -1);
			}
			if (number < 24) {
				if (count_ & 1)
				Make1Light(ENE_X(*data)-HEX_LX, ENE_Y(*data)+HEX_LY, -1, -1);
				else
				Make1Light(ENE_X(*data)-HEX_LX, ENE_Y(*data)+HEX_LY, -1, 1);
			}
			break;
		  case 2:
			if (number == 88) {
				makebomb2(ENE_X(*data), ENE_Y(*data), 4, 1);
			}
			if (number < 72) {
				if (count_ & 1)
				Make1Light(ENE_X(*data)+HEX_LX, ENE_Y(*data)+HEX_LY, 1, -1);
				else
				Make1Light(ENE_X(*data)+HEX_LX, ENE_Y(*data)+HEX_LY, 1, 1);
			}
			break;
		}
		data->MotionCnt++;
		if (MOTION(mf_buff[HEX_TYPE], mtype, data->MotionCnt) < 0) {
			data->MotionCnt = 0;
			data->MotionType = -1;
			ENE_BOMBCNT(*data) = HEX_WAIT;
			if (mtype == 1) {
				ENE_VX(*data) = HEX_XSPEED;
			} else {
				ENE_VX(*data) = -HEX_XSPEED;
			}
		}
	} else {

		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -HEX_XSPEED;
		} else {
			ENE_VX(*data) = HEX_XSPEED;
		}

		if (sc < 64) {
			ENE_VY(*data) = sintbl[sc + 32] / 16;
		} else if (128 <= sc && sc < 192) {
			ENE_VY(*data) = sintbl[(sc - 32) & 127] / 16;
		} else {
			ENE_VY(*data) = 0;
		}
		number = (count_ & 15);


		if (ENE_BOMBCNT(*data)) {
			ENE_BOMBCNT(*data)--;
		} else {
			dx = (ENE_X(*data) - dspdata.x) & 0xffff;
								/* Despy is on the left */
			if (dx < HEX_RADIUS && dx >= 0) {
				data->MotionType = 1;
								/* Despy is on the right */
			} else if (dx > -HEX_RADIUS && dx < 0) {
				data->MotionType = 2;
			}
		}
	}

	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);

	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;

	ENE_PALET(*data) = 7;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[HEX_TYPE], number), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}

/*
 *		Blue grasshopper (still unfinished)
 */

#define		AO_VXMAX	96
#define		AO_VXMAX2	64
#define		AO_VYMAX	16
#define		AO_XSPEED	2
#define		AO_YSPEED	1
#define		AO_DCNT		7	/* 0 <= VX/DDIV + CNT <= 14 */
#define		AO_DDIV		4
#define		AO_YDIV		8
#define		AO_YCNT		2
#define		AO_UD_L		30	/* Top and bottom pattern numbers */
#define		AO_UD_R		35
#define		AO_WAIT		120
#define		AO_REVWAIT	300
#define		AO_RADIUS	(127 * DIV_X)

void	EneMove_ao(ENEMY *data, short scx, short scy)
{

	int	number;
	short	dx;
	int	mtype = data->MotionType;
	int	mcnt  = data->MotionCnt;

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	if (mtype >= 0) {
		number = MOTION(mf_buff[AO_TYPE], mtype, data->MotionCnt);
		switch(mtype) {
		  case 0:
		  case 2:
			if (ENE_VX(*data) < AO_VXMAX) {
				ENE_VX(*data) += AO_XSPEED;
			}
			break;
		  case 1:
		  case 3:
			if (ENE_VX(*data) > -AO_VXMAX) {
				ENE_VX(*data) -= AO_XSPEED;
			}
			break;
		  case 4:
			ENE_VY(*data) = -AO_VYMAX;
			ENE_VX(*data) = -AO_VXMAX2;
			if (mcnt == 10 || mcnt == 14 || mcnt == 18 || mcnt == 22) {
				makebomb2(ENE_X(*data), ENE_Y(*data) + 256, 0, -1);
			}
			break;
		  case 5:
			ENE_VY(*data) = AO_VYMAX;
			ENE_VX(*data) = -AO_VXMAX2;
			if (mcnt == 10 || mcnt == 14 || mcnt == 18 || mcnt == 22) {
				makebomb2(ENE_X(*data), ENE_Y(*data) + 256, 0, -1);
			}
			break;
		  case 6:
			ENE_VY(*data) = -AO_VYMAX;
			ENE_VX(*data) = AO_VXMAX2;
			if (mcnt == 10 || mcnt == 14 || mcnt == 18 || mcnt == 22) {
				makebomb2(ENE_X(*data), ENE_Y(*data) + 256, 0, 1);
			}
			break;
		  case 7:
			ENE_VY(*data) = AO_VYMAX;
			ENE_VX(*data) = AO_VXMAX2;
			if (mcnt == 10 || mcnt == 14 || mcnt == 18 || mcnt == 22) {
				makebomb2(ENE_X(*data), ENE_Y(*data) + 256, 0, 1);
			}
			break;
		}

		data->MotionCnt++;
		if (MOTION(mf_buff[AO_TYPE], mtype, data->MotionCnt) < 0) {
			data->MotionCnt = 0;
			data->MotionType = -1;
			switch(mtype) {
				case 0:
				case 2:
					ENE_VX(*data) = 1;
					data->dummy1 = AO_REVWAIT;
					break;
				case 1:
				case 3:
					ENE_VX(*data) = -1;
					data->dummy1 = AO_REVWAIT;
					break;
				case 4:
					data->MotionType = 0;
					break;
				case 5:
					data->MotionType = 2;
					break;
				case 6:
					data->MotionType = 1;
					break;
				case 7:
					data->MotionType = 3;
					break;
			}
		}

	} else {

		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -AO_VXMAX;
		} else {
			ENE_VX(*data) = AO_VXMAX;
		}
		if (ENE_BOMBCNT(*data)) {
			ENE_BOMBCNT(*data) --;
		} else {
			dx = (ENE_X(*data) - dspdata.x) & 0xffff;
								/* Despy is on the left */
			if (dx < AO_RADIUS && dx >= 0) {
				if (ENE_VX(*data) < 0) {
					if (ENE_Y(*data) > 3600) {
						data->MotionType = 4;
					} else {
						data->MotionType = 5;
					}
					ENE_BOMBCNT(*data) = AO_WAIT;
				}
								/* Despy is on the right */
			} else if (dx > -AO_RADIUS && dx <= 0) {
				if (ENE_VX(*data) > 0) {
					if (ENE_Y(*data) > 3600) {
						data->MotionType = 6;
					} else {
						data->MotionType = 7;
					}
					ENE_BOMBCNT(*data) = AO_WAIT;
				}
			}
		}

		if (data->dummy1) {
			data->dummy1--;
		} else if (data->MotionType == 0) {
			dx = (ENE_X(*data) - dspdata.x) & 0xffff;
			if (dx < 0) {
				if (ENE_VX(*data) > 0) {
					if (ENE_VY(*data) < 0) {
						data->MotionType = 1;
					} else {
						data->MotionType = 3;
					}
				}
			} else {
				if (ENE_VX(*data) < 0) {
					if (ENE_VY(*data) < 0) {
						data->MotionType = 0;
					} else {
						data->MotionType = 2;
					}
				}
			}
		}

		if (ENE_Y(*data) < dspdata.y) ENE_VY(*data) += AO_YSPEED;
		else				 ENE_VY(*data) -= AO_YSPEED;

		if (ENE_VY(*data) >  AO_VYMAX)   ENE_VY(*data) =  AO_VYMAX;
		if (ENE_VY(*data) < -AO_VYMAX)   ENE_VY(*data) = -AO_VYMAX;
		if (ENE_VX(*data) < 0) {
			number = (AO_UD_L + AO_YCNT + ENE_VY(*data) / AO_YDIV);
		} else {
			number = (AO_UD_R + AO_YCNT + ENE_VY(*data) / AO_YDIV);
		}
	}

	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;
	ENE_PALET(*data) = 3;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[AO_TYPE], number), scx, scy);

	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}


/*
 *		Bird
 */

#define		BIRD_XSPEED	(64 + rank)
#define		BIRD_YSPEED	64
#define		BIRD_VXS	2
#define		BIRD_VYS	5
#define		BIRD_RADIUS	(127 * DIV_X)

void	EneMove_bird(ENEMY *data, short scx, short scy)
{
	int	mtype, number;
	short	dx;

	// Remove the radar dot.
	radar_reset(ENE_X(*data), ENE_Y(*data), 15);

	mtype = data->MotionType;
	if (mtype >= 0) {
		number = MOTION(mf_buff[BIRD_TYPE], mtype, data->MotionCnt);
		switch (mtype) {
		  case 0:
		  case 2:
			ENE_VY(*data) -= BIRD_VYS;
			break;
		  case 1:
		  case 3:
			ENE_VY(*data) += BIRD_VYS;
			break;
		  case 4:
			ENE_VX(*data) += BIRD_VXS;
			if (ENE_VY(*data) > 0) {
				ENE_VY(*data) -= BIRD_VYS;
			} else {
				ENE_VY(*data) += BIRD_VYS;
			}
			break;
		  case 5:
			ENE_VX(*data) -= BIRD_VXS;
			if (ENE_VY(*data) > 0) {
				ENE_VY(*data) -= BIRD_VYS;
			} else {
				ENE_VY(*data) += BIRD_VYS;
			}
			break;
		}
		if (ENE_VY(*data) > BIRD_YSPEED) ENE_VY(*data) = BIRD_YSPEED;
		if (ENE_VY(*data) <-BIRD_YSPEED) ENE_VY(*data) =-BIRD_YSPEED;

		data->MotionCnt++;
		if (MOTION(mf_buff[BIRD_TYPE], mtype, data->MotionCnt) < 0) {
			data->MotionCnt = 0;
			switch(mtype) {
			  case 0:
			  case 1:
				data->MotionType = 4;
				data->dummy1 = 180;
				break;
			  case 2:
			  case 3:
				data->MotionType = 5;
				data->dummy1 = 180;
				break;
			  case 4:
				data->MotionType = -1;
				ENE_VX(*data) = BIRD_XSPEED;
				break;
			  case 5:
				data->MotionType = -1;
				ENE_VX(*data) = -BIRD_XSPEED;
				break;
			  default:
				data->MotionType = -1;
			}
		}

	} else {

		ENE_VY(*data) = 0;
		if (ENE_VX(*data) < 0) {
			ENE_VX(*data) = -BIRD_XSPEED;
			number = MOTION(mf_buff[BIRD_TYPE], 0, 0);
		} else {
			ENE_VX(*data) = BIRD_XSPEED;
			number = MOTION(mf_buff[BIRD_TYPE], 2, 0);
		}

		if (data->dummy1) {
			data->dummy1--;
		} else {
			dx = (ENE_X(*data) - dspdata.x) & 0xffff;
								/* Despy is on the left */
			if (dx < BIRD_RADIUS && dx >= 0) {
				if (ENE_VX(*data) > 0) {
					data->MotionType = 5;
				} else {
					if (ENE_Y(*data) < dspdata.y) {
						data->MotionType = 1;
					} else {
						data->MotionType = 0;
					}
				}
								/* Despy is on the right */
			} else if (dx > -BIRD_RADIUS && dx < 0) {
				if (ENE_VX(*data) < 0) {
					data->MotionType = 4;
				} else {
					if (ENE_Y(*data) < dspdata.y) {
						data->MotionType = 3;
					} else {
						data->MotionType = 2;
					}
				}
			}
		}
	}


	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	if (ENE_Y(*data) < ENE_YMIN) ENE_Y(*data) = ENE_YMIN;
	if (ENE_Y(*data) > ENE_YMAX) ENE_Y(*data) = ENE_YMAX;
	ENE_PALET(*data) = 5;
	EnemyDisp(data, CHR4x4, SPRADR(pat_buff[BIRD_TYPE], number), scx, scy);
	radar_set(ENE_X(*data), ENE_Y(*data), 15);		// Display on radar

}


/*	It's an explosion	*/

void
EneMove_crash(ENEMY *data, short scx, short scy) {
	short	number;

	radar_reset(ENE_X(*data), ENE_Y(*data), 15);	// Remove the radar dot.

	data->atariflg = 0;
	ENE_X(*data) += ENE_VX(*data);
	ENE_Y(*data) += ENE_VY(*data);
	ENE_PALET(*data) = 2;

	if (data->MotionType >= 0) {
		number = MOTION(mf_buff[CRASH_TYPE], data->MotionType, data->MotionCnt);
		data->MotionCnt++;
		if (MOTION(mf_buff[CRASH_TYPE], data->MotionType, data->MotionCnt) < 0) {
			data->MotionType = -1;
			data->MotionCnt = 0;
		}

		EnemyDisp(data, CHR4x4, SPRADR(pat_buff[CRASH_TYPE], number), scx, scy);

	} else {

		if (ENE_SP(*data)) {
			SpsFree(ENE_SP(*data));
			ENE_SP(*data) = NULL;
		}
		if (ENE_SHADOW(*data)) {
			SpsFree(ENE_SHADOW(*data));
			ENE_SHADOW(*data) = NULL;
		}
		ENE_TYPE(*data) = -1;
	}

}

/*
 *			tub
 */

void
EneMove_tarai(ENEMY *data, short scx, short scy) {

	radar_reset(ENE_X(*data), ENE_Y(*data), 15);	// Remove the radar dot.

	ENE_Y(*data) += ENE_VY(*data);

	ENE_PALET(*data) = 3;
	EnemyDisp(data, CHR3x3, SPRADR(pat_buff[TARAI_TYPE], 0), scx, scy);

	if (ENE_Y(*data) > 2400) {
		if (ENE_STATE(*data)) {
			if (count_ >= 0) {			// Level not cleared
				sd_on(SD_WARN1);
				warn_display();
			}
			ENE_STATE(*data) = 0;
		} else {
			short	x;
			x = SCX4x4(ENE_X(*data), scx) & 0xffff;
			if (x < (short)-32)		tarai_dir_display(1);
			else if (x > (short)320)	tarai_dir_display(2);
			else				tarai_dir_display(0);
		}
	}

	if (ENE_Y(*data) > YMAX) {				// The bucket fell.
		tarai_dir_display(0);
		if (count_ >= 0) {				// Level not cleared
			damagecnt = 64;
		}
		ENE_ENERGY(*data) = 0;
	}

	radar_set(ENE_X(*data), ENE_Y(*data), 11);	// Display on radar

}


/*
 *		Appearance motion
 */

#define	APPEARCNT	64
#define	APPEARWIDTH	128
#define	APPEARMUL	(APPEARWIDTH / APPEARCNT)
#define	APPEARNUM1	3
#define	APPEARNUM2	4
#define	APPEARNUM3	5
#define	APPEARNUM4	6

void	ENE_APPEAR(ENEMY *data, short scx, short scy)
{
	short	x, y, cnt, d;

	cnt = APPEARCNT - data->MotionCnt;
	d = APPEARMUL * cnt;
	x = SCX1x1(ENE_X(*data), scx);
	y = SCX1x1(ENE_Y(*data), scy);

	data->atariflg = 0;

	if (x < (short)-256 || x > (short)520) {
		if (ENE_SP(*data)) {
			SpsFree(ENE_SP(*data));
			ENE_SP(*data) = NULL;
		}
		if (ENE_SHADOW(*data)) {
			SpsFree(ENE_SHADOW(*data));
			ENE_SHADOW(*data) = NULL;
		}
	} else {
		if (!ENE_SP(*data)) {
			ENE_SP(*data) = SpsAlloc(CHR1x1);
			ENE_SP_COLOR(*data) = 2;
		}
		if (!ENE_SHADOW(*data)) {
			ENE_SHADOW(*data) = SpsAlloc(CHR1x1);
			ENE_SHADOW_COLOR(*data) = 2;
		}
		if (!ENE_SP(*data) || !ENE_SHADOW(*data)) return;
		if (cnt & 1) {
			ENE_SHADOW_X(*data) = x + d;
			ENE_SHADOW_Y(*data) = y + d;
			ENE_SHADOW_NUM(*data) = APPEARNUM4;
			ENE_SP_X(*data) = x - d;
			ENE_SP_Y(*data) = y - d;
			ENE_SP_NUM(*data) = APPEARNUM1;
		} else {
			ENE_SHADOW_X(*data) = x - d;
			ENE_SHADOW_Y(*data) = y + d;
			ENE_SHADOW_NUM(*data) = APPEARNUM3;
			ENE_SP_X(*data) = x + d;
			ENE_SP_Y(*data) = y - d;
			ENE_SP_NUM(*data) = APPEARNUM2;
		}
	}

	data->MotionCnt++;

	if (cnt == 0) {
		if (ENE_SP(*data) != NULL) {
			SpsFree(ENE_SP(*data));
			SpsFree(ENE_SHADOW(*data));
			ENE_SP(*data) = NULL;
			ENE_SHADOW(*data) = NULL;
		}
		data->state =  1;
		data->MotionType = -1;
		data->MotionCnt = 0;
		data->atariflg = 1;
		data->count_ = 0;
	}

}
