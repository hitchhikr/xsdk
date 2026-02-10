/*	main.h : main routine header	*/

#define		STAGEMAX	9

#define	ON		1
#define	OFF		0
#define	OVER_TIME	64

#define	ESC_CHECK()	if(KeySns(0)&2)InitEnd()

extern int	debug, nosound, stages;
extern int	round_, round_init, round_counter;
extern int	gameover_flag, player_level_flag;
extern int	muteki;

void	InitEnd(void);
void	functions(void);
