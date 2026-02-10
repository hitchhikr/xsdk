/*
 *		bg.c( BG character output rountine )
 *
 *				Programmed by chika1994
 */

#include	"stdmomo.h"
#include	"battle.h"
#include	"bg.h"
#include	"textput.h"
#include	"display.h"
#include	"etc.h"
int	trans1( void *cmdp , void *colp , volatile void *dstp );

static	uword	picture[ 256 * 256 ];
static	uword	patdata[ CHR_SIZE * CHR_NUM ];
#define	X0 0
#define	Y0 0

char	*patdata_filename = "bg.pic";
void get_pattern( int x, int y, int number )
{
	int	i, j;
	uword	*src, *dst;

	x *= CHR_XSIZE;
	y *= CHR_YSIZE;
	dst = &patdata[ number * CHR_SIZE ];

	for( i = y; i < y + CHR_YSIZE ; i++ ){
		src = GRAMADR( x, i );
		for( j = 0; j < CHR_XSIZE ; j++ ){
			*(dst++) = *(src++);
		}
	}
}


void picload( char *fname, uword *addr, int flg )
{
	FILE	*fp;
	uword	*picdata, *pic;
	ubyte	tmp = 0;
	long	filesize, col, len, x = 0, y = 0;

	fp = fopen( fname, "rb" );
	if( !fp ) error( "picfile not found!" );

	filesize = FILELENGTH(fp);
	if ((picdata = (uword *)MALLOC(filesize)) >= (uword *)0x81000000) {
		error("memory cannot allocated(in function picload).");
	}
	while (tmp != 0x1a) {
		fread( &tmp, 1, 1, fp );
		filesize--;
	}
	fread(picdata, 2, filesize / 2, fp);
	fclose(fp);
	pic = picdata + 2;

	while (y <= 255) {
		col = *pic++;	len = *pic++;
		while (len) {
			*(addr++) = col;
			len--, x++ ;
			if (x > 255) {
				y++, x = 0;
				if( flg ) addr += 256;
			}
		}
	}
	MFREE(picdata);
}


void picextract( uword *pic, uword *addr, int flg )
{
	long	col, len, x = 0, y = 0;

	while (y <= 255) {
		col = *pic++;	len = *pic++;
		while (len) {
			*(addr++) = col;
			len--, x++ ;
			if (x > 255) {
				y++, x = 0;
				if( flg ) addr += 256;
			}
		}
	}
}

void buffer_to_vram( void )
{
	int x, y;
	uword	*src, *dst;
	dst = GRAMADR(0, 0);
	src = picture;
	for( y = 0; y < 256; y++ ){
		for( x = 0; x < 256; x++ )
			*(dst++) = *(src++);
		dst += 256;
	}
}

void vram_to_buffer( void )
{
	int x, y;
	uword	*src, *dst;
	dst = GRAMADR(0, 0);
	src = picture;
	for( y = 0; y < 256; y++ ){
		for( x = 0; x < 256; x++ )
			*(src++) = *(dst++);
		dst += 256;
	}
}

void picload_to_vram( char *fname )
{
	picload( fname, GRAMADR(0, 0), 1 );
}
void picload_to_buffer( char *fname, uword *addr)
{
	picload( fname, addr, 0 );
}
void load_pattern( void )
{
	int	i;
	picload_to_vram( patdata_filename );
	get_pattern( 0, 0, 0 );
	get_pattern( 1, 0, 1 );
}
void load_picture( char *fname )
{
	picload_to_buffer( fname, picture );
	buffer_to_vram();
}

void send_picture( uword *src )
{
	picextract( src, picture, 0 );
	buffer_to_vram();
}
void send_picture1( void *cmd, void *col )
{
	trans1(cmd, col, (volatile void *)0xc00000);
	vram_to_buffer();
}

/*
 *		put pattern
 */

void put_pattern( int x, int y, int number )
{
	int	i, j;
	uword	*src, *dst;

	x = x * CHR_XSIZE + X0;
	y = y * CHR_YSIZE + Y0;
	src = &patdata[ number * CHR_SIZE ];

	for( i = y; i < y + CHR_YSIZE; i++ ){
		dst = GRAMADR( x, i );
		for( j = 0; j < CHR_XSIZE; j++ ){
			*(dst++) = *(src++);
		}
	}
}


#define	Z4	0x404
#define	Z3	0x303
#define	Z2	0x202
#define	Z1	0x101
#define	M1	-0x101
#define	M2	-0x202
#define	Z0	0x000
const	int	zure[ CHR_YSIZE ][ CHR_XSIZE ] = {
	{Z4,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z3,Z2},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z3,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,Z2,M1},
	{Z2,M1,M1,M1,M1,M1,M1,M1,M1,M1,M1,M1,M1,M1,M1,M2}
};

