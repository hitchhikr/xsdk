#ifndef __STDMOMO_H
#define __STDMOMO_H

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<doslib.h>

/* Sprite transfer macro */
#define	my_spset( spno, x, y, code, prw )	\
	{	short	*tmp = ( short * )0xeb0000 + (spno) * 4; \
		*( tmp++ ) = ( x );		*( tmp++ ) = ( y ); \
		*( tmp++ ) = ( code );		*( tmp ) = ( prw ); }

#define	RGB_R( c )	((c)/ 64 & 31)
#define	RGB_G( c )	((c)/ 2048 & 31)
#define	RGB_B( c )	((c)/ 2 & 31)

#define	GRAMADR( x, y )	((uword *)0xc00000 + (y) * 512 + (x))
#define	GRAM( x, y )	(*GRAMADR( x, y ))
#define	BGADR(x,y)	((unsigned short *)(0xebe000+ (y) * 128 + (x) * 2))

/*		    typedefs    		*/

typedef	signed char	byte;
typedef	unsigned char	ubyte;
typedef	signed short	word;
typedef	unsigned short	uword;

#endif
