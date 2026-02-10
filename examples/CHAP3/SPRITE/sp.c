
#include <stdio.h>
#include <iocslib.h>
#include "main.h"
#include "sp.h"

/* for sp_sub.s */
unsigned short *sp_trans_adrs;

typedef struct {
	SP low[ SP_L_NUM ];
	SP mid[ SP_M_NUM ];
	SP high[ SP_H_NUM ];
} SP_TRANS_DATA;

static SP_TRANS_DATA *sp_trans_data;
static short sp_bank_select;

static SP *sp_data;
static SP_DATA *sp_buff, *sp_free_buff, *sp_used_buff;

void sp_init( void )
{
	short i;
	SP_DATA *sp;

	CRTCRAS( 0, 0 );

	/* Initialization of data used by sp_alloc() and sp_free() */
	sp_data = (SP *) my_malloc( sizeof(SP) * SP_NUM );
	sp_buff = (SP_DATA *) my_malloc( sizeof(SP_DATA) * SP_NUM );
	for( i = 0, sp = sp_buff; i < SP_NUM; i ++, sp ++ ) {
		sp->next = sp + 1;
		sp->sp = sp_data + i;
	}
	( sp - 1 )->next = 0;
	sp_free_buff = sp_buff;
	sp_used_buff = 0;

	/* For transferring data */
	sp_trans_data = (SP_TRANS_DATA *) my_malloc( sizeof(SP_TRANS_DATA) * 2 );
	sp_bank_select = 0;
	for( i = 0; i < 2; i ++ ) {
		short j;
		
		for( j = 0; j < SP_L_NUM; j ++ )
			sp_trans_data[ i ].low[ j ].prw = 0;
		for( j = 0; j < SP_M_NUM; j ++ )
			sp_trans_data[ i ].mid[ j ].prw = 0;
		for( j = 0; j < SP_H_NUM; j ++ )
			sp_trans_data[ i ].high[ j ].prw = 0;
	}

	/* Initialization for sp_sub.s */
	sp_trans_adrs = (unsigned short *) ( sp_trans_data + 0 );

	/* Set interrupt */
	CRTCRAS( sp_trans, SP_R0 );
}

void sp_end( void )
{
	CRTCRAS( 0, 0 );
}

SP_DATA *sp_alloc( void )
{
	SP_DATA *sp;

	if ( !( sp = sp_free_buff ) ) return 0;
	sp_free_buff = sp->next;
	if ( sp->next = sp_used_buff ) {
		sp->next->pre = sp;
	}
	sp->pre = 0;
	sp_used_buff = sp;
	sp->sp->prw = SP_ALLOC_PRW;
	return sp;
}

void sp_free( SP_DATA *sp )
{
	if ( !( sp->pre ) ) {
		if ( sp->next ) {
			sp->next->pre = 0;
		}
		sp_used_buff = sp->next;
	} else {
		if ( sp->pre->next = sp->next ) {
			sp->next->pre = sp->pre;
		}
	}
	sp->next = sp_free_buff;
	sp->sp->prw = SP_FREE_PRW;
	sp_free_buff = sp;
}

void sp_set( void )
{
	SP_DATA *sp;
	SP_TRANS_DATA *sp_trans;
	short l, m, h;

	if ( sp_bank_select ) {
		sp_trans = sp_trans_data + 0;
	} else {
		sp_trans = sp_trans_data + 1;
	}
	for( sp = sp_used_buff, l = m = h = 0; sp; sp = sp->next ) {
		if ( sp->sp->y < ( SP_Y0 + SP_Y1 ) / 2 && l < SP_L_NUM ) {
			sp_trans->low[ l ].x = sp->sp->x;
			sp_trans->low[ l ].y = sp->sp->y;
			sp_trans->low[ l ].code = sp->sp->code;
			sp_trans->low[ l ].prw = sp->sp->prw;
			l ++;
		} else if ( h < SP_H_NUM ) {
			sp_trans->high[ h ].x = sp->sp->x;
			sp_trans->high[ h ].y = sp->sp->y;
			sp_trans->high[ h ].code = sp->sp->code;
			sp_trans->high[ h ].prw = sp->sp->prw;
			h ++;
		}
		if ( sp->sp->y > SP_Y0 && sp->sp->y <= SP_Y1 && m < SP_M_NUM ) {
			sp_trans->mid[ m ].x = sp->sp->x;
			sp_trans->mid[ m ].y = sp->sp->y;
			sp_trans->mid[ m ].code = sp->sp->code;
			sp_trans->mid[ m ].prw = sp->sp->prw;
			m ++;
		}

	}
	for( ; l < SP_L_NUM; l ++ ) {
		sp_trans->low[ l ].prw = 0;
	}
	for( ; m < SP_M_NUM; m ++ ) {
		sp_trans->mid[ m ].prw = 0;
	}
	for( ; h < SP_H_NUM; h ++ ) {
		sp_trans->high[ h ].prw = 0;
	}
	if ( sp_bank_select ) {
		sp_bank_select = 0;
		sp_trans_adrs = (unsigned short *) ( sp_trans_data + 0 );
	} else {
		sp_bank_select = 1;
		sp_trans_adrs = (unsigned short *) ( sp_trans_data + 1 );
	}
}
