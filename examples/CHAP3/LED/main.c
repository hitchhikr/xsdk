/*
 *		A sample program that synchronizes LEDs with music and blinks
 *							programmed by chika@kmc
 */

#include	<iocslib.h>
#include	"zlib.h"

short	*seq_tbl;			/* Z-MUSIC work table start address */

/*	Function to turn on/off the LED	*/

void	led_control( void )
{
	int i;
	unsigned char j = 0;

	/* Track number for each LED: From left to right: 
       Full-width, Hiragana, INS, CAPS, Code input, Roman letters, Kana */
	static int trno[ 7 ] = { 6, 5, 0, 3, 4, 1, 2 };

	/* Look at the work and decide whether to turn on or off each LED.
       If the remaining gate time is 3 or more and there is no rest, it will turn on. */

	for( i = 0; i < 7; i++ ) {
		j *= 2;
		if( *( seq_tbl + 1 + 128 * trno[ i ] ) >= 3 &&
			!(*((unsigned char *)seq_tbl + 0x42 + 256 * trno[ i ] )
			& 0x80 ) ) {
		} else {
				j++;
		}
	}

	/* Send data to the keyboard */

	*(volatile unsigned char *)0xe8802f = j | 0x80;
	*(volatile unsigned char *)0xe8802d = 0x05;
}

/*
 *		Main Routine
 */

void main( void )
{
dummy:	B_SUPER( 0 );				/* Switch to B_SUPER */

	seq_tbl = z_get_wk_tbl();		/* Get the start address of the work */

	while( !(BITSNS( 0 ) & 2) ){		/* Repeat until ESC is pressed */
		led_control();
	}
	LEDMOD( 2, 0 );				/* Turn the LED back on */
}
