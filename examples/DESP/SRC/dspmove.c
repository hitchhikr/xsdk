/*
 *		dspmove.c
 *
 *						Programmed by Momo
 */


#include	<stdio.h>
#include	<iocslib.h>

#include	"sps.h"
#include	"dsp.h"
#include	"dspmove.h"
#include	"display.h"
#include	"sound.h"
#include	"item.h"
#include	"graph.h"

#define		DSP_CHRNUM	0
#define		BUR_CHRNUM	1
#define		BUR_PATTERN0	8
#define		BUR_PATTERN1	9
#define		BUR_PALET	2

#define		SHOT_PATTERN	15
#define		SHOT_PALET	2
#define		SHOT_SPEED	256		// Shot Speed
#define		SHOT_SPEEDF	224		// Shot speed immediately after firing
#define		SHOT_INIT_X	768		// Shot firing position
#define		SHOT_ENDCNT	24		// The number of shots that will disappear
#define		SHOT_Y		(dspdata->y - 4 * DIV_Y)

#define		SGN(x, y)	((x) > 0 ? (y) : -(y))
#define		ABS(x)	((x) > 0 ? (x) : -(x))

#define		DIV_MOTION0	1
#define		DIRMAX		9	// Maximum direction (19 patterns from -9 to 9)
#define		DIV_MOTION1	2
#define		MOTION1MAX	18	// Maximum posture value (0-9)
#define		MOTION1MIN	0

#define		PAT_LEFT	0	// Pattern Number
#define		PAT_RIGHT	10
#define		PAT_LK2RG	20
#define		PAT_RK2LG	39

#define		VMAX_X		94
#define		VMAX_Y		96

static	int	smokecount[16] = {
	32, 64, 48, 80,
	40, 72, 56, 88,
	36, 78, 52, 84,
	44, 76, 60, 92
};

#define		SCROLLX_DIF	(24 * DIV_X)	// Width when the ship moves left or right(??)
#define		SCROLLX_VELO	DIV_X		// The speed at that time

#define		DSP_YMIN	1344
#define		DSP_YMAX	8576

static	int	smoke_x[8] = {
	0, 500, 0, -500, 250, 380, -200, -400};

extern	int	whitecnt;			//	Counter with illuminated screen
extern	short	screen_x0, screen_y0;		// Virtual coordinates of the top left corner of the real screen
extern	short	*pat_buff[];
extern	short	*mf_buff[];	//	mf address
extern	unsigned short	spalet[256];
extern	unsigned short	wingpalet[];

extern	short	joymem[];			//	LOG
extern	short	gamemode;
extern	short	joymempoint;
extern	volatile
int	graphicflag;			/* Between RASTER2 and VDISP, it is 1 */

extern	int	count_;
extern	int	invincible;			//	Invincible Flag

static	short	burnercnt = 0;			// For vernier
static	short	scroll_x = 0;			// For scrolling

/*
 *	Joystick & Keyboard Reader Can be used directly instead of JOYGET().
 *
 *	7 . . . . . 0 (bit)
 *	.xf1 xf2 . 6 4 2 8
 *	    space
 */

short	JOY(int mode)
{
	short	i, b1, b2, b3;

	if (mode != PLAY_DEMO) {
		i  = JOYGET(0);
		b1 = BITSNS(8);
		b2 = BITSNS(9);
		b3 = BITSNS(10);

		if (b1		 &  16)	i &= 254;
		if (b1		 & 128)	i &= 251;
		if (b2		 &   2)	i &= 247;
		if (b2		 &  16)	i &= 253;

		if (b3		 &  64)	i &= 223;
		if (b3		 &  32)	i &= 191;
		if (BITSNS(6) &  32)	i &= 191;

		if (mode == PLAY_RECORD) {
			joymem[joymempoint++] = i;
		}

	} else {
		i = joymem[joymempoint++];
	}

	if (joymempoint > 32766) {
		joymempoint = 32766;
	}

	return	i;
}

/*
 *			Bullet firing
 */

