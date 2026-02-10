/*
 *		Event Routine
 */

#include	<stdio.h>
#include	<string.h>
#include	"sub.h"
#include	"sound.h"
#include	"enemy.h"
#include	"event.h"
#include	"graphic.h"

#define	PCG_DATA_SIZE	( 128 * 128 )

#define	PCG_AREA	( (volatile void *)0xeb8000 )
#define	BG0_DATA_AREA	( (volatile void *)0xebc000 )
#define	BG1_DATA_AREA	( (volatile void *)0xebe000 )
#define	S_PALET		( (volatile void *)0xe82200 )
#define	SP_CTRL_REGS	( (volatile void *)0xeb0000 )
#define	BG0_SCRL_REG	( (volatile void *)0xeb0800 )
#define	BG1_SCRL_REG	( (volatile void *)0xeb0804 )
#define	BG_CTRL_REG	( (volatile void *)0xeb0808 )

#define	G_PALET		( (volatile void *)0xe82000 )
#define	VCNT_R0		( (volatile short *)0xe82400 )
#define	VCNT_R1		( (volatile unsigned short *)0xe82500 )
#define	VCNT_R2		( (volatile unsigned short *)0xe82600 )

#define	CRTC_REG( x )	( (volatile unsigned short *)0xe80000 + x )
#define	TX_XSCROLL	( CRTC_REG( 10 ) )
#define	TX_YSCROLL	( CRTC_REG( 11 ) )
#define	G0_XSCROLL	( CRTC_REG( 20 ) )
#define	G0_YSCROLL	( CRTC_REG( 21 ) )
#define	G1_XSCROLL	( CRTC_REG( 22 ) )
#define	G1_YSCROLL	( CRTC_REG( 23 ) )
#define	G2_XSCROLL	( CRTC_REG( 24 ) )
#define	G2_YSCROLL	( CRTC_REG( 25 ) )
#define	G3_XSCROLL	( CRTC_REG( 26 ) )
#define	G3_YSCROLL	( CRTC_REG( 27 ) )

#define	GPIP		( (volatile unsigned char *)0xe88001 )

#define	BGMMAX		3	/* Maximum number of BGM per stage */

typedef	struct	bgm_info {
	int	count;		/* BGM play time */
	int	bgm_no;		/* BGM number */
} BGM_INFO;

static	unsigned char	boss_pcg_data[ PCG_DATA_SIZE ];

void	pcg_read_sub(char *filename, void *buf)
{
	FILE	*fp;
	fp = fopen(filename, "rb");
	if (!fp) {
		printf("%s ‚ªŒ©‚Â‚©‚ç‚È‚¢‚Å‚·B\n");
		ende();
	}

	fread(buf, PCG_DATA_SIZE, 1, fp);
	fclose(fp);
}

void	pcg_trans(volatile unsigned char *dst, unsigned char *src, short size)
{
	short	i;
	for (i = size / 2 - 1; i > 0; i--) {
		*(volatile unsigned short *)dst = *(unsigned short *)src;
		dst += 2;
		src += 2;
	}
}

void	pcg_read(int stage)
{
	static char *pcg_filename[] = {
		"stage1.sp", "stage1.sp", "stage1.sp"
	};
	static char *boss_pcg_filename[] = {
		"boss.sp", "boss.sp", "boss.sp"
	};

	pcg_read_sub(pcg_filename[ stage ], boss_pcg_data);
	pcg_trans(PCG_AREA, boss_pcg_data, PCG_DATA_SIZE);
	pcg_read_sub(boss_pcg_filename[ stage ], boss_pcg_data);
}

void	event(int stage, int count)
{
	int	i;

	static int boss_trans_count[ROUNDMAX] = { 	/* Time to transfer PCG before Boss */
		299*16, 305*16, 299*16,
	};
							/* BGM start time */
	static BGM_INFO binfo[ ROUNDMAX ][ BGMMAX ] = {
		{ { 2200, 1 } , { 4600, 2 }, { 65535, -1 } },
		{ { 2200, 3 } , { 4600, 2 }, { 65535, -1 } },
		{ { 1400, 4 } , { 4600, 2 }, { 65535, -1 } },
	};

	graphic_move();				/* Moving the background */

	if (count == boss_trans_count[stage]) {
		pcg_trans(PCG_AREA, boss_pcg_data, PCG_DATA_SIZE);
	}
	for( i = 0; i < BGMMAX; i++ ) {
		if( count == binfo[stage][i].count - 160 ) {
			bgm_fade( 48 );
		}
		if( count == binfo[stage][i].count ) {
			bgm( binfo[stage][i].bgm_no );	/* Play background music */
		}
	}
}
