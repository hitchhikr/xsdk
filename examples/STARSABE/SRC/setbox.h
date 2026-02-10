/*	setbox.c : prepare display program	*/

typedef struct	boxbuf {
	struct	boxbuf	*n;
	short	x;
	short	y;
	short	z;
	int	box;
	short	rx;
	short	ry;
	int	pat;
} BOXBUF;

void	DebugCount(void);
void	InitSetBox(void);
void	SetBox(void);
void	ReturnBox(void);
