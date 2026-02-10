/********************************
		  s h i p . h
********************************/

#define VELO_X	  1
#define VELO_Y	  1
#define SHOT_VX   4

#define SHOT_NUM  3

#define SHIP_CODE 0x0100
#define SHOT_CODE 0x0101

enum {
	SHIP_DEAD = 0, SHIP_ALIVE
};

enum {
	SHOT_DEAD = 0, SHOT_ALIVE
};

typedef struct _SHIP {
	short type;
	short x, y;
} SHIP;

typedef struct _SHOT {
	short type;
	short x, y;
	short vx, vy;
} SHOT;

void ship_init( void );
void ship_move( void );
void shot_init( void );
void shot_move( void );
