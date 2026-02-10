#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>
#include <doslib.h>
#include "sub.h"
#include "ship.h"
#include "enemy.h"
#include "event.h"
#include "atari.h"
#include "bg.h"
#include "graphic.h"
#include "etc.h"
#include "sound.h"

int count = 0 ;		/* Time since the start of the face */
int score = 0 ;		/* Score */
extern SHIP ship_data;	/* Your aircraft data */

void main( void )
{
	int game_over, stage_clear, stage;

DUMMY:	B_SUPER(0);						/* << */

    srand(0);
	sd_init();

	for(;;){
		score = 0; ship_data.left = 3;
		stage = 0;
		game_over = 0;

		init();
		title();

		init();
		graphic_init();

		while( !game_over ) {

			/* Initialization at the start of a stage */

			init();
			graphic_init();

			ship_init();
			enemy_init( stage );
			if ( bgread( stage ) ) {
				printf( "Is there no BG file?\n" );
				ende();
			}
			pcg_read(stage);
			count = 0 ;
			stage_clear = 0;
			stage_start( stage );

			/* Main Loop */

			while (!game_over && stage_clear < 120) {
				if (BITSNS( 0 ) & 2) pause();		/* Esc to pause */
				if (BITSNS( 12 ) & 2) ende();		/* End with break */
				if (!(BITSNS( 0 ) & 4)) wait();		/* Wait for vertical blanking */
#if 1
				else ship_data.dead_count = -120 ;
#endif
				bgscroll();
				trans_sprite_data();			/* Sprite drawing */
				score_disp( score, ship_data.left );	/* Score display */
				event(stage, count);			/* Event */

				if (ship_move()) game_over = 1;		/* Movement of your own aircraft */
				shot_move();				/* Movement of your own bullets */
				enemy_move();				/* Enemy aircraft movement */
				bomb_move();				/* Enemy bullet movement */
				if (stage_clear) stage_clear++;
				if (atari()) stage_clear++;

				count++ ;
			}
			/* Processing at the end of the main loop */
			if (!game_over) stage_end(stage++); else disp_gameover();
		}
	}
	ende();
}
