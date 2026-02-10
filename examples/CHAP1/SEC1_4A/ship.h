/*******************************
          s h i p . h
*******************************/

/* Moving speed */
#define VELO_X    1
#define VELO_Y    1
#define SHOT_VX   4

/* Number of self-shots */
#define SHOT_NUM  3

#define SHIP_CODE 0x0100
#define SHOT_CODE 0x0101

#define TP_DEAD -1
#define TP_ALIVE 1

typedef struct {
    short flag;
    short x, y;
} SHIP;

typedef struct {
    short flag;
    short x, y;
} SHOT;

void ship_init( void );
void ship_move( void );
void shot_init( void );
void shot_move( void );
