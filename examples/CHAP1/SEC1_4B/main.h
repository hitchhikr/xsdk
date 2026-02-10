/********************************
		  m a i n . h
********************************/

#define X_MAX	  256
#define X_MIN	  0
#define Y_MAX	  256
#define Y_MIN	  0

#define KEY_LEFT  (1)
#define KEY_RIGHT (2)
#define KEY_UPPER (4)
#define KEY_LOWER (8)
#define KEY_A	  (16)

/*
  -direction-
	 0 1 2
	 3 4 5
	 6 7 8
*/


#define	TRUE	1
#define	FALSE	0


short check_dir( short x1, short y1, short x2, short y2 );
void *mymalloc( int size );
short easy_rand( void );
short get_key( void );
