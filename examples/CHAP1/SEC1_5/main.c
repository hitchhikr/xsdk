/*****************************
		  m a i n . c
*****************************/

#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>
#include "main.h"
#include "ship.h"
#include "enemy.h"
#include "atari.h"

void init( void )
{
	int j;

	/* Screen Settings */
	CRTMOD( 6 );
	G_CLR_ON();

	/* Erase the cursor */
	B_CUROFF();

	/* Sprites related initializations */
	SP_ON();
	BGCTRLST( 0, 0, 0 );
	BGCTRLST( 1, 1, 0 );

	/* Sprite-related initialization */
	sp_init();

	/* Other initialization */
	ship_init();
	shot_init();
	enemy_init();
	bomb_init();
}

void main( void )
{
	init();

	while( 1 ) {
		ship_move();
		shot_move();
		enemy_move();
		bomb_move();
		atari();

		sp_trans();

		if( !( JOYGET( 0 ) & 0x40 ) ) { 		/* B button */
			exit( 0 );
		}
	}
}
