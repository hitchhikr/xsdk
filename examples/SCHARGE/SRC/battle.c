/*
 *		Star * Charge
 */

#include	"stdmomo.h"
#include	"battle.h"
#include	"man.h"
#include	"stick.h"
#include	"atari.h"
#include	"display.h"
#include	"item.h"
#include	"bg.h"
#include	"textput.h"
#include	"sound.h"
#include	"stage.h"
#include	"title.h"
#include	"etc.h"
#include	"rand.h"
#include	"cpu.h"

#define	ROUNDMAX	5

typedef struct _res {
	enum	{ KO, TIMEOVER, RINGOUT, DOUBLEKO, DRAW} why;
	int	timer;
	int	win;
} res;
res result[ROUNDMAX+1];

#define	TIMEMAX		(30*60)
#define	CONFIGNUM 2
config	cnf[CONFIGNUM];
config	*nowcnf;
man	mandat[PLAYERNUM];
laser	laserdat[PLAYERNUM];
int	count, round_ = 0;
int	gamemode = 0;
int	battle[CONFIGNUM + 1] = {0, 0, 0};
int	enableshot;
int	timer;
char	*pic_loadpath;
int	commode[PLAYERNUM] = {1, 2};		/* Player or computer? */
int	nmiflg;				/* When triggered, it becomes 1 */

#define	LIFE	2
#define	STRBUFSIZE	256
static char	strbuf[ STRBUFSIZE ];
static char	strbuf2[ STRBUFSIZE ];

/*
 *		Normal single unit processing
 */

void	disp_push_start( void )
{
	int	p;
	static	char	mes[4][20] = { "ENERGY", "CHARGE", "PUSH  ", " START" };
	count++;
	if (!(count & 63)) {
		p = (count & 64) / 32;
		if (commode[0] == MODE_CPU) {
			tx_puts(  1, 0, mes[p]  , BASENUM);
			tx_puts(  1, 1, mes[p+1], BASENUM);
		}
		if (commode[1] == MODE_CPU) {
			tx_puts( 18, 30, mes[p]  , BASENUM);
			tx_puts( 18, 31, mes[p+1], BASENUM);
		}
	}
}

void	idle(void)
{
	STICK	s;
	int	i;
	disp_push_start();
	addcharge();
	lasermove();
	atari();
	itemmove();
	disp_status();
	wait();
	disp();
	for (i = 0; i < 2; i++) {
		s = JOY(i);
		if (s.flg.lb || s.flg.rb) {
			if(commode[i] == MODE_CPU) nmiflg = i+1;
		}
	}
}

