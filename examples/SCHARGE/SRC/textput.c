/*
 *	text vram draw module for NKF super Battle
 */

#include	"stdmomo.h"
#include	"textput.h"

#define	PALET(i)	(*(unsigned short *)(0xe82200 + (i)*2))
#define	PALETNUM	16
#define	CHRNUM		1024
#define	CHRSIZE		32

static	unsigned char	textbuf[ PALETNUM * 2 + CHRNUM * CHRSIZE ];

#define	tr(i)	(*(adr + (i) * 128) = *src++)
static	void	putter( int x, int y, unsigned char *src )
{
	char	*adr;
	adr = (char *)0xe00000 + y * 1024 + x;
	tr(0); tr(1); tr(2); tr(3); tr(4); tr(5); tr(6); tr(7);
	adr += 0x20000;
	tr(0); tr(1); tr(2); tr(3); tr(4); tr(5); tr(6); tr(7);
	adr += 0x20000;
	tr(0); tr(1); tr(2); tr(3); tr(4); tr(5); tr(6); tr(7);
	adr += 0x20000;
	tr(0); tr(1); tr(2); tr(3); tr(4); tr(5); tr(6); tr(7);
}

/*
 *		Text Initialization
 */

void	textinit( char *filename )
{
	FILE	*fp;
	int	i;
	fp = fopen( filename, "rb" );
	if (!fp) {
		printf( "not found %s(text data file).\n", filename );
		exit( 1 );
	}
	fread( textbuf, 1, PALETNUM * 2 + CHRNUM * CHRSIZE, fp );
	fclose(fp);
	for( i = 0; i < PALETNUM; i++ ){
		PALET(i) = *((unsigned short *)textbuf + i);
	}
}

/*		Single character display		*/
void tx_putc( int x, int y, int chrnum )
{
	putter( x, y, textbuf + 32 + chrnum * 32 );
}

/*		Single character display (big)		*/
void tx_putc_big( int x, int y, int chrnum )
{
	putter( x    , y    , textbuf +  32 + chrnum * 128 );
	putter( x    , y + 1, textbuf +  64 + chrnum * 128 );
	putter( x + 1, y    , textbuf +  96 + chrnum * 128 );
	putter( x + 1, y + 1, textbuf + 128 + chrnum * 128 );
}

/*		String display		*/
void tx_puts( int x, int y, char *s, int basenum)
{
	while( *s != '\0' ) {
		tx_putc( x++, y, basenum + *s - 0x20);
		s++;
	}
}

/*		String display (big)		*/
void tx_puts_big( int x, int y, char *s, int basenum)
{
	while( *s != '\0' ) {
		tx_putc_big( x, y, basenum + *s - 0x20);
		s++;
		x += 2;
	}
}

/*		Numerical display		*/
void tx_putn( int x, int y, int keta, int num, int basenum)
{
	if (num < 0) num = 0;
	keta--;
	do {
		tx_putc(x + keta, y, (num % 10) + 0x10 + basenum);
		num /= 10;
	} while( (num > 0) && (--keta >= 0) );
	while(keta-- > 0) {
		tx_putc(x + keta, y, basenum);
	}
}

/*		Numerical display		*/
void tx_putn0( int x, int y, int keta, int num, int basenum)
{
	if (num < 0) num = 0;
	keta--;
	do {
		tx_putc(x + keta, y, (num % 10) + 0x10 + basenum);
		num /= 10;
	} while( (--keta >= 0) );
}

/*		Numerical display		*/
void tx_putn_big( int x, int y, int keta, int num, int basenum)
{
	if (num < 0) num = 0;
	keta--;
	do {
		tx_putc_big(x + keta*2, y, (num % 10) + 0x10 + basenum);
		num /= 10;
	} while( (num > 0) && (--keta >= 0) );
	while(keta-- > 0) {
		tx_putc_big(x + keta*2, y, basenum);
	}
}

void	textclear( void )
{
	int	i;
	long	*adr1 = (long *)0xe00000;
	long	*adr2 = (long *)0xe20000;
	long	*adr3 = (long *)0xe40000;
	long	*adr4 = (long *)0xe60000;
	for( i = 0; i < 32768; i++ ){
		*adr1++ = 0;
		*adr2++ = 0;
		*adr3++ = 0;
		*adr4++ = 0;
	}
}
