/*	motlocal.h : enemy motion local header	*/

#define	BFMAX		1024
#define	ENEMYKIND	36
#define	ABS(x)		(0<(x)?(x):-(x))

int	SetMotion(char *);
int	WriteMotion(MOTION **);
void	PutError(int);
int	SkipComm(FILE *,char *);
void	ClearMot(void);
int	ReadNumber(FILE *);

typedef struct	looptl {
	MOTION	*start;
	MOTION	*end;
} LOOPTL;

extern MOTION		**motionbuf;
extern LOOPTL		*loopbuf;