int	dispresult(void)
{
	int	i, y, tmp, tw = 0, exitflg = 0, j;
	int	winnum[PLAYERNUM]={0, 0};
	res 	*rp;
	STICK	s;

	bgdisp(2);
	sprite_clear();
	for (i = 32; i >= 0; i--){ fade(i);wait(); }

	tx_puts_big(9, 4, "RESULTS", BIG_BASENUM);

	y = 7;
	for (i = 1; i <= round_; i++, y += 3){
		rp = result + i;
		tmp = TIMEMAX - rp->timer;
		sprintf( strbuf, "ROUND %1d", i );
		tx_puts( 0, y, strbuf, BASENUM );
		if (commode[rp->win-1] == MODE_CPU) {
			sprintf( strbuf2, "COMPUTER WIN" );
		} else {
			sprintf( strbuf2, "PLAYER %d WIN", rp->win );
		}
		switch (rp->why) {
		  case KO:
			sprintf( strbuf, "       KO  %02d\"%02d  %s",
				overtime(tmp), undertime(tmp), strbuf2);
			winnum[rp->win-1]++;
			break;
		  case RINGOUT:
			sprintf( strbuf, "  RINGOUT  %02d\"%02d  %s",
				overtime(tmp), undertime(tmp), strbuf2);
			winnum[rp->win-1]++;
			break;
		  case TIMEOVER:
			sprintf( strbuf, " TIMEOVER         %s", strbuf2);
			winnum[rp->win-1]++;
			break;
		  case DOUBLEKO:
			sprintf( strbuf, " DOUBLE KO %02d\"%02d",
				overtime(tmp), undertime(tmp));
			break;
		  case DRAW:
			sprintf( strbuf, " DRAW GAME(TIMEOVER)          " );
				/*	  123456789012345678901234567890 30	*/
			break;
		}
		tx_puts( 2, y+1, strbuf, BASENUM );
	}
	if( winnum[0] > winnum[1] ) tw = 1;
	if( winnum[1] > winnum[0] ) tw = 2;

	if (tw) {
		tx_puts_big( 1, 23, "TOTAL WINNER IS", BIG_BASENUM);
				/* 123456789012345 */
		if (commode[tw-1] == MODE_CPU) {
			sprintf( strbuf2, "COMPUTER" );
		} else {
			sprintf( strbuf2 ,"PLAYER %d", tw );
		}
		tx_puts_big( 8, 26, strbuf2, BIG_BASENUM );
	} else {
		tx_puts_big( 1, 23, "   DRAW GAME   ", BIG_BASENUM);
	}
	if (tw) { commode[2-tw] = MODE_CPU;
	} else {
		commode[0] = MODE_CPU;
		commode[1] = MODE_CPU;
	}
	for (i = 0; i < 600; i++) {
		wait();
		for (j = 0; j < 2; j++) {
			s = JOY(j);
			if (s.flg.lb || s.flg.rb) {
				if(commode[j] == MODE_CPU && commode[1-j] == MODE_HUMAN){
					nmiflg = j + 1;
					return tw;
				}
				exitflg = 1;
			}
		}
		if (exitflg) break;
	}
	for (i = 0; i <= 32; i++){
		fade(i);
		if (i > 1 && i < 30) {
			tx_puts(0, i, "                                ", BASENUM);
		}
		wait();
	}
	return tw;
}

/*
 *		Display the winner and loser
 */

void	dispwin(int pl)
{
	int	i;
	result[round_].timer = timer; result[round_].win = pl;
	switch (pl) {
	  case 3:
		tx_puts_big(3, 7, "  DOUBLE KO  ", BIG_BASENUM);
		result[round_].why = DOUBLEKO;
		break;
	  case 4:
		tx_puts_big( 3, 7, "  DRAW GAME  ", BIG_BASENUM );
		result[round_].why = DRAW;
		break;
	  default:
		if( commode[pl - 1] == MODE_CPU ) {
			sprintf( strbuf, "COMPUTER WINS" );
		} else {
			sprintf( strbuf, "PLAYER %1d WINS", pl );
		}
		tx_puts_big( 3, 7, strbuf, BIG_BASENUM );
		if (timer <= 0) result[round_].why = TIMEOVER;
		else if (mandat[2-pl].dead_mode) result[round_].why = RINGOUT;
		else result[round_].why = KO;
	}
	manmove(0); manmove(1); idle();
	disp_status_init();
	for( i = 0; i < 120; i++ ) {
		manmove(0); manmove(1); idle();
		if (nmiflg) return;
	}
	for( i = 0; i <= 32; i++ ) {
		fade(i);
		manmove(0); manmove(1); idle();
		if (nmiflg) return;
	}
	tx_puts_big( 0, 7, "                ", BIG_BASENUM );	/* 16 spaces */
}



/*
 *		Load the Configuration File
 */

#define	GETSTR	do { fgets(strbuf, STRBUFSIZE, fp); } while (strbuf[0] == '#')

