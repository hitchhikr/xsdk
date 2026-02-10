/*
 *		Terrain Processing Routines
 */

#include	<stdio.h>
#include	<iocslib.h>
#include	"sub.h"
#include	"bg.h"

#define		BG_SCRNUM	16		/* Number of screens per side */
#define		BG_XSIZE	(32*BG_SCRNUM)
#define		BG_YSIZE	32

#define	BGDISP_X 34				/* New drawing position */
#define	SCROLL_SPEED ( DOT_RATE / 2 )		/* Terrain scrolling speed */

/* Terrain data file name */
static	char	*bgfile[] = {
	"stage1.bg" , "stage2.bg" , "stage3.bg"
};

static unsigned short bg_x0 = 0;		/* The x coordinate of the top left corner of the screen */
static unsigned short bg_buf[ BG_XSIZE * BG_YSIZE ];
						/* Array for storing topographical data */


/*
 *		Loading terrain data
 */

int	bgread( int stage )
{
	FILE *fp;

	bg_x0 = 0;
	fp = fopen(bgfile[ stage ] , "rb");
	if (!fp) return 1;
	fread(bg_buf, sizeof(unsigned short), BG_XSIZE * BG_YSIZE, fp);
	fclose(fp);

	return 0;
}

/*
 *		Terrain Scrolling
 */

void	bgscroll( void )
{
	unsigned short x, y, i;

	/* Draw the newly appearing terrain */
	x = bg_x0 / (8 * DOT_RATE);
	y = ( (bg_x0 / SCROLL_SPEED) & 15) * 2;

	for(i = 0; i < 2; i++){
		BGTEXTST( 1, (x + BGDISP_X) & 63, y + i,
			bg_buf[ x * BG_YSIZE + (y + i) ] );
	}

	/* Scroll the background screen */
	BGSCRLST(0x80000001, bg_x0 / DOT_RATE, 0);
	bg_x0 += SCROLL_SPEED;
}

/*
 *		Check the terrain collision detection
 */

int	bgatari( int x, int y )
{
	int dx, dy;

	dx = (x / DOT_RATE + bg_x0 / DOT_RATE) / 8 - BGDISP_X;
	dy = y / DOT_RATE / 8;
	if( dx < 0 ) return 0;

	if( ( bg_buf[dx * BG_YSIZE + dy] & 255 ) >= 252  ) return 2;
	if( bg_buf[dx * BG_YSIZE + dy] & 255 ) return 1;
	return 0;

}

/*
 *		Make the characters written in the background blank
 */

void	bgbreak( int x, int y )
{
	int dx, dy;

	dx = (x / DOT_RATE + bg_x0 / DOT_RATE) / 8 - BGDISP_X;
	dy = y / DOT_RATE / 8;

	bg_buf[dx * BG_YSIZE + dy] = 0;
	BGTEXTST( 1, (dx + BGDISP_X) & 63, dy,
		bg_buf[ dx * BG_YSIZE + dy ] );
}
