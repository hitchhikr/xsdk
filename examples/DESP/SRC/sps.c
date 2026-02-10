
#include	<stdio.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<sprite.h>
#include	<basic0.h>

#include	"sps.h"


/*	I added it for Chika's convenience.	*/
int	spsflag = 0;		/* 0...End of display 1...Displaying */
volatile int	graphicflag = 0;	/* Between RASTER2 and VDISP, it is 1 */
short	GScroll[8];		/* The value written to the graphics scroll register */
/* Interrupt position */
#define RASTER1_H (130*2)
#define RASTER2_H (228*2)
#define RASTER1_L (130*1)
#define RASTER2_L (228*1)

short	RASTER1 = RASTER1_H;
short	RASTER2 = RASTER2_H;

/* The boundary when dividing a sprite into upper and lower parts */
#define   UPPER_LINE   135
#define   LOWER_LINE   85

/* Number of sprites, etc. */
#define   SP_PRW   3
#define   SP_REG_BANK   2
#define   SP_REG_NUM   128

/* Display area */
#define   X_MIN   (-32)
#define   X_MAX   (256+32)
#define   Y_MIN   (-32)
#define   Y_MAX   (208+32)

/* Number to be saved */
#define   CACHE_NUM   2
short sps_cache_num = 0;

/* Sprite register virtual buffer (top and bottom) */
SP_REG sps_reg_data[CACHE_NUM][SP_REG_BANK][SP_REG_NUM];
short sps_reg_bank = 0;

/* SPRITE PCG virtual buffer */
unsigned char *sps_pcg_4x4[PCG_4x4_MAX], *sps_pcg_3x3[PCG_3x3_MAX]
	, *sps_pcg_2x2[PCG_2x2_MAX];
unsigned char *sps_now_4x4[PCG_4x4_MAX], *sps_now_3x3[PCG_3x3_MAX]
	, *sps_now_2x2[PCG_2x2_MAX];

/* Switching between sp128 = 0 and sp256 = 0 */
short sps_reg_mode = 0;

/* Flag that the sprite was transferred by the interrupt routine */
short sps_reg_flag = 0;

/* Data Buffer */
static SPS sps_buff[SPS_MAX];

/* The beginning of the buffer to start searching when allocating or freeing */
static SPS *sps_used = 0, *sps_empty = 0;

/* The number of sprites currently in use */
static short num_of_used_sprite = 0;

#define		DMACH2		(0xe84080)
typedef struct DMAREG {
	unsigned char csr;
	unsigned char cer;
	unsigned short dummy1;
	unsigned char dcr;
	unsigned char ocr;
	unsigned char scr;
	unsigned char ccr;
	unsigned short dummy2;
	unsigned short mtc;
	unsigned char *mar;
	unsigned long dummy3;
	volatile unsigned char *dar;
	unsigned short dummy4;
	unsigned short btc;
	unsigned char *bar;
	unsigned long dummy5;
	unsigned char dummy6;
	unsigned char niv;
	unsigned char dummy7;
	unsigned char eiv;
	unsigned char dummy8;
	unsigned char mfc;
	unsigned short dummy9;
	unsigned char dummy10;
	unsigned char cpr;
	unsigned short dummy11;
	unsigned char dummy12;
	unsigned char dfc;
	unsigned long dummy13;
	unsigned short dummy14;
	unsigned char dummy15;
	unsigned char bfc;
	unsigned long dummy16;
	unsigned char dummy17;
	unsigned char gcr;
} DMA_REG;

/* 32-bit transfer */
#define   TRANS_DMA(adrs, src, dst, count_) \
	{ \
		(adrs)->csr = 0xff; \
		(adrs)->dcr = 0x08; \
		(adrs)->ocr = 0x21; \
		(adrs)->scr = 0x05; \
		(adrs)->ccr = 0x00; \
		(adrs)->cpr = 0x03; \
		(adrs)->mfc = 0x05; \
		(adrs)->dfc = 0x05; \
		(adrs)->mtc = (count_); \
		(adrs)->mar = (src); \
		(adrs)->dar = (dst); \
		(adrs)->ccr |= 0x80; \
	}

