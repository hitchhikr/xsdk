#include	<stdlib.h>
#include	"battle.h"
#include	"item.h"

item	itemdat[ ITEM_NUM ];

item	*search_item( void )
{
	int	i = 0;
	item	*ip = itemdat;

	while( ip->cond ) {
		i++; ip++;
		if( i >= ITEM_NUM ) return (void *)0;
	}
	return ip;
}

/*
 *		make_simpleitem
 */

void make_simpleitem( short x, short y, short cond, short patnum)
{
	item	*ip;

	ip = search_item();
	if( !ip ) return;
	ip->x = x;
	ip->y = y;
	ip->vx = 0;	ip->vy = 0;	ip->dir = 0;	ip->flg = 0;
	ip->type = ITEM_SIMPLE;
	ip->cond = cond;
	ip->patnum = patnum;
}
/*
 *		makebrick
 */

void makebrick( short x, short y, short vx, short vy, short en)
{
	item	*ip;

	ip = search_item();
	if( !ip ) return;
	ip->x = x;
	ip->y = y;
	ip->vx = vx;
	ip->vy = vy;
	ip->type = ITEM_BRICK;
	ip->cond = en;
}

/*
 *		makelaser
 */

void make_laseritem( short x, short y, short pl, short dir)
{
	item	*ip;

	ip = search_item();
	if( !ip ) return;
	ip->x = x;
	ip->y = y;
	ip->vx = ip->vy = 0;
	ip->type = ITEM_LASER;
	ip->flg = pl;
	ip->dir = dir;
	ip->cond = 15;
}

/*
 *		makebricks
 */

void	makebrickr( short x, short y, short vx, short vy)
{
	int	i;
	vx += (rand() % 32767) / 1024- 16;
	vy += (rand() % 32767) / 1024- 16;
	makebrick( x, y, vx, vy, 32 );
}

void itemmove( void )
{
	int	i;
	item	*ip = itemdat;
	for( i = 0; i < ITEM_NUM; i++, ip++){
		if( !ip->cond ) continue;
		switch( ip->type ){
		  case ITEM_BRICK:
			ip->x += ip->vx;
			ip->y += ip->vy;
			ip->cond--;
			ip->vx = ip->vx * 31 / 32;
			ip->vy = ip->vy * 31 / 32;
			ip->patnum = ((ip->cond + i) & 7) + 0x551; 
			break;
		  case ITEM_LASER:
			ip->patnum = 0x16c + (ip->flg) * 0x100 +
					(ip->dir & 3) - (ip->cond / 4) * 4;
			ip->cond--;
			break;
		  case ITEM_SIMPLE:
			ip->x += ip->vx;
			ip->y += ip->vy;
			ip->cond--;
			break;
		}
	}
}

void iteminit( void )
{
	int	i;
	item	*ip = itemdat;
	for( i = 0; i < ITEM_NUM; i++, ip++){
		ip->cond = 0;
	}
}

