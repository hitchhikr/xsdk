/******************************
          m a i n . h
*******************************/

#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>

#define X_MAX     256
#define X_MIN     0
#define Y_MAX     256
#define Y_MIN     0

#define KEY_LEFT  (1)
#define KEY_RIGHT (2)
#define KEY_UPPER (4)
#define KEY_LOWER (8)
#define KEY_A     (16)

#define	TRUE	1
#define	FALSE	0

short get_key( void );
void *mymalloc( int size );