void	read_configuration( char *fname )
{
	FILE		*fp;
	config		*cnfp;
	int		i, j, k, l,m, loops;

	fp = fopen( fname, "r" );
	if (!fp) error("cannot open configuration file( in read_configuration ).");
	for( loops = 0; loops < CONFIGNUM; loops++ ){
		cnfp = &(cnf[ loops ]);
		m = cnfp->charge_max = 192;
		GETSTR;
		cnfp->energy_max = atoi(strbuf);
	/* read sword data */
		GETSTR;
		sscanf(strbuf, "%d %d %d", &i, &j, &k);
		cnfp->charge_to_sword = i;
		cnfp->d_min_sword = j;
		cnfp->mul_sword = (k - j) * CONFIG_MULDIV / (m - i);
	/* read laser data */
		GETSTR;
		sscanf(strbuf, "%d %d %d", &i, &j, &k);
		cnfp->charge_to_laser = i;
		cnfp->d_min_laser = j;
		cnfp->mul_laser = (k - j) * CONFIG_MULDIV / (m - i);
	/* read speed data */
		GETSTR;
		sscanf(strbuf, "%d %d %d", &i, &j, &k);
		cnfp->man_vspeed = i;
		cnfp->man_speed = j;
		cnfp->laser_speed = k;
		GETSTR;
		sscanf(strbuf, "%d %d %d %d", &i, &j, &k, &l);
		cnfp->inv_time = i;
		cnfp->wait_damage = j;
		cnfp->wait_laser = k;
		cnfp->wait_sword = l;
	}

	fclose( fp );
}

/*
 *		Displaying \___1p 2p___ etc.
 *		                       \
 */

void	make_playerdisp(void)
{
	man	*mp = mandat;
	int	x, y, tmp;
	/* 1p */
	tmp = 0;
	if (commode[0] == MODE_CPU) tmp = 0x40;
	x = mp->x;
	y = mp->y+ 16 * DIVX;
	make_simpleitem( x +  8 * DIVX, y, 120, 0x108+tmp );
	make_simpleitem( x + 24 * DIVX, y, 120, 0x109+tmp );
	make_simpleitem( x + 40 * DIVX, y, 120, 0x10a+tmp );
	mp++;
	/* 2p */
	tmp = 0;
	if (commode[1] == MODE_CPU) tmp = 0x40;
	x = mp->x;
	y = mp->y- 16 * DIVX;
	make_simpleitem( x - 40 * DIVX, y, 120, 0x10c+tmp );
	make_simpleitem( x - 24 * DIVX, y, 120, 0x10d+tmp );
	make_simpleitem( x -  8 * DIVX, y, 120, 0x10e + tmp );
}

/*
 *		STAGESTART!
 */

void	stagestart(int is1p, int stageno)
{
	int	i;

	fade(32);
	textscroll(0, 0);
	sprintf(strbuf, "ROUND%2d", round_);
	tx_puts_big(9 ,  8, strbuf, BIG_BASENUM);
	if (is1p) {
		sprintf(strbuf, " STAGE %2d ", stageno);
	} else {
		sprintf(strbuf, "BATTLE %3d", battle[0]+battle[1]);
	}
	tx_puts(11, 11, strbuf, BASENUM);
	tx_puts(0, 31, strbuf, BASENUM);
	if (!gamemode) {
		tx_puts(0, 30, "NORMAL  ", BASENUM);
		if (is1p) background(stageno + 1, 0);
		else background(rand_a() + 2, 0);
	} else {
		tx_puts(0, 30, "EXPERT  ", BASENUM);
		if (is1p) background(stageno + 1, rand_b() + 1);
		else background(rand_a() + 2, rand_b() + 1);
	}
	adjust_initial_location();
	make_playerdisp();
	for (i = 0; i < 120; i++) {
		if (i <= 32) fade(32 - i);
		if (i > 20) addcharge();
		if (i > 80) addcharge();
		idle();
		if (nmiflg && i > 32) return;
	}

	sd_on(SD_START);
	for (i = 0; i < 10; i++){
		tx_puts_big(11, 15, "ABCDE", BIG_BASENUM+0x20);
		idle();
		tx_puts_big(11, 15, "ABCDE", BIG_BASENUM+0x30);
		idle();
	}
	tx_puts(4, 11, "                        ", BASENUM);
	tx_puts_big( 9,  6, "       ", BIG_BASENUM);
	tx_puts_big( 9,  8, "       ", BIG_BASENUM);
	tx_puts_big(11, 15, "     "  , BIG_BASENUM);
}

