/*
 *		textput.h
 *				Programmed by Naoya Kawahara 1994
 */

#define	BIG_BASENUM	0x20
#define	BASENUM		0
#define	textscroll(x,y)	{*(volatile short *)0xe80014 = (x); \
			 *(volatile short *)0xe80016 = (y); }

void textinit(char *filename);
void tx_putc(int x, int y, int charnum);
void tx_puts(int x, int y, char *s, int basenum);
void tx_putn0(int x, int y, int keta, int num, int basenum);
void tx_putn(int x, int y, int keta, int num, int basenum);
void tx_putc_big(int x, int y, int charnum);
void tx_puts_big(int x, int y, char *s, int basenum);
void tx_putn_big(int x, int y, int keta, int num, int basenum);
void textclear( void );