void	put_pattern3( int x, int y, int number )
{
	int	i, j;
/*	uword	c, r, g, b;	*/
	uword	*src1, *src2, *dst;

	x = x * CHR_XSIZE + X0;
	y = y * CHR_YSIZE + Y0;
	src1 = &patdata[ number * CHR_SIZE ];
	for( i = 0; i < CHR_YSIZE; i++ ){
		dst = GRAMADR( x, i+y );
		src2 = picture + (i + y) * 256 + x;
		for( j = 0; j < CHR_XSIZE; j++ ){
/*			*dst = (*src1 & 0xf7bc)/2 + (*(src2+zure[i][j]) & 0xf7bc)/2 ;	*/
                        *dst = *src1 + (*(src2+zure[i][j]) & 0xf7bc)/2 ;
/*			c = *(src2 + zure[i][j]);
			r = RGB_R( *src1 ) + RGB_R(c) * 5 / 8;
			g = RGB_G( *src1 ) + RGB_G(c) * 5 / 8;
			b = RGB_B( *src1 ) + RGB_B(c) * 5 / 8;
			if( r > 31 ) r = 31;
			if( g > 31 ) g = 31;
			if( b > 31 ) b = 31;
			*dst = b * 2 + r * 64 + g * 2048;
*/
			src1++, src2++, dst++;
		}
	}
}




/*
 *		Charge meter display
 */

static	void	disp_chargegraph( int x, int y, int n, int j )
{
	int	i, palno = 0x100;
	if( n > nowcnf->charge_to_sword ) palno = 0x400;
	if( n > nowcnf->charge_to_laser ) palno = 0x300;
	n /= 4;		/* Actual number of dots to display */
	my_spset(j++, (x++) * 8 + 16, y * 8 + 16, 0xb5 + palno, 3);
	for( i = 0; i < n / 8; i++ ) {
		my_spset(j++, (x++) * 8 + 16, y * 8 + 16, 0xbf + palno, 3);
	}
	if( n % 8 ) {
		my_spset(j++, (x++) * 8 + 16, y * 8 + 16, 0xb7 + n % 8 + palno, 3);
		i++;
	}
	for( ; i < 6; i++ ) {
		my_spset(j++, (x++) * 8 + 16, y * 8 + 16, 0xb7 + palno, 3);
	}
	my_spset(j++, (x++) * 8 + 16, y * 8+16, 0xb6 + palno, 3);
}


/*
 *	Display of scores etc.
 */

void	disp_status( void )
{
	man	*mp = mandat;
	disptime();
	tx_putn(11,  0, 3, mp->energy , BASENUM);
	disp_chargegraph(7, 1, mp->charge, 0);
	mp++;
	tx_putn(28, 30, 3, mp->energy , BASENUM);
	disp_chargegraph(24, 31, mp->charge, 8);
}

void	disp_status_init( void )
{
	man	*mp = mandat;
	tx_puts(  1, 0, "ENERGY", BASENUM);
	tx_puts(  1, 1, "CHARGE", BASENUM);
	tx_puts( 18,30, "ENERGY", BASENUM);
	tx_puts( 18,31, "CHARGE", BASENUM);
	disp_status();
	tx_putc(  0, 0, '[' + BASENUM - 0x20 + ( mp->life >= 2 ) );
	tx_putc(  0, 1, '[' + BASENUM - 0x20 + ( mp->life >= 1 ) );
	mp++;
	tx_putc( 17,30, ']' + BASENUM - 0x20 + ( mp->life >= 2 ) );
	tx_putc( 17,31, ']' + BASENUM - 0x20 + ( mp->life >= 1 ) );
}

/*
 *	Fade in/out (32..not transparent --- 0..transparent) 
 */

void	fade(int level)
{
	int	x, y;
	unsigned short	c;

	c = 0x17f - level;
	for( y = 0; y < 32; y++ ){
		for( x = 0; x < 32; x++ ){
			*(unsigned short *)(0xebe000+ y * 128 + x * 2) = c;
		}
	}
}

#define	TIMEDIV	60
static	int	timeunder[ TIMEDIV ];

void	timeinit( void )
{
	int	i;
	for( i = 0; i < TIMEDIV; i++ ){
		timeunder[ i ] = i * 100 / TIMEDIV;
	}
}

#define	TIMER_X	26
#define	TIMER_Y	0

void	disptime( void )
{
	extern	int	timer;
	tx_putn_big(TIMER_X, TIMER_Y, 2, timer / TIMEDIV , BIG_BASENUM);
	tx_putn0(TIMER_X+4, TIMER_Y+1, 2, timeunder[timer % TIMEDIV], BASENUM);
}

int	overtime(int t)   {	return	t / TIMEDIV;		}
int	undertime(int t)  {	return	timeunder[t % TIMEDIV];	}

