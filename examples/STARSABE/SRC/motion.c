/*	motion.c : enemy motion program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<doslib.h>
#include	<ctype.h>
#include	"main.h"
#include	"matrix.h"
#include	"enemy.h"
#include	"motion.h"
#include	"motlocal.h"

#define		MAXCOMM 11

const char *error[] = {
	"There is no \"begin\"!",
	"There is no \"end\"!",
	"The number of parameters is different.",
	"Grammar violation",
	"There are too many motion patterns.",
	"Not available here.",
	"There is too much motion data.",
	"The comment is strange"
};

enum	com_kind {
	CMD_COUNT,
	CMD_MROT,
	CMD_POS,
	CMD_VROT,
	CMD_SPEED,
	CMD_SHOT,
	CMD_BORN,
	CMD_END,
	CMD_BEGIN,
	CMD_LOOPEND,
	CMD_LOOPSTART
};

const char *command[MAXCOMM] = {
	"count",
	"mrot",
	"pos",
	"vrot",
	"speed",
	"shot",
	"born",
	"end",
	"begin",
	"loopend",
	"loopstart"
};

const char *filename[] = {
	"motion.st1", "motion.st2", "motion.st3", "motion.st4",
	"motion.st5", "motion.st6", "motion.st7", "motion.tit", "motion.end"
};


MOTION		**motionbuf;
MOTION		*motionbuf_org[ STAGEMAX ][ ENEMYKIND ];
LOOPTL		*loopbuf;
LOOPTL		loopbuf_org[ STAGEMAX ][ ENEMYKIND ];

static MOTION	*scan, *mot;
static int	enemyno, motionno, sum;

int	mfscanf(FILE *fp, char *buf)
{
	char	c;
	int	flag = 0;

	for (;;) {
		switch ( c = getc(fp) ) {
			case	EOF:
				*buf = '\0';
				return EOF;
			case	'\r':
                break;
			case	' ':
			case	'\t':
			case	'\n':
				*buf = '\0';
				if (flag) return 1;
				break;
			default:
				*buf++ = c;
				flag = 1;
				break;
		}
	}
}

int	ReadMotion(MOTION **motionbuf, LOOPTL *loopbuf, int round_)
{
	FILE	*dfp;
	char	buf[BFMAX];
	int	i, first = 1, flag = 0;
	
	enemyno = 0;	motionno = 0;	sum = 0;
	
	for (i = 0; i < ENEMYKIND; i++) {
		loopbuf[i].start = loopbuf[i].end = NULL;
		motionbuf[i] = NULL;
	}
	if ( NULL == (dfp = fopen(filename[round_ - 1], "r")) ) {
		printf("Can't read enemy data file %s\n", filename[round_ - 1]);
		return 1;
	}

	printf("motion data %s now reading ...\n", filename[round_ - 1]);
	if (( mot = (MOTION *)MALLOC(sizeof(MOTION)) ) >= (MOTION *)0x81000000) {
		printf("Out Of Memory.\n");
		return 1;
	}
	ClearMot();
	while ( SkipComm(dfp,buf) ) {
		if (strcmp(command[CMD_BEGIN],buf)) {
			PutError(1);
			return 1;
		}
		first = 1;	flag = 1;
		if (debug) printf("%d{", enemyno);
		while ( SkipComm(dfp,buf) ) {
			for ( i = 0 ; i < MAXCOMM ; i++ ) if (!strcmp(command[i],buf)) break;
			switch (i) {
				case CMD_END:				/* end */
					flag = 0;
					goto end;
				case CMD_VROT:				/* vrot */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d ", i);	*/
					if (i==0x80000000) return 3;
					mot->vec.x = (unsigned char)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d)", i);	*/
					if (i==0x80000000) return 3;
					mot->vec.y = (unsigned char)i;
					break;
				case CMD_MROT:				/* mrot */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d ", i);	*/
					if (i==0x80000000) return 3;
					mot->rot.x = (unsigned char)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d ", i);	*/
					if (i==0x80000000) return 3;
					mot->rot.y = (unsigned char)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d)", i);	*/
					if (i==0x80000000) return 3;
					mot->rot.z = (unsigned char)i;
					break;
				case CMD_SPEED:				/* speed */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->veloc = (int)i;
					break;
				case CMD_COUNT:				/* count */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->count = (short)i;
					if (i = WriteMotion(motionbuf)) return i;
				/*	if (debug) printf(" ; ");	*/
					first = 0;
					break;
				case CMD_POS:				/* pos */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d ", i);	*/
					if (i==0x80000000) return 3;
					mot->pos.x = (short)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d ", i);	*/
					if (i==0x80000000) return 3;
					mot->pos.y = (short)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%d)", i);	*/
					if (i==0x80000000) return 3;
					mot->pos.z = (short)i;
					break;
				case CMD_SHOT:				/* shot */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->shot.count = (int)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->shot.kind = (int)i;
					break;
				case CMD_BORN:				/* born */
				/*	if (debug) printf(" %s(",buf);	*/
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->born.count = (int)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->born.mod_k = (int)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->born.mot_k = (int)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->born.sign = (int)i;
					i = ReadNumber(dfp);
				/*	if (debug) printf("%u)", i);	*/
					if (i==0x80000000) return 3;
					mot->born.flag = (int)i;
					break;
				case CMD_LOOPSTART:				/* loopstart */
				/*	if (debug) printf(" %s(",buf);	*/
				/*	if (debug) printf("%d)", motionno - 1);	*/
					loopbuf[enemyno].start = scan;
					break;
				case CMD_LOOPEND:			/* loopend */
				/*	if (debug) printf(" %s(",buf);	*/
				/*	if (debug) printf("%d)", motionno - 1);	*/
					loopbuf[enemyno].end = scan;
					break;
				default:
					printf("\n\"%s\" ??",buf);
					PutError(4);
					return 4;
			}
		}
