/*****************************
		  e n e m y . h
*****************************/

#define ENEMY_NUM (5)
#define BOMB_NUM (15)

#define ENEMY_MAKE_COUNT (50)
#define ENEMY_EXPL_COUNT (10)

#define ENEMY_VX (1*SPRATE)
#define ENEMY_VY (1*SPRATE)

#define BOMB_VX (1.5*SPRATE)
#define BOMB_VY (1.5*SPRATE)

#define ENEMY_CODE (0x0102)
#define BOMB_CODE (0x0103)

enum {
	ENEMY_NONEXIST = -1, ENEMY_EXIST = 0
};
enum {
	BOMB_NONEXIST = -1, BOMB_EXIST = 0, BOMB_TYPE_A
};

typedef struct _ENEMY {
	short type; 		  /* Type (dead/alive) */
	short x, y; 		  /* Coordinates */
	short vx, vy;		  /* Speed */
	short count;
	void (*move)( struct _ENEMY * );
	SPDATA *sp;
} ENEMY;

typedef struct _BOMB {
	short type; 		  /* Kinds */
	short x, y; 		  /* Coordinates */
	short vx, vy;		  /* Speed */
	void (*move)( struct _BOMB * );
	SPDATA *sp;
} BOMB;

typedef struct _ENEMY_MAP_DATA {
	short type;
	short x, y;
	void (*move)( struct _ENEMY * );
} ENEMY_MAP_DATA;

void enemy_init( void );
void enemy_move(void);

void bomb_init(void);
void bomb_born( short x, short y );
void bomb_move(void);
BOMB *bomb_make( short type );
void bomb_erase( BOMB *bomb );

ENEMY *enemy_make( short type );
void enemy_erase( ENEMY *enemy );

ENEMY_MAP_DATA *get_enemy_map_data( short loc );
