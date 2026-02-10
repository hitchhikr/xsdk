/*****************************
		  s h i p . h
*****************************/

#define SHIP_VX   20
#define SHIP_VY   20
#define SHOT_VX   40

#define SHOT_NUM  3

#define SHIP_CODE 0x0100
#define SHOT_CODE 0x0101

enum {
	SHIP_NONEXIST = -1, SHIP_EXIST = 0
};

enum {
	SHOT_NONEXIST = -1, SHOT_EXIST = 0
};

typedef struct _SHIP {
	short type;
	short x, y;
	SPDATA *sp;
} SHIP;

typedef struct _SHOT {
	short type;
	short x, y;
	short vx, vy;
	SPDATA *sp;
} SHOT;

void ship_init( void );
void ship_move( void );

void shot_init( void );
void shot_move( void );
void shot_make( short x, short y );
void shot_erase( SHOT *shot );
