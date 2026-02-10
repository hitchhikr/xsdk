
/*	Floor-related	*/
#define	XSIZE	18
#define	YSIZE	18
#define	YX(x)	(((x) + 384) / (DIVX * 16))
#define	YY(y)	(((y) + 384) / (DIVX * 16))
#define	STAGENUM	32

typedef struct	_stage {
	char	yuka[ YSIZE ][ XSIZE ];
	short	x0, y0;
	short	x1, y1;
} stage;

extern	stage	stagedat[STAGENUM];
void	initstage(void);
void	background( int backno, int sno );
void	bginit( void );
void	bgdisp( int no );
int	yukadata( int x, int y );
void	adjust_initial_location( void );


