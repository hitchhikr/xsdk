/*	spdisp.c : sprite display program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<doslib.h>
#include	<math.h>
#include	"calc.h"
#include	"star.h"
#include	"txdisp.h"

#define	ME_CRASH_TIME_MAX	32

#define	SCOPE	0x01
#define	H_STICK	12
#define	V_STICK	13
#define	VERT	14

static unsigned short	*patbuf;
static unsigned short	*palbuf;
static int	shake_x[ME_CRASH_TIME_MAX], shake_y[ME_CRASH_TIME_MAX];

void	SpClr(void)
{
	int	i;
	
	for (i = 0; i < 128 * 4; ++i) *(SPCTL + i) = 0;		/*	sprite	*/
}

void	BgClr(void)
{
	int	i;
	
	for (i = 0; i < 64 * 64; ++i) *BGDT(i) = 0x0100;	/*	bg	*/
}

void	SpBgClr(void)
{
	int	i;
	
	for (i = 0; i < 128 * 4; ++i) *(SPCTL + i) = 0;		/*	sprite	*/
	for (i = 0; i < 64 * 64; ++i) *BGDT(i) = 0x0100;	/*	bg	*/
}

void	SetPat(void)
{
	int	i;
	unsigned short	*src;

	src = patbuf;
	for (i = 0; i < 96 * 2 * 8 * 4; ++i)	*(PCG + i) = *src++;
	if ( 0 > MFREE( patbuf ) ) {
		fputs("pattern buffer malloc free error\n", stdout);
	}
	src = palbuf;
	for (i = 0; i < 16 * 15; ++i)		*(PAL + i) = *src++;
	if ( 0 > MFREE( palbuf ) ) {
		fputs("palet buffer malloc free error\n", stdout);
	}
}

void	ScrollBg(int x, int y)
{
	*BGSC_X = x;		/*	BG scroll register X */
	*BGSC_Y = y;		/*	BG scroll register Y */
}

void	InitSp(void)
{
	FILE	*fp;
	int	size, i;
	const char	*patname = "nuka.sp";
	const char	*palname = "nuka.pal";
	
	SP_ON();
	*(volatile unsigned short *)0xeb0808 = 0x0233;	/*	BG Controller	*/
	ScrollBg(0, 0);
	SpBgClr();
	/*	Sprite Pattern Loading	*/
	if ((fp = fopen(patname, "rb")) == NULL) {
		fprintf(stdout, "file %s open error\n", patname);
		exit(1);
	}
	size = FILELENGTH(fp);
	if ((patbuf = (unsigned short *)MALLOC(size)) >= (unsigned short *)0x81000000) {
		fclose(fp);
		fputs("sprite pattern out of memory\n", stdout);
		exit(1);
	}
	printf("sprite pattern %s now reading ...\n", patname);
	fread(patbuf, 1, size, fp);
	fclose(fp);
	/*	Sprite Palette Loading	*/
	if ((fp = fopen(palname, "rb")) == NULL) {
		fprintf(stdout, "file %s open error\n", palname);
		exit(1);
	}
	size = FILELENGTH(fp);
	if ((palbuf = (unsigned short *)MALLOC(size)) >= (unsigned short *)0x81000000) {
		fclose(fp);
		fputs("sprite palette out of memory\n", stdout);
		exit(1);
	}
	printf("sprite palette %s now reading ...\n", palname);
	fread(palbuf, 1, size, fp);
	fclose(fp);
	
	SetPat();

	for (i = 0; i < ME_CRASH_TIME_MAX; ++i) {
		int	sign_x = (rand() & 4096) / 2048 - 1;
		int	sign_y = (rand() & 4096) / 2048 - 1;
		
		shake_x[i] = sign_x * sqrt( (double)( i * (rand() & 4095) ) ) / 16;
		shake_y[i] = sign_y * sqrt( (double)( i * (rand() & 4095) ) ) / 16;
	}
}

void	DispBg8(int x, int y, int pat, int col)
{
	volatile unsigned short *bgdt = BGDT(x + (y << 6));
	
	*bgdt = pat + (col << 8);
}

void	DispBg16(int x, int y, int pat, int col)
{
	volatile unsigned short *bgdt = BGDT(x + (y << 6));
	
	pat <<= 2;
	*bgdt++ = pat		+ (col << 8);
	*bgdt	= pat + 2	+ (col << 8);		bgdt += 63;
	*bgdt++ = pat + 1	+ (col << 8);
	*bgdt	= pat + 3	+ (col << 8);
}

void	DrawTarget(void)
{
	DispBg16(30, 30, SCOPE + 0x00, 5);
	DispBg16(32, 30, SCOPE + 0x01, 5);
	DispBg16(30, 32, SCOPE + 0x10, 5);
	DispBg16(32, 32, SCOPE + 0x11, 5);
}

void	DispTarget(void)
{
	static int	old_u, old_r, old_d, old_l;
	volatile unsigned short	*bg;
	int		i, crash;

	if (me.crash) {
		if (me.crash >= ME_CRASH_TIME_MAX) {
			crash = 0;
		} else {
			crash = me.crash;
		}
		ScrollBg(128 - me.dx / 2 - shake_x[crash], 128 + me.dy / 2 - shake_y[crash]);
		ScrollText(-shake_x[crash], -shake_y[crash]);
		*TARGET_PAL = RGB(31, 0, 0);
		DamageColor(1);
	} else {
		ScrollBg(128 - me.dx / 2, 128 + me.dy / 2);
		ScrollText(0, 0);
		*TARGET_PAL = RGB(0, 31, 0);
		DamageColor(0);
	}
	
	if (old_u != range_u && range_u < 24) {
		bg = BGDT(29 + ((28 - old_u) << 6));
		for (i = 29; i < 35; ++i) *bg++ = 0;
		
		bg = BGDT(29 + ((28 - range_u) << 6));
		*bg++ = VERT + (5 << 8);
		for (i = 30; i < 34; ++i) *bg++ = H_STICK + (5 << 8);
		*bg = VERT + (5 << 8) + 0x4000;
	}
	if (old_r != range_r && range_r < 24) {
		bg = BGDT(35 + old_r + (29 << 6));
		for (i = 29; i < 35; ++i) {
			*bg = 0;	bg += 64;
		}
		bg = BGDT(35 + range_r + (29 << 6));
		*bg = VERT + (5 << 8) + 0x4000;		bg += 64;
		for (i = 30; i < 34; ++i) {
			*bg = V_STICK + (5 << 8);	bg += 64;
		}
		*bg = VERT + (5 << 8) + 0xc000;
	}
	if (old_d != range_d && range_d < 24) {
		bg = BGDT(29 + ((35 + old_d) << 6));
		for (i = 29; i < 35; ++i) *bg++ = 0;
		
		bg = BGDT(29 + ((35 + range_d) << 6));
		*bg++ = VERT + (5 << 8) + 0x8000;
		for (i = 30; i < 34; ++i) *bg++ = H_STICK + (5 << 8);
		*bg = VERT + (5 << 8) + 0xc000;
	}
	if (old_l != range_l && range_l < 24) {
		bg = BGDT(28 - old_l + (29 << 6));
		for (i = 29; i < 35; ++i) {
			*bg = 0;	bg += 64;
		}
		bg = BGDT(28 - range_l + (29 << 6));
		*bg = VERT + (5 << 8);			bg += 64;
		for (i = 30; i < 34; ++i) {
			*bg = V_STICK + (5 << 8);	bg += 64;
		}
		*bg = VERT + (5 << 8) + 0x8000;
	}
	old_u = range_u;
	old_r = range_r;
	old_d = range_d;
	old_l = range_l;
}
