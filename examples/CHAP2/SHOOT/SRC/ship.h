
#include "sub.h"

#define SHIP_NUM 1

typedef struct SHIP {
	short type;
	
	/* Number of remaining lives */
	short left;
	/* Shot Power */
	short pow;
	/* The direction your ship is facing */
	short dir;
	/* Death counter (0 means alive) */
	short dead_count;
	/* Sprite */
	SP *sp;

	/* Common Data */
	COM_TYPE data;
} SHIP;

#define SHOT_NUM 16

typedef struct SHOT {
	short type;

	/* Sprite */
	SP *sp;
	
	/* Common Data */
	COM_TYPE data;

	/* I don't care */
	struct SHOT *next, *pre;
} SHOT;


/* Function prototype declaration */
int ship_move( void );
void ship_init( void );
void shot_move( void );
void shot_make( short x, short y, short vx, short vy, short type );
SHOT *shot_erase( SHOT *shot );
