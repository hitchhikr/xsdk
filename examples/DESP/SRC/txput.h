/*
 *		txput.h
 *
 */

void	tx_init( char *filename ) ;
void	tx_putc( short x , short y , short s , char color) ;
void	tx_puts( short x , short y , char s[] , char color ) ;
void	tx_putn( short x , short y , int n , short i , char color ) ;
void    tx_putn2(short x, short y, int n, short i, char color);
void	tx_puts2( short x , short y , char s[] , char color ) ;

char	*s_num( char *c , int n , short i ) ;

void	tx_clr1() ;
void	tx_clr2() ;
void	txclear(void);
