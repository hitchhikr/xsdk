#define	ITEM_NUM	64
#define	ITEM_LASER	0
#define	ITEM_BRICK	1
#define	ITEM_SIMPLE	2

typedef struct	_item {
	short	cond;
	short	type;
	short	x, y;
	short	vx, vy;
	short	dir;
	short	flg;
	short	patnum;
} item;

void make_simpleitem( short x, short y, short cond, short patnum);
void makebrick( short x, short y, short vx, short vy, short en);
void makebrickr( short x, short y, short vx, short vy);
void make_laseritem( short x, short y, short pl, short dir);
void itemmove( void );
void iteminit( void );

extern	item	itemdat[ ITEM_NUM ];
