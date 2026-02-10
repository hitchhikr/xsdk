#include <stdio.h>
#include <stdlib.h>
#include <iocslib.h>
#include "main.h"
#include "sp.h"

#define	DIV	256
#define	XMAX (256 * DIV)
#define	YMAX (256 * DIV)
#define	XMIN 0
#define	YMIN 0

#define OBJNUM 180
typedef struct {
	int x;
	int y;
	int vx;
	int vy;
	SP_DATA *spdata;
} OBJECT;
OBJECT obj[OBJNUM];

void pe( char *str )
{
	fputs( str, stdout );
	exit( 1 );
}

void *my_malloc( int size )
{
	void *p;

	if ( !( p = (void *) malloc( size ) ) )
		pe( "cannot allocate memory.\n" );
	return p;
}

void init( void )
{
	short i;

	CRTMOD( 10 ); /* 256x256 256 colors */
	G_CLR_ON();
	SP_ON();

	sp_init();
	for (i = 0; i < OBJNUM; i++) {
		if ( !( obj[i].spdata = sp_alloc() ) ) pe( "cannot allocate sprite.\n" );
		(obj[i].spdata)->sp->prw = 2;
		(obj[i].spdata)->sp->code = 0x100;
		obj[i].vx = (rand() & 511) + 1024;
		obj[i].vy = (rand() & 511) + 1024;
		obj[i].x = rand() % 65534;
		obj[i].y = rand() % 65534;
	}
}

void ende( void )
{
	sp_end();
}

void loop( void )
{
	short i;

	while( !(BITSNS(0) && 2) ) {
		for (i = 0; i < OBJNUM; i++) {
			if (obj[i].x < XMIN && obj[i].vx < 0) obj[i].vx = -obj[i].vx;
			if (obj[i].x > XMAX && obj[i].vx > 0) obj[i].vx = -obj[i].vx;
			if (obj[i].y < YMIN && obj[i].vy < 0) obj[i].vy = -obj[i].vy;
			if (obj[i].y > YMAX && obj[i].vy > 0) obj[i].vy = -obj[i].vy;
			obj[i].x += obj[i].vx;
			obj[i].y += obj[i].vy;
			(obj[i].spdata)->sp->x = obj[i].x / DIV;
			(obj[i].spdata)->sp->y = obj[i].y / DIV;
		}
/*
		fprintf( stdout, "[1;1HX:%3d  Y:%3d\n", ship->sp->x, ship->sp->y );
*/
		sp_set();
		sp_wait();
	}
}

void main( void )
{
	init();
	loop();
	ende();
}

