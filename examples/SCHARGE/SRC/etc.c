/*
 *		etc.c
 */

#include	"stdmomo.h"
#include	"battle.h"
#include	"textput.h"
#include	"etc.h"

static	int	counter = 0;
short	_sin[ 8 ] = { 0, 184, 256, 184, 0, -184, -256, -184 };

void	error( char *mes )
{
	puts( mes );
	exit( 1 );
}

void	endbattle( void )
{
	textclear();
	KFLUSHIO( 0xff );
	CRTMOD( 16 );
	B_CURON();
	C_CURON();
	exit( 0 );
}


void	wait( void )
{
	counter++;
	if (!(BITSNS(0) & 4))	BGSCRLST( 0, 0, 0);
	if ((BITSNS(0) & 2))	endbattle();
/*	if( counter & 1 )	*(short *)0xe82202 = 1;
	else			*(short *)0xe82202 = 48;
*/
}


