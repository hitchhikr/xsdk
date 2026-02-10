/***************************************

			 m a i n . c

*****************************************/

#include <stdlib.h>
#include <iocslib.h>

#define X_MAX	  256
#define X_MIN	  0
#define Y_MAX	  256
#define Y_MIN	  0

#define DATA_SIZE 2
#define DATA_X	  0
#define DATA_Y	  1

#define VELO_X	  1
#define VELO_Y	  1

/* Your aircraft data */
short ship_data[ DATA_SIZE ];

void main( void )
{
		unsigned int j;

		/* Screen Settings */
		CRTMOD( 6 );
		G_CLR_ON();

		/* Erase the cursor */
		B_CUROFF();

		/* Sprites related initializations */
		SP_ON();
		BGCTRLST( 0, 0, 0 );
		BGCTRLST( 1, 1, 0 );

		/* Initialize the sprite registers */
		for( j = 0x80000000; j < 0x80000000 + 128; j ++ ) {
				SP_REGST( j, 0, 0, 0, 0 );
		}

		/* Initial settings for your own aircraft, etc. */
		ship_data[ DATA_X ] = 128;
		ship_data[ DATA_Y ] = 128;

		while( 1 ) {
				/* Move your own aircraft */
				if ( !( JOYGET( 0 ) & 0x01 )
					&& ( ship_data[ DATA_Y ] > Y_MIN ) ) { /* Up */
						ship_data[ DATA_Y ] -= VELO_Y;
				}
				if ( !( JOYGET( 0 ) & 0x02 )
					&& ( ship_data[ DATA_Y ] < Y_MAX ) ) { /* Down */
						ship_data[ DATA_Y ] += VELO_Y;
				}
				if ( !( JOYGET( 0 ) & 0x04 )
					&& ( ship_data[ DATA_X ] > X_MIN ) ) { /* Left */
						ship_data[ DATA_X ] -= VELO_X;
				}
				if ( !( JOYGET( 0 ) & 0x08 )
					&& ( ship_data[ DATA_X ] < X_MAX ) ) { /* Right */
						ship_data[ DATA_X ] += VELO_X;
				}

				/* For emergency escape */
				if ( !( JOYGET( 0 ) & 0x40 ) ) { /* B button */
						/* End */
						exit( 0 );
				}

				/* Draw your own ship as a sprite */
				/* There are 5 arguments, PRW is 0 to 2 */
				SP_REGST( 0, ship_data[ DATA_X ], ship_data[ DATA_Y ], 0x0100, 2 );
		}
}