static void	makeshot(short x, short y, short vx, short vy,
						  short type, SHOT *shotdata)
{
	int	i = 0;

	for(; shotdata->condition != 0 && i < SHOT_NUM; i++, shotdata++);

	if (i < SHOT_NUM) {
		shotdata->x = x;
		shotdata->y = y;

		shotdata->type = type;
		shotdata->condition = 1;
		shotdata->vx = vx;
		shotdata->vy = vy;
		shotdata->sp = SpsAlloc(CHR1x1);
		if (!(shotdata->sp)) {
			shotdata->condition = 0;
			return;
		}
		sd_on(0);
	}
}

static	void	MakeSmoke(DSP *dspdata)
{
	if (!(count_ & 7) && dspdata->y > 8500 - ABS(dspdata->vx) * 4) {
		if (dspdata->y > 8400) {
			dspdata->vy = -dVY * 4;
			MakeItem(dspdata->x + smoke_x[count_ / 16 & 7] - dspdata->vx * 4,
				9200 - ABS(dspdata->vx) * 2,
				smoke_x[count_ / 16 & 7] / 64, -12, 0, 0xc);
		} else if (smokecount[count_ / 8 & 15] < ABS(dspdata->vx))
			MakeItem(dspdata->x + smoke_x[count_ / 16 & 7] - dspdata->vx * 16,
				9200 - ABS(dspdata->vx) * 2,
				smoke_x[count_ / 16 & 7] / 64, -12, 0, 0xc);
	}
}


/*
 *		Desperado move!
 */

