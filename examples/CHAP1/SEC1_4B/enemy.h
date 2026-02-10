/********************************
		  e n e m y . h
********************************/

#define ENEMY_NUM 5
#define BOMB_NUM 30

#define ENEMY_EXPL_COUNT 10
#define ENEMY_MAKE_COUNT 50

#define ENEMY_VX 1
#define ENEMY_VY 1

#define BOMB_VX 1
#define BOMB_VY 1

#define ENEMY_CODE 0x0102
#define BOMB_CODE 0x0103

enum {
	ENEMY_EXPL = -1, ENEMY_DEAD = 0, ENEMY_TYPE_A, ENEMY_TYPE_B
};
enum {
	BOMB_DEAD = 0, BOMB_TYPE_A, BOMB_TYPE_B
};

typedef struct {
	short type; 		  /* Type (dead/alive) */
	short x, y; 		  /* Coordinates */
	short vx, vy;		  /* Speed */
	short count;
} ENEMY;

typedef struct {
	short type;         /* Kinds */
	short x, y; 	    /* Coordinates */
	short vx, vy;
} BOMB;

void enemy_init( void );
void enemy_move(void);

void bomb_init(void);
void bomb_born( short x, short y );
void bomb_move(void);


