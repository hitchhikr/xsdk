#define	BG_NULL		0x100

/*	prototype for bgscr.c */

int	bgread( int stage );
void	bgscroll( void );
int	bgatari( int x, int y );
void	bgbreak( int x, int y );

/*	prototype for bgput.c */

void	bgputs(int page, int x, int y, int color, char *cp);
void	bgputn(int page, int x, int y, int color, int num, int keta);
void	bgputn0(int page, int x, int y, int color, int num, int keta);
void	score_disp(int score , int left);

