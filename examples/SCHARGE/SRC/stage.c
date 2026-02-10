/*
 *	stage.c
 */

#include	"stdmomo.h"
#include	"battle.h"
/*#include	"graphic.h"*/
#include	"stage.h"
#include	"bg.h"
#include	"etc.h"

stage	stagedat[STAGENUM];
stage	*nowstage;
int	stageno = 0;
extern	char	*pic_loadpath;

void	initstage(void)
{
	FILE	*fp;

	fp = fopen("stage.dat", "rb");
	if (fp){
		fread(stagedat, sizeof(stage), STAGENUM, fp);
		fclose(fp);
	} else {
		exit(1);
	}
	load_pattern();
}

#define	BGNUM	11
uword	*colbuffer[BGNUM];
uword	*cmdbuffer[BGNUM];
static	char *bgfile[BGNUM] = {"title.dp", "select.dp",
	"back1.dp", "back2.dp", "back3.dp", "back4.dp", "back5.dp",
	"back6.dp", "back7.dp", "back8.dp", "ending.dp"};
static	char strbuf[256];

void	bginit( void )
{
	FILE	*fp;
	char	tmp = 0;
	int	i, colsize, cmdsize;

	for (i = 0; i < BGNUM; i++){
		sprintf(strbuf, "%s/%s", pic_loadpath, bgfile[i]);
		fp = fopen(strbuf, "rb");
		if (!fp) { printf("dpfile %s not found!\n", strbuf);error("abort");}

		fread( &cmdsize, 4, 1, fp );
		fread( &cmdsize, 4, 1, fp );
		fread( &colsize, 4, 1, fp );
		printf("loading %s  \tcmd %7d/col %7d/total %7d bytes\n",
			strbuf, cmdsize, colsize*2, cmdsize + colsize *2);
		if ((cmdbuffer[i] = (uword *)MALLOC(cmdsize)) >= (uword *)0x81000000) {
			error("memory cannot allocated(in function bginit).");
		}
		if ((colbuffer[i] = (uword *)MALLOC(colsize*2)) >= (uword *)0x81000000) {
			error("memory cannot allocated(in function bginit).");
		}
		fread(cmdbuffer[i], 1, cmdsize, fp);
		fread(colbuffer[i], 2, colsize, fp);
		fclose(fp);
	}
}

void	bgdisp( int no )
{
	if (no >= BGNUM) no = BGNUM;
/*	send_picture(picbuffer[no]);	*/
	send_picture1(cmdbuffer[no], colbuffer[no]);
}

void	background( int backno, int sno )
{
	int	x, y;
	bgdisp( backno );
	stageno = sno;
	nowstage = stagedat + stageno;
	for( x = 0; x< 16;x++ ){
		for( y = 0; y< 16;y++ ){
			if( (nowstage->yuka)[y+1][x+1]) put_pattern3( x,y, 0);
		}
	}
}

int	yukadata( int x, int y )
{
	return ( (nowstage->yuka)[YY(y)][YX(x)] );
}

/*
 *		A routine to correct the initial position of the player's ship so that it is on the floor.
 */
void	adjust_initial_location( void )
{
	man	*mp = mandat;

	mp->x = nowstage->x0 * 16 * DIVX - 256;
	mp->y = nowstage->y0 * 16 * DIVX - 256;
	mp++;
	mp->x = nowstage->x1 * 16 * DIVX - 256;
	mp->y = nowstage->y1 * 16 * DIVX - 256;
}