end:
		if ( flag ) {
			PutError(2);
			return 2;
		} else {
		/*	if (debug) printf("} ");	*/
			enemyno++;
			motionno = 0;
		}
	}
	fclose(dfp);
	MFREE(mot);
	printf ("motion sum = %d\n", sum);
	return 0;
}

void	InitMotion(void)
{
	if (stages) {
		int	r;

		for (r = round_init; r <= STAGEMAX; ++r) ReadMotion(motionbuf_org[r - 1], loopbuf_org[r - 1], r);
	} else {
		if (ReadMotion(motionbuf_org[round_init - 1], loopbuf_org[round_init - 1], round_init)) exit(1);
	}
}

void	InitStageMotion(void)
{
	motionbuf = motionbuf_org[round_ - 1];
	loopbuf = loopbuf_org[round_ - 1];
}

void	PutError(int errno)
{
	printf("\n*** Error : %s\n",error[errno-1]);
}

int	WriteMotion(MOTION **motionbuf)
{
	if (ENEMYKIND == enemyno) {
		PutError(5);
		return 5;
	}
	if (0 == motionno) {
		scan = motionbuf[enemyno] = mot;
	} else {
		scan = (scan->n = mot);
	}
	if (( mot = (MOTION *)MALLOC(sizeof(MOTION)) ) >= (MOTION *)0x81000000) {
		printf("Out Of Memory.\n");
		return 1;
	}
	ClearMot();
	motionno++;
	sum++;
	return 0;
}

int	SkipComm(FILE *fp , char *buf)
{
	int	commlevel = 0;

	while ( 1 ) {
		if ( EOF == mfscanf(fp ,buf) ) {
    		return 0;
		} else if ( '/' == *buf && '*' == *(buf + 1) ) {
			++commlevel;
		} else if ( '*' == *buf && '/' == *(buf + 1) ) {
			if ( 0 > --commlevel ) {
				PutError(8);
				return 8;
			}
		} else if ( 0 == commlevel ) {
    		return 1;
		}
	}
}

void	ClearMot(void)
{
	int	i;
	char	*tmp = (char *)mot;

	for ( i = 0 ; i < sizeof(MOTION) ; i++ ) *tmp++ = 0;
}

int	scanfnum(char *buf, int *num)
{
	char	c;
	int	flag = 0, sign = 1;

	for (;;) {
			switch (c = *buf++) {
			case	'\0':
				if (flag) {
					if ( -1 ==sign ) *num *= -1;
					return 1;
				}
                else
                {
                    return 0;
                }
			default:
				if ('-' == c) {
					if (!flag) sign = -1;
					else
                    {
                        return 0;
                    }
				} else if ('0' <= c && c <= '9') {
					*num *= 10;
					*num += c - '0';
				}
				flag = 1;
				break;
		}
	}
}

int	ReadNumber(FILE *fp)
{
	int	tmp = 0;
	char	buf[BFMAX];

	if ( SkipComm(fp,buf) )
		if (!scanfnum(buf, &tmp)) {
			PutError(3);
			return 0x80000000;
		};
	return tmp;
}
