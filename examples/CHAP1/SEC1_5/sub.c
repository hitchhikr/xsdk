/******************************
		  s u b . c
******************************/

#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>

#include "main.h"

#ifdef __LIBC__
#define MY_SPREGST( no, flag, x, y, code, prw ) \
	SP_REGST( (no), (flag), (x), (y), (code), (prw) )
#else
#define MY_SPREGST( no, flag, x, y, code, prw ) \
	SP_REGST( (no) | (flag), (x), (y), (code), (prw) )
#endif

/* Register Buffer */
static SPDATA *spreg_buf;
/* The number currently in use */
static short sp_used_num = 0;

/* for sprite_alloc & sprite_free */
static SPDATA *spreg_free = 0;
static SPDATA *spreg_used = 0;

void *mymalloc( int size )
{
	void *p;

	if ( !( p = (void *) malloc( size ) ) ) {
		fputs( "cannot alloc memory at mymalloc()", stderr );
		exit( 1 );
	}
	return p;
}

/* Random numbers */
short easy_rand( void )
{
	static unsigned long int next = 3749696;

	next = next * 1103515245 + 5963;
	return (short) ( next / 65536 );
}

int get_key( void )
{
	static short key_a_flag = TRUE;
	int k, d, s;

	s = ~JOYGET( 0 );
	d = s & 0x0f;

	k = BITSNS( 8 );
	if ( k & 0x10 ) d |= KEY_UPPER;
	if ( k & 0x80 ) d |= KEY_LEFT;
	k = BITSNS( 9 );
	if ( k & 0x02 ) d |= KEY_RIGHT;
	if ( k & 0x10 ) d |= KEY_LOWER;
	k = BITSNS( 14 );
	if ( (k & 0x04) || (s & 0x20) ) {
		if ( key_a_flag ) {
			d |= KEY_A;
			key_a_flag = FALSE;
		}
	} else {
		key_a_flag = TRUE;
	}
	if ( (k & 0x08) || (s & 0x40) )		d |= KEY_B;
	k = BITSNS( 0 );
	if ( k & 0x02 ) { /* ESC */
		d |= KEY_EXIT;
	}
	return d;
}

/*
  -direction-
	 0 1 2
	 3 4 5
	 6 7 8
*/
#define DIR_SGN( c ) ( (c) > 0 ? 2 : ( (c) < 0 ? 0 : 1 ) )
#define DIR_DIV (16*SPRATE)

short check_dir( short x1, short y1, short x2, short y2 )
{
	return DIR_SGN( (x2 - x1) / DIR_DIV ) + DIR_SGN( (y2 - y1) / DIR_DIV ) * 3;
}

void sp_init( void )
{
	int j;
	SPDATA *sp;

	spreg_buf = (SPDATA *) mymalloc( sizeof(SPDATA) * SPRITE_NUM );
	for( j = 0; j < 128; j ++ ) {
		MY_SPREGST( j, 0x80000000, 0, 0, 0, 0 );
	}
	for( j = 0, sp = spreg_buf; j < SPRITE_NUM; j ++, sp ++ ) {
		sp->type = SP_NO_DISP;
		sp->next = sp + 1;
		sp->pre = sp - 1;
	}
	spreg_buf->pre = 0;
	( spreg_buf + SPRITE_NUM - 1 )->next = 0;
	spreg_free = spreg_buf;
	spreg_used = 0;
}

void sp_trans( void )
{
	int j;
	SPDATA *sp;

	/* Wait for vertical blanking interval */
	MY_SPREGST( 0, 0, 0, 0, 0, 0 );

#if 1
	for( j = 0, sp = spreg_used; sp; sp = sp->next, j ++ ) {
		MY_SPREGST( j, 0x80000000, sp->x & 511, sp->y & 511, sp->code, sp->prw );
	}
#else
	for( j = 0, sp = spreg_buf; j < SPRITE_NUM; sp ++, j ++ ) {
		if ( sp->type == SP_NO_DISP ) continue;
		else MY_SPREGST( j, 0x80000000, sp->x & 511, sp->y & 511, sp->code, sp->prw );
	}
#endif
	for( ; j < SPRITE_NUM; j ++ ) {
		MY_SPREGST( j, 0x80000000, 0, 0, 0, 0 );
	}
}

SPDATA *sp_alloc( void )
{
	SPDATA *sp;

	/* Find a vacant one */
	if ( !spreg_free ) return 0;
	sp = spreg_free;
	spreg_free = spreg_free->next;

	/* Add to list of used items */
	if ( sp->next = spreg_used ) {
		spreg_used->pre = sp;
	}
	sp->pre = 0;
	spreg_used = sp;

	sp->type = SP_DISP;
	sp->prw = 2;
	sp_used_num ++;
	return sp;
}

void sp_free( SPDATA *sp )
{
	sp->type = SP_NO_DISP;

	/* Remove from the list of uses */
	if ( sp->pre ) {
		sp->pre->next = sp->next;
		if ( sp->next ) sp->next->pre = sp->pre;
	} else {
		if ( sp->next ) sp->next->pre = 0;
		spreg_used = sp->next;
	}

	/* Add to vacant list */
	sp->pre = 0;
	sp->next = spreg_free;
	if ( spreg_free ) spreg_free->pre = sp;
	spreg_free = sp;

	sp_used_num --;
}
