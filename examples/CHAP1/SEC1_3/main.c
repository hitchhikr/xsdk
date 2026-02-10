#include <iocslib.h>

#define X_MAX	  256
#define X_MIN	  0
#define Y_MAX	  256
#define Y_MIN	  0

/* Current status */
#define TP_ALIVE  1
#define TP_DEAD   2

/* Moving speed */
#define VELO_X	  1
#define VELO_Y	  1

/* The structure of each data */
typedef struct {
	short flag;
	short x, y;
} SHIP;

typedef struct {
	short flag;
	short x, y;
	short vx, vy;
} SHOT;

/* Your aircraft data */
SHIP ship_data;

/* Number of self-shots */
#define SHOT_NUM  3

/* Self-fire data */
SHOT shot_data[ SHOT_NUM ];

#define KEY_LEFT  (1)
#define KEY_RIGHT (2)
#define KEY_UPPER (4)
#define KEY_LOWER (8)
#define KEY_A	  (16)

#define	TRUE	1
#define	FALSE	0

short get_key( void )
{
	static short repeat_flag = TRUE;
	short ret = 0;

	if ( !( JOYGET( 0 ) & 0x01 ) ) ret |= KEY_UPPER;
	if ( !( JOYGET( 0 ) & 0x02 ) ) ret |= KEY_LOWER;
	if ( !( JOYGET( 0 ) & 0x04 ) ) ret |= KEY_LEFT;
	if ( !( JOYGET( 0 ) & 0x08 ) ) ret |= KEY_RIGHT;
	if ( !( JOYGET( 0 ) & 0x20 ) ) { /* A button */
		if ( repeat_flag ) {
			ret |= KEY_A;
			repeat_flag = FALSE;
		}
	}else{
		repeat_flag = TRUE;
	}

	return ret;
}

void init( void )
{
	unsigned int j;

	/* Screen Settings */
	CRTMOD( 6 );
	G_CLR_ON();

	/* Erase the cursor */
	B_CUROFF();

	/* Initialize sprite related */
	SP_ON();
	BGCTRLST( 0, 0, 0 );
	BGCTRLST( 1, 1, 0 );

	/* Initialize the sprite registers */
	for( j = 0x80000000; j < 0x80000000 + 128; j ++ ) {
		SP_REGST( j, 0, 0, 0, 0 );
	}

	/* Set the initial settings for the object (your own aircraft, your own bullets) */
	ship_data.x = 128;
	ship_data.y = 128;
	ship_data.flag = TP_ALIVE;

	/* Initialize your own bullet settings */
	for( j = 0; j < SHOT_NUM; j ++ ) {
		shot_data[ j ].flag = TP_DEAD;
	}
}

void ship_move( void )
{
	short key;

	key = get_key();
	if ( ( key & KEY_UPPER ) && ( ship_data.y > Y_MIN ) ) { /* Up */
		ship_data.y -= VELO_Y;
	}
	if ( ( key & KEY_LOWER ) && ( ship_data.y < Y_MAX ) ) { /* Down */
		ship_data.y += VELO_Y;
	}
	if ( ( key & KEY_LEFT ) && ( ship_data.x > X_MIN ) ) { /* Left */
		ship_data.x -= VELO_X;
	}
	if ( ( key & KEY_RIGHT ) && ( ship_data.x < X_MAX ) ) { /* Right */
		ship_data.x += VELO_X;
	}

	/* Make a bullet */
	if ( key & KEY_A ) { /* A button */
		int i;

		for( i = 0; i < SHOT_NUM; i ++ ) {
			if ( shot_data[ i ].flag == TP_DEAD ) break;
		}
		if ( i < SHOT_NUM ) {
			shot_data[ i ].flag = TP_ALIVE;
			shot_data[ i ].x = ship_data.x;
			shot_data[ i ].y = ship_data.y;
		}
	}
}

void shot_move( void )
{
	int i;

	for( i = 0; i < SHOT_NUM; i ++ ) {
		if ( shot_data[ i ].flag == TP_DEAD ) continue;
		shot_data[ i ].x += VELO_X;
		if ( shot_data[ i ].x >= X_MAX ) {
			shot_data[ i ].flag = TP_DEAD;
		}
	}
}

void trans_sp( void )
{
	unsigned int j;
	int i;

	/* Display of your own aircraft */
	SP_REGST( 0, ship_data.x, ship_data.y, 0x0100, 2 );

	/* Display of own bullet */
	for( j = 0x80000001, i = 0; i < SHOT_NUM; i ++, j ++ ) {
		if ( shot_data[ i ].flag == TP_DEAD ) {
			SP_REGST( j, 0, 0, 0, 0 );
			continue;
		} else {
			SP_REGST( j, shot_data[ i ].x, 
					 shot_data[ i ].y, 0x0101, 2 );
		}
	}
}

void main( void )
{
	int i;

	init();

	while( ( JOYGET( 0 ) & 0x40 ) ) { /* B button */
		ship_move();
		shot_move();

		trans_sp();
	}
}