inline void
SpsPat(void) {
	short i;
	volatile unsigned char *dst;
	volatile DMA_REG *dma = (DMA_REG *) DMACH2;

	if (sps_reg_flag == 0) return;

	for(i = 0, dst = (volatile unsigned char *) PCG_4x4_ADRS; i < PCG_4x4_MAX
	 ; i ++, dst += PCG_4x4_SIZE) {
		if (sps_pcg_4x4[i] == 0) continue;
		if (sps_now_4x4[i] == sps_pcg_4x4[i]) continue;

		/* This is the transfer point */
		TRANS_DMA(dma, sps_pcg_4x4[i], dst, PCG_4x4_SIZE / 4);

		sps_now_4x4[i] = sps_pcg_4x4[i];
	}

	for(i = 0, dst = (volatile unsigned char *) PCG_3x3_ADRS; i < PCG_3x3_MAX
	 ; i ++, dst += PCG_3x3_SIZE) {
		if (sps_pcg_3x3[i] == 0) continue;
		if (sps_now_3x3[i] == sps_pcg_3x3[i]) continue;

		/* This is the transfer point */
		TRANS_DMA(dma, sps_pcg_3x3[i], dst, PCG_3x3_SIZE / 4);

		sps_now_3x3[i] = sps_pcg_3x3[i];
	}

	for(i = 0, dst = (volatile unsigned char *) PCG_2x2_ADRS; i < PCG_2x2_MAX
	 ; i ++, dst += PCG_2x2_SIZE) {
		if (sps_pcg_2x2[i] == 0) continue;
		if (sps_now_2x2[i] == sps_pcg_2x2[i]) continue;

		/* This is the transfer point */
		TRANS_DMA(dma, sps_pcg_2x2[i], dst, PCG_2x2_SIZE / 4);

		sps_now_2x2[i] = sps_pcg_2x2[i];
	}
}

void __attribute__((interrupt)) SpsReg(void) {
	static short cache = 0;
	unsigned char *src;
    volatile unsigned char *dst;
	DMA_REG *dma = (DMA_REG *) DMACH2;
	unsigned char *adrs[8] = {
		(unsigned char *) sps_reg_data[0][0],
		(unsigned char *) sps_reg_data[0][0],
		(unsigned char *) sps_reg_data[0][0],
		(unsigned char *) sps_reg_data[0][1],
		(unsigned char *) sps_reg_data[1][0],
		(unsigned char *) sps_reg_data[1][0],
		(unsigned char *) sps_reg_data[1][0],
		(unsigned char *) sps_reg_data[1][1]
	};

	if (sps_reg_mode == 0 || sps_reg_bank == 0) {
		/* Cut the sprites */
		*((unsigned short *) 0xeb0808) &= 0x00ff;
		graphicflag = 1;

		/* Scrolling the graphic screen by chika */
		*((volatile short *) 0xe80018) = GScroll[0];
		*((volatile short *) 0xe8001a) = GScroll[1];
		*((volatile short *) 0xe8001c) = GScroll[2];
		*((volatile short *) 0xe8001e) = GScroll[3];
		*((volatile short *) 0xe80020) = GScroll[4];
		*((volatile short *) 0xe80022) = GScroll[5];
		*((volatile short *) 0xe80024) = GScroll[6];
		*((volatile short *) 0xe80026) = GScroll[7];

	}

	if (sps_reg_mode) {
		if (sps_reg_bank) {
			cache = sps_cache_num;
			sps_reg_bank = 0;
			*((volatile unsigned short *) 0xe80012) = RASTER2;
			src = adrs[cache * 4 + sps_reg_mode * 2 + 1];
			dst = (volatile unsigned char *) 0xeb0000;
			TRANS_DMA(dma, src, dst, SP_REG_NUM * 2);
		} else {
			sps_reg_bank = 1;
			*((volatile unsigned short *) 0xe80012) = RASTER1;
			SpsPat();
			if (sps_reg_flag) {
				spsflag = 0;
				sps_reg_flag = 0;
			}
			src = adrs[cache * 4 + sps_reg_mode * 2];
			dst = (volatile unsigned char *) 0xeb0000;
			TRANS_DMA(dma, src, dst, SP_REG_NUM * 2);
			/* Add sprites */
			*((volatile unsigned short *) 0xeb0808) |= 0x0200;
		}
	} else {
		cache = sps_cache_num;
		SpsPat();
		if (sps_reg_flag) {
			spsflag = 0;
			sps_reg_flag = 0;
		}
		*((volatile unsigned short *) 0xe80012) = RASTER2;
		src = adrs[cache * 4 + sps_reg_mode * 2];
		dst = (unsigned char *) 0xeb0000;
		TRANS_DMA(dma, src, dst, SP_REG_NUM * 2);
		/* Add sprites */
		*((volatile unsigned short *) 0xeb0808) |= 0x0200;
	}


}

