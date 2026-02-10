
#ifndef _SP_H_
#define _SP_H_

#define SP_L_NUM      ( 90 )
#define SP_M_NUM      ( 38 )
#define SP_H_NUM      ( 90 )

#define SP_NUM        ( SP_L_NUM + SP_M_NUM + SP_H_NUM )
#define SP_TRANS_SIZE ( 128 )

#define SP_Y0         ( 112 )
#define SP_Y1         ( 144 )

#define SP_R0         ( 260 )
#define SP_R1         ( 131 )

#define SP_FREE_PRW   ( 0 )
#define SP_ALLOC_PRW  ( 3 )

typedef struct {
	short x, y;
	unsigned short code, prw;
} SP;

typedef struct SP_DATA {
	struct SP_DATA *pre, *next;
	SP *sp;
} SP_DATA;


void sp_init( void );
void sp_end( void );
void sp_wait( void );
void sp_set( void );
SP_DATA *sp_alloc( void );
void sp_free( SP_DATA *sp );

/* sp_sub.s */
void sp_trans( void );

#endif


