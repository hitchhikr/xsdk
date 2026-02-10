/*	timelist.c : time chart program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"timelist.h"
#include	"main.h"
#include	"enemy.h"

#define		APPEAR_MAX	128
#define		BEGIN		10000
#define		END		20000
#define		FILE_END	30000
#define		NUM_LEN		8

typedef	struct chart {
	int	time;
	char	mod_kind;
	char	mot_kind;
	char	sign_x;		/*	1 or -1		*/
	char	sign_y;		/*	1 or -1		*/
} CHART;

static CHART	*timelist;
static CHART	timelist_org[ STAGEMAX ][ APPEAR_MAX + 1 ];
static int	step;
int		total_time[ STAGEMAX ];

int	SearchNumTL(FILE *fp)
{
	int	input;
	char	numbuf[NUM_LEN];
	char	*numptr = numbuf;
	
	for (;;) {
		if ( EOF == (input = fgetc(fp)) || '~' == input ) return FILE_END;
		if ( '(' == input ) return BEGIN;
		if ( ')' == input ) return END;
		if ( '\\' == input ) {	/*	Skip comments	*/
			while ( '\\' != fgetc(fp) ) ;
			continue;
		}
		if ( ('-' != input) && ('0' > input || '9' < input) ) {
			if (numptr != numbuf) {
				*numptr = '\0';
				return atoi(numbuf);
			} else {
				continue;
			}
		}
		*numptr++ = input;
	}
}

void	ReadTimeList(CHART *timelist, int round_)
{
	int	no = 0, end_flag = 0;
	FILE	*fp;
	const char	*filename[] = {
		"timelist.st1", "timelist.st2", "timelist.st3", "timelist.st4",
		"timelist.st5", "timelist.st6", "timelist.st7", "timelist.tit", "timelist.end"
	};
	
	if ( NULL == (fp = fopen(filename[round_ - 1], "r")) ) {
		fprintf(stdout, "%s file open error\n", filename[round_ - 1]);
		exit(1);
	}
	printf("timechart data %s now reading ...\n", filename[round_ - 1]);
	total_time[round_ - 1] = 0;
	for (;;) {
		for (;;) {
			int	code = SearchNumTL(fp);
			
			if (BEGIN == code) break;
			if (FILE_END == code) {
				end_flag = 1;
				break;
			}
		}
		if (end_flag) break;
		if (no == APPEAR_MAX) {
			fputs("timelist over flow\n", stdout);
			exit(1);
		}
		timelist[no].time = ( total_time[round_ - 1] += SearchNumTL(fp) );
		timelist[no].mod_kind = (char)SearchNumTL(fp);
		timelist[no].mot_kind = (char)SearchNumTL(fp);
		timelist[no].sign_x = (char)SearchNumTL(fp);
		timelist[no].sign_y = (char)SearchNumTL(fp);
		++no;
		while (END != SearchNumTL(fp));	/*No particular need */
	}
	fclose(fp);
	printf("total stage time is %d	total appear list %d\n", total_time[round_ - 1], no);
	timelist[no].time = 65536;
}

void	InitTimeList(void)
{
	if (stages) {
		int	r;
		
		for (r = round_init; r <= STAGEMAX; ++r) ReadTimeList(timelist_org[r - 1], r);
	} else {
		ReadTimeList(timelist_org[round_init - 1], round_init);
	}
}

void	InitStageTimeList(void)
{
	step = 0;
	timelist = timelist_org[round_ - 1];
}

void	AppearCheck(void)
{
	int	model;
	
	while (timelist[step].time == round_counter) {
		BornEnemy( model = (int)timelist[step].mod_kind, (int)timelist[step].mot_kind,
			(int)timelist[step].sign_x, (int)timelist[step].sign_y,
			0,	/*flag = 0*/
			0, 0, 0, 0, 0, 0/*dummy*/ );
		++step;
		if (BOSS_MODEL == model) boss_appear_flag = 1;
	}
	switch (force_appear_from_setbox) {
		case	3:
			BornEnemy( 28, 21, 1, 1,
				1,	/*flag*/
				force_pos.x, force_pos.y, force_pos.z, 0, 0, 0);
			force_appear_from_setbox = 0;
			break;
		case	4:
			BornEnemy( 27, 26, 1, 1,
				1,	/*flag*/
				force_pos.x, force_pos.y, force_pos.z, 0, 0, 0);
			force_appear_from_setbox = 0;
			break;
		case	7:
			BornEnemy( 29, 19, 1, 1,
				1,	/*flag*/
				force_pos.x, force_pos.y, force_pos.z, 0, 0, 0);
			force_appear_from_setbox = 0;
			break;
	}
}
