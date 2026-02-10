/*	calc.h : calculation header	*/

#define		SHOT_NUM	3
#define		SHIELD_MAX	128

typedef struct	medat {
	int	x;
	int	y;
	int	dx;
	int	dy;
	int	crash;
	int	shield;
} MEDAT;

typedef struct	shotdat {
	int	stat;	/*	0 - not exist	1 - flying	*/
	int	x;
	int	y;
	int	z;
	int	dx;
	int	dy;
	int	dz;
} SHOTDAT;

extern MEDAT	me;
extern SHOTDAT	shot[];
extern int	score, hi_score, no_damage_clear, hi_no_damage_clear;
extern char	hiname[];
extern int	range_u, range_r, range_d, range_l;
extern int	clear_flag, ending_flag;

void	InitStageCalc(void);
void	InitCalc(int);
void	Calc(int);
