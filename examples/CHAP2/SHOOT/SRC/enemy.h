
#include "sub.h"
#define ENEMY_NUM 16

typedef struct ENEMY {
	short type;			/* Kinds */

	short count;			/* Time since appearance */
	short state;			/* For motion control */
	short dir;			/* Direction */
	short flg;			/* Etc */

	short flash_count;		/* Flash Count */

	short atari;			/* For hit detection */
	int (*move)( struct ENEMY * );	/* Movement functions */
	SP *sp;				/* Sprite */

	COM_TYPE data;			/* Common Data */

	struct ENEMY *next, *pre;	/* For list structures */
} ENEMY;

#define BOMB_NUM 72

typedef struct BOMB {
	short type;

	/* Sprite */
	SP *sp;
	
	/* Common Data */
	COM_TYPE data;

	/* I don't care */
	struct BOMB *next, *pre;
} BOMB;

/*
 *		Enemy appearance pattern structure
 */

typedef	struct	_enepat {
	short count;			/* Appearance time */
	short type;			/* Enemy Type */
	short x, y;			/* Appearance position */
	short vx, vy;			/* Initial velocity */
	short dir;			/* Direction */
	short flg;			/* Bits and pieces */
} enepat;

#define	ROUNDMAX	3		/* Number of faces */
#define	ENEMAX		256		/* Maximum number of enemies that can appear on one surface */

/*
 *		Enemy Information Structure
 */

typedef struct _ENEINFO {
	short energy;			/* Hardness (for hit detection) */
	short str;			/* Attack power (for hit detection) */
	short points;			/* Score */
	short spsize;			/* Sprite size */
	short rx, ry;			/* Hitbox size */
	int (*move)( struct ENEMY * );	/* Enemy movement functions */
	int crash_sdno;			/* Sound number when destroyed */

} ENEINFO;

extern	ENEINFO	eneinfo[];

void enemy_init( short scene );

void enemy_move( void );
void enemy_make( short scene, short loc );
ENEMY *enemy_alloc( short spsize );
ENEMY *enemy_erase( ENEMY *ene );
void enemy_explode( ENEMY *ene );

void bomb_move( void );
void bomb_make( short x, short y, short vx,  short vy, short type );
BOMB *bomb_erase( BOMB *bomb );