void	rannyu(void)
{
	int	i, j, p;
	bgm(0);
	for(i = 0; i <= 96; i++) {
		if (i & 4) { p = 0x80; } else {
			p = ((nmiflg == 1) ? 0 : 0x40); }
		for(j = 0; j < 20; j++) {
			tx_putc(7 + j, 14, 0x240 + j * 2 + p);
			tx_putc(7 + j, 15, 0x241 + j * 2 + p);
		}
		wait();
		if( i >= 64 ) fade( i- 64);
	}
	sprite_clear();
	for(i = 2; i <= 29; i++){
		tx_puts(0, i, "                                ", BASENUM);
	}
	commode[0] = commode[1] = MODE_HUMAN;
	gamemode = select_();
}

void	main( int argc, char **argv )
{
	int	win, overcount;
	int	is1p, tmp, stageno;

	puts("StarCharge version 0.21(22th Feb. 1995)   Programmed by chika@kmc\n");
	if (argc != 2) {
		puts( "Usage: battle [picloadpath]");
		puts( "       configuration file 'battle.cnf' is needed." );
		exit(1);
	}
	pic_loadpath = argv[1];

	read_configuration("battle.cnf");

_CHARGE:
	B_SUPER(0);
	sd_init();
	bginit();
	crtinit();
	initstage();
	timeinit();
	textinit( "text.dat" );
	textclear();
	bgmake();

	while( -1 ){
		title();
		textclear();
		stageno = 1;
battlestart:
		nmiflg = 0;
		is1p = (commode[0] == MODE_CPU || commode[1] == MODE_CPU);
		bgm(1);
		if (!is1p) nowcnf = cnf + gamemode; else nowcnf = cnf;
		if (!is1p) battle[gamemode]++; else battle[CONFIGNUM]++;
		round_ = 0;
		mandat[0].life = mandat[1].life = LIFE;
		textclear();
		bgmake();

		do {
			count = 0;
			timer = TIMEMAX;
			round_++;
			maninit();
			iteminit();
			cpuinit(stageno);
			disp_status_init();
			stagestart(is1p, stageno);
			if (nmiflg) { rannyu(); goto battlestart; }
			overcount = win = 0;
			enableshot = 1;
			while( -1 ){
				count++;
				if (enableshot) timer--;
				manmove(0);
				manmove(1);
				if (nmiflg) { rannyu(); goto battlestart; }
				idle();
				if (mandat[1].energy <= 0) {
					win |= 1; overcount++;
					enableshot = 0;
				}
				if (mandat[0].energy <= 0) {
					win |= 2; overcount++;
					enableshot = 0;
				}
				if (overcount > 120) break;
				if (timer == 0) {
					overcount += 2;
					if( enableshot) {
						tx_puts_big(7, 7, "TIME OVER", BIG_BASENUM);
						if (mandat[0].energy >= mandat[1].energy)
							win |= 1;
						if (mandat[1].energy >= mandat[0].energy)
							win |= 2;
						if (win == 3) win = 4;
							else mandat[2-win].life--;
					}
					enableshot = 0;
				}
			}
			dispwin( win );
			if (nmiflg) { rannyu(); goto battlestart; }
		}while ( mandat[1].life > 0 && mandat[0].life > 0 && round_ < ROUNDMAX);
		bgm_fade(32);
		tmp = dispresult();
		if (nmiflg) { rannyu(); goto battlestart; }
		if (tmp){
			if (commode[tmp-1] == MODE_HUMAN && is1p) {
				stageno++;
				if (stageno != 9) goto battlestart;
				ending();
			} else {
				gamemode = 0;
				if (commode[tmp-1] == MODE_HUMAN) goto battlestart;
			}
		}
	}
}

