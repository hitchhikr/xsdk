#include	"stdmomo.h"
#include	"battle.h"
#include	"display.h"
#include	"item.h"
#include	"sound.h"
#include	"bg.h"

void    sprite_clear(void)
{
	int	i;
	for( i = 0; i < 128; i++ ){
		my_spset(i, 0, 0, 0, 0);
	}
	disp_status_init();
}

void	crtinit(void)
{
	CRTMOD(14);	/* 256x256 16/65536 512  31kHz */
	G_CLR_ON();
	WINDOW(0, 0, 255, 255);
	HOME(0, 0, 0); APAGE(0);VPAGE(3);
	B_CUROFF();
	C_CUROFF();
	SP_ON();

	BGCTRLST(0, 0, 0);			/* Background settings */
	BGCTRLST(1, 1, 1);
	BGSCRLST(1, 0, 0);

	*( unsigned char * )0xe82500 = 0x12;
	sprite_clear();
}

#define	CRASH_STARTCOUNT	880
#define	CRASH_PATNUM		56

int	crash( int cnt, int x, int y, int pat)
{
	int	i;
	short	crashpattern[ CRASH_PATNUM ] = {
		0x500, 0x500, 0x500, 0x500, 0x501, 0x502, 0x503, 0x504,
		0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505,
		0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505,
		0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505,
		0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505, 0x505,
		0x504, 0x504, 0x503, 0x503, 0x502, 0x502, 0x501, 0x501,
		0x500, 0x500, 0x500, 0x500, 0x500, 0x500, 0x500, 0x500
	};

	if (cnt == CRASH_STARTCOUNT - 16) {
		sd_on( SD_CRASH );
		for( i = 0; i < 64; i++ ){
			makebrick(x, y, (rand() % 32767) / 256 - 64, (rand() % 32767) / 256 - 64, 48);
		}
	}
	if (count & 1) {
		i = (CRASH_STARTCOUNT - cnt) / 2;
		if( i >= CRASH_PATNUM ) return 0x100;
		return crashpattern[ i ];
	} else {
		if( cnt <= CRASH_STARTCOUNT - 60 ) return 0x100;
		return pat;
	}
}

void	disp( void )
{
	short	i, pat, prw;
	short	j = 16;		/* Sprite number (0-15 is used for gauges) */
	man	*mp = mandat;
	laser	*lp = laserdat;
	item	*ip = itemdat;
	static	int	counter;
	counter++;
			/*	Players		*/
	for( i = 0; i < PLAYERNUM ; i++,mp++){
		pat = 0x120 + i * 0x100 + (mp->dir *4) + (mp->Random/4 & 3 );
		prw = 1;
		if( mp->dead_mode ){
			int	tp;
			tp = (mp->dead_mode / 4);
			if( tp > 12 ) tp = 12;
			pat =  tp * 4 + 0x180 + (mp->dead_mode & 3) + i * 0x100;
			my_spset( j++, SCX(mp->x + tp * 8), SCY(mp->y + tp * 8), pat, prw );
		} else if( mp->hit > 500 ){
			pat = crash( mp->hit, mp->x, mp->y, pat );
			my_spset( j++, SCX(mp->x), SCY(mp->y), pat, prw );
		} else {
			if (mp->hit && (count & 1)) prw = 0;
			my_spset( j++, SCX(mp->x), SCY(mp->y), pat, prw );
		}
	}
			/*	items	*/
	for( i = 0; i < ITEM_NUM; i++, ip++){
		if( !ip->cond ) {
			my_spset( j++, 0, 0, 0, 0 );
		} else {
			my_spset( j++, SCX(ip->x), SCY(ip->y), ip->patnum, 1 );
		}
	}
			/*	FIRE	*/
	for( i = 0; i < PLAYERNUM ; i++,lp++ ){
		if( lp->cond )	{
			if( lp->type == LASER )	pat = 0x140 + i * 0x100 + lp->dir;
			else {
				pat = 0x170 + i * 0x100 + lp->dir;
				if( counter & 1 ) pat += 8;
			}
			my_spset( j++, SCX(lp->x), SCY(lp->y), pat, 1 );
		} else {
			my_spset( j++, SCX(lp->x), SCY(lp->y), 0, 0 );
		}
	}
	/* Shadow displayed every other time */
	mp = mandat;
	if( counter & 1 ) mp++;
	if( mp->dead_mode ) {	/* There's no shadow when you're falling */
		my_spset( j++, SCX(mp->x), SCY(mp->y), 0x150, 0 );
	} else	my_spset( j++, SCX(mp->x), SCY(mp->y), 0x150, 1 );
}

void	bgmake( void )
{
	fade(32);
}
