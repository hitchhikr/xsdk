#define	BG_NULL		0x100

/*	prototype for bgput.c */

void	fade(int level);
void	bgputs(int page, int x, int y, int color, char *cp);
void	bgputn(int page, int x, int y, int color, int num, int keta);
void	bgputn0(int page, int x, int y, int color, int num, int keta);
void	disp_status( void );
void	timeinit( void );
void	disptime( void );
void	disp_status_init( void );
int	overtime(int t);
int	undertime(int t);
void send_picture( uword *src );
void send_picture1( void *cmd, void *col );
void load_pattern( void );
void	put_pattern3( int x, int y, int number );


#define	CHR_XSIZE	16
#define	CHR_YSIZE	16
#define	CHR_SIZE	(CHR_XSIZE * CHR_YSIZE)
#define	CHR_NUM		16
