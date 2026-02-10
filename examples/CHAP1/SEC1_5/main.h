/******************************
		  m a i n . h
******************************/

#define TRUE  (1)
#define FALSE (0)

#define SPRITE_NUM (128)

#define SPRATE (16)
#define SPX( x ) ( (x) / SPRATE )
#define SPY( y ) ( (y) / SPRATE )

#define X_MAX	  (280*SPRATE)
#define X_MIN	  (-10*SPRATE)
#define Y_MAX	  (280*SPRATE)
#define Y_MIN	  (-10*SPRATE)

#define KEY_UPPER (1 << 0)
#define KEY_LOWER (1 << 1)
#define KEY_LEFT  (1 << 2)
#define KEY_RIGHT (1 << 3)
#define KEY_A	  (1 << 4)
#define KEY_B	  (1 << 5)
#define KEY_EXIT  (1 << 6)

enum {
	SP_NO_DISP = -1, SP_DISP = 1
};

typedef struct _SP_DATA {
	short type;
	short x, y;
	unsigned short code, prw;
	struct _SP_DATA *next, *pre;
} SPDATA;

/*
  -direction-
	 0 1 2
	 3 4 5
	 6 7 8
*/
short check_dir( short x1, short y1, short x2, short y2 );
void *mymalloc( int size );
short easy_rand( void );
void sp_init( void );
void sp_trans( void );
SPDATA *sp_alloc( void );
void sp_free( SPDATA *sp );
int get_key( void );
