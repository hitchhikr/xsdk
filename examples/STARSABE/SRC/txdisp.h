/*	txdisp.h : text header	*/

#define	PCG		(volatile unsigned short *)0xeb8000
#define	PAL		(volatile unsigned short *)0xe82220
#define	SPCTL		(volatile unsigned short *)0xeb0000
#define	PRW		3
#define	SPALET_		(volatile unsigned short *)0xe82200
#define	BGSC_X		(volatile unsigned short *)0xeb0800
#define	BGSC_Y		(volatile unsigned short *)0xeb0802
#define	BGDT(c)		(volatile unsigned short *)(0xebe000 + (c << 1))
#define	RGB(r, g, b)	((r) * 0x40 + (g) * 0x800 + (b) * 2)

#define	TARGET_PAL	(PAL + 4 * 16 + 2)

#define	DEFAULT_X		128
#define	DEFAULT_Y		128
#define	ScrollText(x,y)		(*(volatile unsigned short *)0xe80014)=(DEFAULT_X+(x));(*(volatile unsigned short *)0xe80016)=(DEFAULT_Y+(y))

void	DamageColor(int);
void	PutCha(int, int, int);
void	PutString(int, int, char *);
void	InitTx(void);
void	InitDispTx(void);
int	TxDisp(void);
int	GameOver(void);
void	GameEnd(void);
void	EndingMessage(void);