void SpsInit(void) {
	short i;

	/* Sprite initialization */
#if 0
	screen(0, 2, 1, 1); // screen mode : 256*256 (256colors)
	sp_disp(1);
#endif
	/* Interrupts Off */
	CRTCRAS(0, 0);

	/* initialize PCG buffer */
	for(i = 0; i < PCG_4x4_MAX; i ++) {
		sps_now_4x4[i] = 0;
		sps_pcg_4x4[i] = 0;
	}
	for(i = 0; i < PCG_3x3_MAX; i ++) {
		sps_now_3x3[i] = 0;
		sps_pcg_3x3[i] = 0;
	}
	for(i = 0; i < PCG_2x2_MAX; i ++) {
		sps_now_2x2[i] = 0;
		sps_pcg_2x2[i] = 0;
	}

	/* Initialize the internal data of the buffer */
	for(i = 0; i < SPS_MAX; i ++) {
		sps_buff[i].chr = 0;
		sps_buff[i].condition = -1;
		sps_buff[i]._table_num = i;
		sps_buff[i]._pre = &sps_buff[i - 1];
		sps_buff[i]._post = &sps_buff[i + 1];
	}
	sps_buff[0]._pre = 0;
	sps_buff[SPS_MAX - 1]._post = 0;

	/* Initialize the sprite register buffer */
	for(i = 0; i < CACHE_NUM; i ++) {
		short j;

		for(j = 0; j < SP_REG_BANK; j ++) {
			short k;

			for(k = 0; k < SP_REG_NUM; k ++) {
				sps_reg_data[i][j][k].prw.dummy = 0;
			}
		}
	}

	/* Initialization of each static variable */
	sps_used = 0;
	sps_empty = &sps_buff[0];

	num_of_used_sprite = 0;
	sps_reg_mode = 0;
	sps_cache_num = 0;
	sps_reg_flag = 0;
	sps_reg_bank = 0;

	/* Barge On */
	CRTCRAS((unsigned char *) SpsReg, RASTER2);
}

void
SpsExit(void) {
	CRTCRAS(0, 0);
}

#define   CHR1x1XOFF   8
#define   CHR1x1YOFF   8
#define   CHR2x2XOFF   16
#define   CHR2x2YOFF   16
#define   CHR3x3XOFF   24
#define   CHR3x3YOFF   24
#define   CHR4x4XOFF   32
#define   CHR4x4YOFF   32

#define   PUT_REG0(cache, loc, xx, yy, num, pal, xoff, yoff) \
	{ \
	sps_reg_data[(cache)][0][(loc)].x = (xx) + (xoff); \
	sps_reg_data[(cache)][0][(loc)].y = (yy) + (yoff); \
	sps_reg_data[(cache)][0][(loc)].prw.dummy = SP_PRW; \
	sps_reg_data[(cache)][0][(loc)].ctrl.dummy = (num) + ((pal) & 0x0f) * 256; \
	}

