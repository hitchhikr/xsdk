
#ifndef _SUB_H_
#define _SUB_H_

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* Counts per dot */
#define DOT_RATE 16

#define FLASH_PAL (0x0c00)

#define SP_X( x ) ( (x) / DOT_RATE + 8 )
#define SP_Y( y ) ( (y) / DOT_RATE + 8 )

#define X_MIN (0*DOT_RATE)
#define X_MAX (256*DOT_RATE)
#define Y_MIN (0*DOT_RATE)
#define Y_MAX (256*DOT_RATE)

/* This is the define for get_ley() */
#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 4
#define KEY_RIGHT 8
#define KEY_A 16
#define KEY_B 32

/* Maximum direction value + 1 */
#define DIRECTION_MAX 16

/* Define the amount of movement */
#define SPEED_RATE 16
#define VX_DIR(direc, speed) (vx_dir[(direc)] * (speed) / SPEED_RATE)
#define VY_DIR(direc, speed) (vy_dir[(direc)] * (speed) / SPEED_RATE)

#define SINTABLE_NUM 128
#define SINTABLE_MAX 256	/* sintbl ÇÃ êUïù */
#define SIN(x) (sin_table[ (x) ])

extern short vx_dir[], vy_dir[], sin_table[];

typedef struct COM_TYPE {
	/* Coordinates */
	short x, y;

	/* Speeds */
	short vx, vy;

	/* Hit detection range */
	short rx, ry;

	short energy, str;

	short palet;
} COM_TYPE;

/* Number of sprites */
#define SP_NUM 64

/* Size (argument for sprite_alloc()) */
#define CHR1x1 1
#define CHR2x1 2
#define CHR1x2 3
#define CHR2x2 4
#define CHR3x3 8

typedef struct SP {
	/* Character type */
	short type;

	/* For register transfer */
	short x, y;
	unsigned short code;	/* <- short becomes a negative value when v-reverse is done, 
                                  and becomes strange when sign-extended to int. */
	short prw;

	/* I don't care */
	struct SP *next, *pre;
} SP;

int get_key( void );
void init( void );
void wait( void );
void ende( void );
void my_error( char *str );

/* Sprite functions */
void trans_sprite_data( void );
SP *sprite_alloc( short size );
void sprite_free( SP *sp );

/* Functions used for movement */
short calc_direction( short x1 , short y1 , short x2 , short y2 );

#endif
