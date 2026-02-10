/*	star.h : star scroll header	*/

#define		STAR_NUM	32

typedef	struct stardat {
	short	x;
	short	y;
	short	z;
	short	rx;
	short	ry;
	short	old_rx;
	short	old_ry;
	int	box;
	int	old_box;
} STARDAT;

extern STARDAT	star[];
extern int	bokan;

void	InitStar(void);
void	MoveStar(void);