#define   PUT_REG1(cache, loc, xx, yy, num, pal, xoff, yoff) \
	{ \
	sps_reg_data[(cache)][1][(loc)].x = (xx) + (xoff); \
	sps_reg_data[(cache)][1][(loc)].y = (yy) + (yoff); \
	sps_reg_data[(cache)][1][(loc)].prw.dummy = SP_PRW; \
	sps_reg_data[(cache)][1][(loc)].ctrl.dummy = (num) + ((pal) & 0x0f) * 256; \
	}

void
Sps128(short num) {
	short i, j, sp_loc;
	SPS *sp = sps_used;

	for(sp_loc = 0; sp && sp_loc < SP_REG_NUM; sp = sp->_post) {
		register short x = sp->x, y = sp->y;

		if (x > X_MAX || y > Y_MAX || x < X_MIN || y < Y_MIN) continue;

		// if (sp->condition < 0) continue;
		switch(sp->condition) {
			default:
				break;
			case CHR1x1:
				PUT_REG0(num, sp_loc, x, y, sp->num,
						sp->palet, CHR1x1XOFF, CHR1x1YOFF);
				sp_loc += 1;
				break;
			case CHR2x2:
				if (sp_loc + 4 > SP_REG_NUM) break;
				PUT_REG0(num, sp_loc, x, y, sp->num,
						sp->palet, 0, 0);
				sp_loc += 1;
				PUT_REG0(num, sp_loc, x, y, sp->num + 1,
						sp->palet, CHR2x2XOFF, 0);
				sp_loc += 1;
				PUT_REG0(num, sp_loc, x, y, sp->num + 2,
						sp->palet, 0, CHR2x2YOFF);
				sp_loc += 1;
				PUT_REG0(num, sp_loc, x, y, sp->num + 3,
						sp->palet, CHR2x2XOFF, CHR2x2YOFF);
				sp_loc += 1;
				break;
			case CHR3x3:
				for(j = 0; j < 3 && sp_loc + 3 <= SP_REG_NUM; j ++) {
					PUT_REG0(num, sp_loc, x, y, sp->num + 0 + j * 3,
							sp->palet, 16 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
					sp_loc += 1;
					PUT_REG0(num, sp_loc, x, y, sp->num + 1 + j * 3,
							sp->palet, 32 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
					sp_loc += 1;
					PUT_REG0(num, sp_loc, x, y, sp->num + 2 + j * 3,
							sp->palet, 48 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
					sp_loc += 1;
				}
				break;
			case CHR4x4:
				for(j = 0; j < 4 && sp_loc + 4 <= SP_REG_NUM; j ++) {
					PUT_REG0(num, sp_loc, x, y, sp->num + 0 + j * 4,
							sp->palet, 16 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
					sp_loc += 1;
					PUT_REG0(num, sp_loc, x, y, sp->num + 1 + j * 4,
							sp->palet, 32 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
					sp_loc += 1;
					PUT_REG0(num, sp_loc, x, y, sp->num + 2 + j * 4,
							sp->palet, 48 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
					sp_loc += 1;
					PUT_REG0(num, sp_loc, x, y, sp->num + 3 + j * 4,
							sp->palet, 64 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
					sp_loc += 1;
				}
				break;
		}
	}
	for(; sp_loc < SP_REG_NUM; sp_loc ++) {
		sps_reg_data[num][0][sp_loc].prw.dummy = 0;
	}
}

void
Sps256(short cache) {
	short i, sp_loc_a, sp_loc_b;
	SPS *sp = sps_used;

	for(sp_loc_a = 0, sp_loc_b = 0; sp; sp=sp->_post) {
		register short x = sp->x, y = sp->y;

		if (x > X_MAX || y > Y_MAX || x < X_MIN || y < Y_MIN) continue;
		
		// if (sp->condition < 0) continue;
		switch(sp->condition) {
			short j;
			default:
				break;
			case CHR1x1:
				if (y < UPPER_LINE && sp_loc_a < SP_REG_NUM) {
					PUT_REG0(cache, sp_loc_a, x, y, sp->num,
							sp->palet, CHR1x1XOFF, CHR1x1YOFF);
					sp_loc_a += 1;
				}
				if (y > LOWER_LINE && sp_loc_b < SP_REG_NUM) {
					PUT_REG1(cache, sp_loc_b, x, y, sp->num,
							sp->palet, CHR1x1XOFF, CHR1x1YOFF);
					sp_loc_b += 1;
				}
				break;
			case CHR2x2:
				for(j = 0; j < 2; j ++) {
					if (y + j * 16 < UPPER_LINE && sp_loc_a + 2 <= SP_REG_NUM) {
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + j * 2,
								sp->palet, 0, CHR2x2YOFF * j);
						sp_loc_a += 1;
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + j * 2 + 1,
								sp->palet, CHR2x2XOFF, CHR2x2YOFF * j);
						sp_loc_a += 1;
					}
					if (y + j * 16 > LOWER_LINE && sp_loc_b + 2 <= SP_REG_NUM) {
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + j * 2,
								sp->palet, 0, CHR2x2YOFF * j);
						sp_loc_b += 1;
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + j * 2 + 1,
								sp->palet, CHR2x2XOFF, CHR2x2YOFF * j);
						sp_loc_b += 1;
					}
				}
				break;
			case CHR3x3:
				for(j = 0; j < 3; j ++) {
					if (y + (j + 1) * 16 - CHR3x3YOFF < UPPER_LINE && sp_loc_a + 3 <= SP_REG_NUM) {
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 0 + j * 3,
							sp->palet, 16 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
						sp_loc_a += 1;
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 1 + j * 3,
							sp->palet, 32 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
						sp_loc_a += 1;
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 2 + j * 3,
							sp->palet, 48 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
						sp_loc_a += 1;
					}
					if (y + (j + 1) * 16 - CHR3x3YOFF > LOWER_LINE && sp_loc_b + 3 <= SP_REG_NUM) {
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 0 + j * 3,
							sp->palet, 16 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
						sp_loc_b += 1;
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 1 + j * 3,
							sp->palet, 32 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
						sp_loc_b += 1;
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 2 + j * 3,
							sp->palet, 48 - CHR3x3XOFF, (j + 1) * 16 - CHR3x3YOFF);
						sp_loc_b += 1;
					}
				}
				break;
			case CHR4x4:
				for(j = 0; j < 4; j ++) {
					if (y + (j + 1) * 16 - CHR4x4YOFF < UPPER_LINE && sp_loc_a + 4 <= SP_REG_NUM) {
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 0 + j * 4,
							sp->palet, 16 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_a += 1;
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 1 + j * 4,
							sp->palet, 32 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_a += 1;
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 2 + j * 4,
							sp->palet, 48 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_a += 1;
						PUT_REG0(cache, sp_loc_a, x, y, sp->num + 3 + j * 4,
							sp->palet, 64 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_a += 1;
					}
					if (y + (j + 1) * 16 - CHR4x4YOFF > LOWER_LINE && sp_loc_b + 4 <= SP_REG_NUM) {
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 0 + j * 4,
							sp->palet, 16 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_b += 1;
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 1 + j * 4,
							sp->palet, 32 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_b += 1;
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 2 + j * 4,
							sp->palet, 48 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_b += 1;
						PUT_REG1(cache, sp_loc_b, x, y, sp->num + 3 + j * 4,
							sp->palet, 64 - CHR4x4XOFF, (j + 1) * 16 - CHR4x4YOFF);
						sp_loc_b += 1;
					}
				}
				break;
		}
	}
	// printf("[1;1H bank0:%3d  bank1:%3d", sp_loc_a, sp_loc_b);
	for(; sp_loc_a < SP_REG_NUM; sp_loc_a ++) {
		sps_reg_data[cache][0][sp_loc_a].prw.dummy = 0;
	}
	for(; sp_loc_b < SP_REG_NUM; sp_loc_b ++) {
		sps_reg_data[cache][1][sp_loc_b].prw.dummy = 0;
	}
}

short
Spsys(void) {
	short i, j, num;
	SPS *sps_nump[SPS_MAX];
	short loc;
	short loc_4x4, loc_3x3, loc_2x2;
	SPS *sp = sps_used;

	if (sps_reg_flag) return num_of_used_sprite;
	num = sps_cache_num == 0 ? 1 : 0;

	/* PCG assign */
	for(i = 0; i < PCG_4x4_MAX; i ++) sps_pcg_4x4[i] = 0;
	for(i = 0; i < PCG_3x3_MAX; i ++) sps_pcg_3x3[i] = 0;
	for(i = 0; i < PCG_2x2_MAX; i ++) sps_pcg_2x2[i] = 0;

	for(i = 0, loc = 0; sp; sp = sp->_post) {
		short k;
		
		if (sp->chr == 0) continue;
		switch(sp->condition) {
			case CHR4x4:
				for(k = 0; k < PCG_4x4_MAX; k ++) {
					if (sp->chr == sps_now_4x4[k]) {
						sp->num = PCG_4x4_START + k * 16;
						sps_pcg_4x4[k] = sp->chr;
						break;
					}
				}
				if (k == PCG_4x4_MAX) sps_nump[loc ++] = sp;
				break;
			case CHR3x3:
				for(k = 0; k < PCG_3x3_MAX; k ++) {
					if (sp->chr == sps_now_3x3[k]) {
						sp->num = PCG_3x3_START + k * 9;
						sps_pcg_3x3[k] = sp->chr;
						break;
					}
				}
				if (k == PCG_3x3_MAX) sps_nump[loc ++] = sp;
				break;
			case CHR2x2:
				for(k = 0; k < PCG_2x2_MAX; k ++) {
					if (sp->chr == sps_now_2x2[k]) {
						sp->num = PCG_2x2_START + k * 4;
						sps_pcg_2x2[k] = sp->chr;
						break;
					}
				}
				if (k == PCG_2x2_MAX) sps_nump[loc ++] = sp;
				break;
		}
	}

	for(i = 0, loc_4x4 = 0, loc_3x3 = 0, loc_2x2 = 0; i < loc; i ++) {
		switch((sps_nump[i])->condition) {
			case CHR4x4:
				for(j = 0; j <= loc_4x4 && j < PCG_4x4_MAX; j ++) {
					if (sps_pcg_4x4[j] == (sps_nump[i])->chr) {
						(sps_nump[i])->num = PCG_4x4_START + j * 16;
						break;
					}
				}
				if (j <= loc_4x4) break;
				for(; loc_4x4 < PCG_4x4_MAX; loc_4x4 ++) {
					if (sps_pcg_4x4[loc_4x4] == 0) {
						sps_pcg_4x4[loc_4x4] = (sps_nump[i])->chr;
						(sps_nump[i])->num = PCG_4x4_START + loc_4x4 * 16;
						break;
					}
				}
				break;
			case CHR3x3:
				for(j = 0; j <= loc_3x3 && j < PCG_3x3_MAX; j ++) {
					if (sps_pcg_3x3[j] == (sps_nump[i])->chr) {
						(sps_nump[i])->num = PCG_3x3_START + j * 9;
						break;
					}
				}
				if (j <= loc_3x3) break;
				for(; loc_3x3 < PCG_3x3_MAX; loc_3x3 ++) {
					if (sps_pcg_3x3[loc_3x3] == 0) {
						sps_pcg_3x3[loc_3x3] = (sps_nump[i])->chr;
						(sps_nump[i])->num = PCG_3x3_START + loc_3x3 * 9;
						break;
					}
				}
				break;
			case CHR2x2:
				for(j = 0; j <= loc_2x2 && j < PCG_2x2_MAX; j ++) {
					if (sps_pcg_2x2[j] == (sps_nump[i])->chr) {
						(sps_nump[i])->num = PCG_2x2_START + j * 4;
						break;
					}
				}
				if (j <= loc_2x2) break;
				for(; loc_2x2 < PCG_2x2_MAX; loc_2x2 ++) {
					if (sps_pcg_2x2[loc_2x2] == 0) {
						sps_pcg_2x2[loc_2x2] = (sps_nump[i])->chr;
						(sps_nump[i])->num = PCG_2x2_START + loc_2x2 * 4;
						break;
					}
				}
				break;
		}
	}

#ifdef EBUG
	for(i = 0; i < PCG_4x4_MAX; i ++) {
		printf("[%d;1H%6X : %6X", i + 1, sps_pcg_4x4[i], sps_now_4x4[i]);
	}
#endif

	/* assign sp-reg */
	if (num_of_used_sprite <= 128) {
		/* sp128 */
		Sps128(num);
		sps_reg_mode = 0;
	} else {
		/* sp256 */
		Sps256(num);
		sps_reg_mode = 1;
	}

	sps_cache_num = num;
	sps_reg_flag = 1;

	return num_of_used_sprite;
}

SPS *
SpsAlloc(short size) {
#ifdef EBUG
	SPS *dummy;
#endif
	SPS *buff;

	/* If the buffer is full, returns null. */
	if (!sps_empty) return 0;
	else {
		buff = sps_empty;
		sps_empty = sps_empty->_post;

		buff->_post = sps_used;
		if (sps_used) sps_used->_pre = buff;

		buff->_pre = 0;
		sps_used = buff;
	}

	switch(size) {
		case CHR4x4:
			num_of_used_sprite += 16;
			break;
		case CHR3x3:
			num_of_used_sprite += 9;
			break;
		case CHR2x2:
			num_of_used_sprite += 4;
			break;
		case CHR1x1:
			num_of_used_sprite += 1;
			break;
	}

	buff->chr = 0;
	buff->condition = size;

#ifdef EBUG
	printf("\nlist of SPS allocation\n");
	for(dummy = sps_used; dummy; dummy = dummy->_post) {
		printf("table number:%d\n", dummy->_table_num);
	}
#endif
	return buff;
}

void
SpsFree(SPS *buff) {
#ifdef EBUG
	SPS *dummy;
#endif

/*	if (buff == (void *) 0) return;	*/

	if (buff->_pre) (buff->_pre)->_post = buff->_post;
	if (buff->_post) (buff->_post)->_pre = buff->_pre;
	if (buff == sps_used) sps_used = buff->_post;

	buff->_pre = 0;
	buff->_post = sps_empty;
	buff->chr = 0;
	sps_empty = buff;

	switch(buff->condition) {
		case CHR4x4:
			num_of_used_sprite -= 16;
			break;
		case CHR3x3:
			num_of_used_sprite -= 9;
			break;
		case CHR2x2:
			num_of_used_sprite -= 4;
			break;
		case CHR1x1:
			num_of_used_sprite -= 1;
			break;
	}

	buff->condition = -1;

#ifdef EBUG
	printf("\nlist of SPS free\n");
	for(dummy = sps_empty; dummy; dummy = dummy->_post) {
		printf("table number:%d\n", dummy->_table_num);
	}
#endif
}


void	ScrInit(int screenmode)
{
	if (screenmode & 1) {
		RASTER1 = RASTER1_L;
		RASTER2 = RASTER2_L;
		CRTMOD(screenmode);
	} else {
		CRTMOD(screenmode);
		RASTER1 = RASTER1_H;
		RASTER2 = RASTER2_H;
	}
}
