/*
 *		stick.c
 *
 *				Programmed by Momo     1993
 */

#include	"stdmomo.h"
#include	"stick.h"

STICK	JOY( int mode )
{
	short	b1, b2, b3;
	STICK	s;

	s.bin = ~JOYGET(mode);
	if (mode == 0) {
		b1 = BITSNS(8);
		b2 = BITSNS(9);
		b3 = BITSNS(0x0e);

		if (b1 &  16) s.bin |= 1;
		if (b1 & 128) s.bin |= 4;
		if (b2 &   2) s.bin |= 8;
		if (b2 &  16) s.bin |= 2;
		if (b3 &   8) s.bin |= 32;
		if (b3 &   4) s.bin |= 64;
	} else {
		b3 = BITSNS(0x0e);

		if (BITSNS(2) &   2) s.bin |= 1;	//w
		if (BITSNS(5) &   8) s.bin |= 2;	//x
		if (BITSNS(3) &  64) s.bin |= 4;	//a
		if (BITSNS(4) &   1) s.bin |= 8;	//d
		if (b3 &   2) s.bin |= 32;
		if (b3 &   1) s.bin |= 64;
	}
	return s;
}