int	dspmove(DSP *dspdata, SHOT *shotdata, int invflg)
{

	short		x, y, dx, dy;
	short		vx, vy, i, j;
	short		number;			//  Pattern Number
	short		motion1;
	short		bx = 0, by = 0;		// Vernia coordinates
	short		burnerpat;			// Vernier pattern
	short		vx0, vy0;			// For radar
	static	short	death;

	SpsFree(dspdata->sp);	dspdata->sp = SpsAlloc(CHR4x4);

	MakeSmoke(dspdata);

	if (invflg == 0 && invincible == 0) {
		dspdata->atariflg = 1;
	} else {
		dspdata->atariflg = 0;
	}

	x  = dspdata->x;
	y  = dspdata->y;
	vx = dspdata->vx;
	vy = dspdata->vy;

	motion1 = dspdata->motion1;
	dx = x; dy = y;

							// Movement Routines
	i = JOY(gamemode);

	if (!(i & UP))		vy -= dVY;
	else	if (!(i & DOWN))	vy += dVY;
	else	if (vy > 0)		vy -= dVY;
	else	if (vy < 0)		vy += dVY;

	if (dspdata->MotionType < 0) {
		j = 1;
	} else {
		j = 0;
	}

	if (!(i & RIGHT)) {
		vx += dVX;
		if (motion1 < MOTION1MAX && j) motion1++;

		if (motion1 == MOTION1MAX && dspdata->MotionType < 0) {
			burnercnt += 2;
		}

	} else if (!(i & LEFT)) {
		vx -= dVX;
		if (motion1 > MOTION1MIN && j) motion1--;

		if (motion1 == MOTION1MIN && dspdata->MotionType < 0) {
			burnercnt += 2;
		}
	} else {
/*		if (vx < - VMAX_X) vx += dVX;
		if (vx >   VMAX_X) vx -= dVX;
*/
	}

	if (vy < - VMAX_Y) vy = - VMAX_Y;
	if (vy >   VMAX_Y) vy =   VMAX_Y;
	if (vx < - VMAX_X) vx = - VMAX_X;
	if (vx >   VMAX_X) vx =   VMAX_X;


							// When you're dead

	if (dspdata->energy <= 0) {
		if (death == 0) bgm_fade(32);
		death++;

		dspdata->vx = vx;
		dspdata->x = (x += vx);
		if (y < DSP_YMAX) dspdata->y = (y += 48);

									// Think of patterns
		if (death > 60) {
			dspdata->sp->chr = SPRADR(crash_pat_buff, (death - 60) / 4);
			dspdata->sp->palet = 2;
		} else {
			dspdata->sp->chr = SPRADR(dsp_pat_buff, death % 5 + 88);
			if (death % 3 == 0) sd_on(20);
		}
		if (death == 60) {
			sd_on(21);
			MakeBricks(x, y, 0, 0, 0, 1);
			MakeBricks(x, y, 0, 0, 0, 0xc);
			whitecnt = 64;
		}

		vx0 = screen_x0;
		vy0 = screen_y0;
		screen_x0 = x - 128 * DIV_X + scroll_x;
		screen_y0 = y / 3;
		if (vx > 0) {
			draw_to_graphic((screen_x0 / DIV_X / 2) + 320, (screen_x0 / DIV_X / 2) + 320);
		} else {
			draw_to_graphic((screen_x0 / DIV_X / 2) - 64, (screen_x0 / DIV_X / 2) - 64);
		}
		radar_scope(screen_x0, screen_y0, vx0, vy0);	// radar

		radar_set(dspdata->x, dspdata->y, 8);

		dspdata->sp->x = SCX4x4(x, screen_x0);
		dspdata->sp->y = SCY4x4(y, screen_y0);
		dspdata->shadow_sp->num = 0;

/*		dspdata->atariflg = 0;		*/
		if (death > 90) return 1;
		else return 0;
	} else {
		death = 0;
	}


							// Vernier coordinate setting
	if (motion1 > MOTION1MAX / 2) {
		bx = x + vx - 21 * DIV_X;
		by = y +  8 * DIV_Y;
	} else {
		bx = x + vx + 21 * DIV_X;
		by = y +  8 * DIV_Y;
	}

	if (burnercnt) burnercnt--;
	if (burnercnt > 12) burnercnt =  12;
	if (burnercnt < 12)	burnerpat = BUR_PATTERN0;
	else			burnerpat = BUR_PATTERN1;


	x = x + vx;
	y = y + vy;

	if (y > DSP_YMAX)		y = DSP_YMAX;
	if (y < DSP_YMIN)		y = DSP_YMIN;

							//  Is the trigger pressed?

	if (dspdata->MotionType < 0) {
		if (!(i & LEFT_TRIGGER) && invflg == 0) {
			if (dspdata->rbutton == NOT_PRESSED & j) {
				makeshot(x + SGN(dspdata->dir, SHOT_INIT_X) - vx, SHOT_Y,
					vx + SGN(dspdata->dir, SHOT_SPEED), 0, 0, shotdata);
			}
			dspdata->rbutton = (dspdata->rbutton + 1) & 15;
		} else {
			dspdata->rbutton = NOT_PRESSED;
		}

		if (!(i & RIGHT_TRIGGER)) {
			if (dspdata->lbutton == NOT_PRESSED) {

				dspdata->lbutton = PRESSED;
				sd_on(1);

				if ((dspdata->dir = -dspdata->dir) > 0) {
					if (motion1 <= MOTION1MAX / 2) {
						motion1 = MOTION1MIN;
						dspdata->MotionType = 2;
						dspdata->MotionCnt = 0;
					} else {
						motion1 = MOTION1MAX;
						dspdata->MotionType = 4;
						dspdata->MotionCnt = 0;
					}
				} else {
					if (motion1 <= MOTION1MAX / 2) {
						motion1 = MOTION1MIN;
						dspdata->MotionType = 5;
					} else {
						motion1 = MOTION1MAX;
						dspdata->MotionType = 3;
					}
				}
			}
		} else {
			dspdata->lbutton = NOT_PRESSED;
		}
	}

	dspdata->x = x;
	dspdata->y = y;
	dspdata->vx = vx;
	dspdata->vy = vy;
	dspdata->motion1 = motion1;


								//Display processing

	if (dspdata->dir < 0 && scroll_x > -SCROLLX_DIF) {
		scroll_x -= SCROLLX_VELO;
	} else
	if (dspdata->dir > 0 && scroll_x < SCROLLX_DIF) {
		scroll_x += SCROLLX_VELO;
	}

	vx0 = screen_x0;
	vy0 = screen_y0;
	screen_x0 = x - 128 * DIV_X + scroll_x;
	screen_y0 = y / 3;

	while(graphicflag);
	radar_reset(dx, dy, 15);
	radar_scope(screen_x0, screen_y0, vx0, vy0);	//radar

	radar_set(x, y, 8);
	speedmeter(vx);					//Speedometer display
								//Graphics Drawing

	if (vx > 0) {
		draw_to_graphic((screen_x0 / DIV_X / 2) + 320, (screen_x0 / DIV_X / 2) + 320);
	} else {
		draw_to_graphic((screen_x0 / DIV_X / 2) - 64, (screen_x0 / DIV_X / 2) - 64);
	}
								//Think of patterns
	if (dspdata->MotionType >= 0) {
		number = MOTION(mf_buff[0], dspdata->MotionType, dspdata->MotionCnt);
		dspdata->MotionCnt++;
		if (MOTION(mf_buff[0], dspdata->MotionType, dspdata->MotionCnt) < 0) {
			dspdata->MotionType = -1;
			dspdata->MotionCnt = 0;
		}
	} else {
		if (dspdata->dir < 0) {
			number = PAT_LEFT + motion1 / DIV_MOTION1;
		} else {
			number = PAT_RIGHT + (MOTION1MAX - motion1) / DIV_MOTION1;
		}
	}

	dspdata->sp->x = SCX4x4(x, screen_x0);
	dspdata->sp->y = SCY4x4(y, screen_y0);
	dspdata->sp->chr = SPRADR(dsp_pat_buff, number);

	if (count_ & 1) {
		if (count_ & 2) {
			*(volatile unsigned short *) 0xe82232 = wingpalet[0];
			*(volatile unsigned short *) 0xe82234 = wingpalet[1];
		} else {
			*(volatile unsigned short *) 0xe82232 = (short)0;
			*(volatile unsigned short *) 0xe82234 = (short)0;
		}
		if (burnercnt) {
			dspdata->shadow_sp->num = burnerpat + ((count_ & 4) == 0);
			dspdata->shadow_sp->palet = BUR_PALET;
			dspdata->shadow_sp->x = SCX1x1(bx, screen_x0);
			dspdata->shadow_sp->y = SCY1x1(by, screen_y0);
		} else {
			dspdata->shadow_sp->num = SPNULL;
		}
	} else {
		*(volatile unsigned short *) 0xe82232 = (short)0;
		*(volatile unsigned short *) 0xe82234 = wingpalet[0];
		dspdata->shadow_sp->num = SHADOWNUM;
		dspdata->shadow_sp->x = SCX1x1(x, screen_x0);
		dspdata->shadow_sp->y = SCY1x1(SHADOWY, screen_y0);
	}

	return 0;

}

