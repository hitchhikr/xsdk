/*
 *		item.c(Smoke & Bit routine for DesperadO-X)
 *
 *				Programmed by chika@kmc	1994 Apr
 */


#include	<stdlib.h>
#include	"sps.h"
#include	"dsp.h"
#include	"item.h"

#define	ITEM_NUM	128
#define	TYPE_SMOKE	0
#define	TYPE_BRICK	1
#define	TYPE_LIGHT	2
#define	LIGHT_TIME	15
#define	LIGHT_TIME2	16

typedef struct	_item{
	struct	_item	*next;
	struct	_item	*prev;

	short		condition;
	short		type;
	short		x;
	short		y;
	short		vx;
	short		vy;
	short		palet;

	SPS  		*sp;
} ITEM;

ITEM	itemdata[ITEM_NUM];

static	int	bricknum[12] = { 0x10, 0x11, 0x1c, 0x1d, 0x10, 0x11, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0 };
extern	short	screen_x0, screen_y0;		// Virtual coordinates of the top left corner of the real screen
extern	int	count_;

static	ITEM	*item_free = (void *)0;
static	ITEM	*item_used = (void *)0;


void	ItemInit(void)
{
	int	i;
	ITEM	*it;

	for(i = 0, it = &itemdata[0]; i < ITEM_NUM; i++, it++) {
		it->next = it + 1;
		it->prev = it - 1;
		it->condition = 0;
		it->sp = (void *)0;
	}
	itemdata[0].prev = (void *)0;
	itemdata[ITEM_NUM - 1].next = (void *)0;
	item_free = &itemdata[0];
	item_used = 0;

}

/*
 *		Object appearance
 */

void	MakeItem(short x, short y, short vx, short vy,
						  short type, short palet)
{
	ITEM	*itemp = itemdata;
	SPS	*sp;

	if (!item_free) return;
	if (!(sp = SpsAlloc(CHR1x1))) return;

	itemp = item_free;
	item_free = item_free->next;
	/* Add to list of used items */
	if (itemp->next = item_used) {
		item_used->prev = itemp;
	}
	itemp->prev = 0;
	item_used = itemp;

	itemp->x = x;
	itemp->y = y;
	itemp->type = type;
	itemp->palet = palet;
	itemp->condition = 1;
	itemp->vx = vx;
	itemp->vy = vy;
	itemp->sp = sp;

}
/*	Object disappears	*/
ITEM	*item_erase(ITEM *ip)
{
	ITEM	*next;

	next = ip->next;
	ip->condition = 0;

	/* Remove from the list of uses */
	if (ip->prev) {
		ip->prev->next = ip->next;
		if (ip->next) ip->next->prev = ip->prev;
	} else {
		if (ip->next) ip->next->prev = 0;
		item_used = ip->next;
	}

	/* Add to vacant list */
	ip->prev = 0;
	ip->next = item_free;
	item_free = ip;

	/* Free the sprite */
	SpsFree(ip->sp);	ip->sp = (void *)0; 

	return next;
}


void	ItemMove(void)
{
	int	i = 0;
	int	x, y;
	short	dx;
	ITEM	*itemp = itemdata;

	for(itemp = item_used; itemp; i++) {

		if (!(itemp->condition)) continue;

		x = itemp->x;
		y = itemp->y;
		itemp->y = (y += itemp->vy);
		itemp->x = (x += itemp->vx);
		itemp->condition++;
		switch(itemp->type) {
		  case	0:			/* Smoke */
			if ((count_ & 1) == (i & 1) ) {
				itemp->sp->num = 0x18 + itemp->condition / 8;
			} else {
				itemp->sp->num = SPNULL;
			}
			itemp->sp->x = dx = SCX1x1(x, screen_x0);
			itemp->sp->y = SCY1x1(y, screen_y0);
			itemp->sp->palet = 0xc;
			if (itemp->condition >= 32) {	/* Extinction */
				itemp->condition = 0;
			}
			break;
		  case	1:			/* Debris */
			itemp->vy++;
			itemp->sp->num = bricknum[itemp->condition / 8];
			itemp->sp->x = dx = SCX1x1(x, screen_x0);
			itemp->sp->y = SCY1x1(y, screen_y0);
			if (!(count_ + i & 13)) {
				itemp->sp->palet = 0xd;
			} else {
				itemp->sp->palet = itemp->palet;
			}
			if (itemp->condition >= 80 || dx < 0 || dx > 272) {	/* Extinction */
				itemp->condition = 0;
			}
			break;
		  case	2:			/* Light spot */
			if ((count_ & 1) == (i & 1) ) {
				itemp->sp->num = 0x2;
			} else {
				itemp->sp->num = 0;
			}
			itemp->sp->x = dx = SCX1x1(x, screen_x0);
			itemp->sp->y = SCY1x1(y, screen_y0);
			itemp->sp->palet = 0xc;
			if (itemp->condition >= LIGHT_TIME) {	/* Extinction */
				itemp->condition = 0;
			}
			break;
		}
		if (itemp->condition) {
			itemp = itemp->next;
		} else {
			itemp = item_erase(itemp);
		}

	}
}

#define	MakeBrick1	MakeItem(x, y, vx2 + (rand() % 32767) / 512 - 32, vy2 + (rand() % 32767) / 512 - 32, 1,palet);
#define	MakeBrick2	MakeItem(x, y, vx * 5 / 16+ (rand() % 32767) / 512-32, (rand() % 32767) / 1024 - 16, 1,palet);

void	MakeBricks(int x, int y, int vx, int vx2, int vy2, int palet)
{
	MakeBrick2	MakeBrick2	MakeBrick2	MakeBrick2	MakeBrick2
	MakeBrick1	MakeBrick1	MakeBrick1	MakeBrick1	MakeBrick1
	palet = 14;
	MakeBrick2	MakeBrick2	MakeBrick2	MakeBrick2
	MakeBrick1	MakeBrick1	MakeBrick1	MakeBrick1

}

void	Make1Brick(int x, int y, int vx, int vy, int palet)
{
	MakeItem(x + (rand() % 32767) / 128 - 128, y + (rand() % 32767) / 128 - 128,
		vx + (rand() % 32767) / 512-32, vy + (rand() % 32767) / 1024 - 16, 1,palet);
}

void	Make1Light(int x, int y, int rx, int ry)
{
	int	vx, vy;
	vx = (rand() % 32767) / 512;
	vy = (rand() % 32767) / 512;
	if (rx > 0) vx = -vx;
	if (ry > 0) vy = -vy;
	MakeItem(x - vx * LIGHT_TIME2, y - vy * LIGHT_TIME2, vx, vy, TYPE_LIGHT, 0xc);
}
