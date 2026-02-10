void	disp_init();
void	disp_status( int pb , int x , int hinv , int yinv , int mode ) ;
void	disp_xy( int x, int y);
void	patternwrite( int hinv , int vinv ) ;
void	chrselect( char ch , int hinv , int vinv ) ;
void	paletset( int block ) ;
void	make_bgnum( void );
void	drawbg( int x ) ;
void	disp_copy_region( int x0 );
void	erase_copy_region( int x0 );

#define	MODE_COPY	1
#define	MODE_PUT	0

#define	SCRNUM	16		/* Number of screens */
#define	XMIN	0
#define	XMAX	( SCRNUM * 32 -1 )
#define	DISPLAY_XSIZE	22
#define	DISPLAY_YSIZE	32
#define	BG_YSIZE	32
#define	BG_X0	10
#define	BG_Y0	0

#define	PT_Y	11
#define	CP_Y	12

#define	BGBUFSIZE	( SCRNUM * 32 * 32 )

typedef struct	_region {
	int	x1 , y1 ;
	int	x2 , y2 ;
} region ;

extern unsigned char	bgnum[ 8 ][ 32 ];