/*
 *		Bullet movement
 */

void	shotmove(DSP *dspdata, SHOT *shotdata)
{
	int	i;
	int	x, y, pat;
	short	dx;
	SHOT	*addr;

	addr = shotdata;

	for(i = 0; i < SHOT_NUM; i++, addr++) {

		if (addr->condition) {		// Normal movement

			x = addr->x;
			y = addr->y;

			if (addr->condition++ > 5) {
//				addr->y = (y += addr->vy);
				addr->x = (x += addr->vx);
				pat = SHOT_PATTERN;
			} else {
				addr->y = y = SHOT_Y;
				addr->x = (x += addr->vx - SGN(addr->vx, SHOT_SPEEDF));
				pat = SHOT_PATTERN - 3 + (addr->condition - 1) / 2;
			}

			addr->sp->x = dx = SCX1x1(x, screen_x0);
			addr->sp->y = SCY1x1(y, screen_y0);
			addr->sp->num = pat;
			addr->sp->palet = SHOT_PALET;

			if (dx < - 8 || dx > 264) {
				if (addr->sp) {
					SpsFree(addr->sp);
					addr->sp = (void*)0;
				}
				addr->condition = 0;
			}

		}
	}


}


int	DspInit(DSP *dspdata, SHOT *shotdata)
{
	int	i;

	dspdata->sp = SpsAlloc(CHR4x4);
	dspdata->shadow_sp = SpsAlloc(CHR1x1);
	dspdata->sp->palet = 1;
	dspdata->palet = 1;
	dspdata->dir = 9;
	dspdata->x = 0;
	dspdata->y = 2048;
	dspdata->MotionType = -1;
	dspdata->MotionCnt = 0;
	dspdata->vx = 0;
	dspdata->vy = 0;
	dspdata->energy = 1;
	dspdata->motion1 = 0;
	dspdata->lbutton = NOT_PRESSED;
	dspdata->rbutton = NOT_PRESSED;

	for(i = 0; i < SHOT_NUM; i++) {
		shotdata[i].condition = 0;
		shotdata[i].sp = (void *)0;
	}

	if (dspdata->sp == (void *)0 || dspdata->shadow_sp == (void *)0) {
		return 1;
	} else {
		return 0;
	}
}
