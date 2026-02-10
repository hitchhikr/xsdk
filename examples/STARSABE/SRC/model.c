/*	model.c : set model program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<doslib.h>
#include	"model.h"
#include	"main.h"

#define		MODEL_MAX	32
#define		BEGIN		10000
#define		END		20000
#define		FILE_END	30000
#define		NUM_LEN		8

MODELTL		*model;
MODELTL		model_org[ STAGEMAX ][ MODEL_MAX ];

int	SearchNumMD(FILE *fp)
{
	int	input;
	char	numbuf[NUM_LEN];
	char	*numptr = numbuf;
	
	for (;;) {
		if ( EOF == (input = fgetc(fp)) ) return FILE_END;
		if ( '{' == input ) return BEGIN;
		if ( '}' == input ) return END;
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

void	ReadModel(MODELTL *model, int round_)
{
	int		no = 0, end_flg = 0;
	BOXMODEL	*boxmodel_ptr;
	FILE	*fp;
	const char	*filename[] = {
		"model.st1", "model.st2", "model.st3", "model.st4",
		"model.st5", "model.st6", "model.st7", "model.tit", "model.end"
	};

	if ( NULL == (fp = fopen(filename[round_ - 1], "r")) ) {
		fprintf(stdout, "%s file open error\n", filename[round_ - 1]);
		exit(1);
	}
	printf("model data %s now reading ...\n", filename[round_ - 1]);
	for (;;) {	/*	Chain Array Loop	*/
		int	per = 0;
		int	code;
		for (;;) {
			code = SearchNumMD(fp);
			
			if ( BEGIN == code ) break;
			if ( FILE_END == code ) {
				end_flg = 1;
				break;
			}
		}
		if (end_flg) break;

		if (no == MODEL_MAX) {
			fputs("model patterns over flow\n", stdout);
			exit(1);
		}
		per = SearchNumMD(fp);			/*	Magnification	*/
		model[no].pat = SearchNumMD(fp);	/*	Sprite Pattern	*/
		model[no].hit_point = SearchNumMD(fp);	/*	Hit Points	*/
		model[no].score = SearchNumMD(fp);	/*	Score */
		
		if ( (boxmodel_ptr = (BOXMODEL *)MALLOC(sizeof(BOXMODEL))) >= (BOXMODEL *)0x81000000 ) {
			fputs("boxmodel malloc out of memory\n", stderr);
			exit(1);
		}
		model[no++].chain_boxmodel = boxmodel_ptr;
		
		boxmodel_ptr->x = SearchNumMD(fp) * per / 100;
		boxmodel_ptr->y = SearchNumMD(fp) * per / 100;
		boxmodel_ptr->z = SearchNumMD(fp) * per / 100;
		boxmodel_ptr->col = SearchNumMD(fp);
		
		for (;;) {	/*	Loops in a chain */
			int	head = SearchNumMD(fp);
			
 			if ( END == head ) break;
			if (NULL == ( boxmodel_ptr->n = (BOXMODEL *)malloc(sizeof(BOXMODEL)) )) {
				fputs("boxmodel malloc out of memory\n", stderr);
				exit(1);
			}
			boxmodel_ptr = boxmodel_ptr->n;
			boxmodel_ptr->x = head * per / 100;
			boxmodel_ptr->y = SearchNumMD(fp) * per / 100;
			boxmodel_ptr->z = SearchNumMD(fp) * per / 100;
			boxmodel_ptr->col = SearchNumMD(fp);
		}
		boxmodel_ptr->n = NULL;
	}
	fclose(fp);
}

void	InitModel(void)
{
	if (ON == stages) {
		int	r;
		
		for (r = round_init; r <= STAGEMAX; ++r) ReadModel(model_org[r - 1], r);
	} else {
		ReadModel(model_org[round_init - 1], round_init);
	}
}

void	InitStageModel(void)
{
	model = model_org[round_ - 1];
}
