/*	setbox.c : prepare display program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	"main.h"
#include	"matrix.h"
#include	"calc.h"
#include	"star.h"
#include	"enemy.h"
#include	"model.h"
#include	"dim.h"
#include	"sound.h"
#include	"motion.h"

#define		BOON_TIME		16
#define		BOSS_BOON_TIME		32
#define		FINAL_BOSS_BOON_TIME	64
#define		PRW			1
#define		HEAD			0
#define		CHAIN_LENGTH		128
#define		EXIST			1
#define		NOTHING			0
#define		ABS(c)			((c) > 0 ? (c) : -(c))
#define		LOCATE(x,y)		printf("\033[%d;%dH",y,x)

#define	MARU		0x4f
#define	SHIKAKU		0x3f

typedef struct spdat {
	short	x, y, pat, prw;
} SPDAT;

SPDAT	spbuf[ HEAD + CHAIN_LENGTH ];

static BOXBUF	*chain_boxbuf;
static BOXBUF	*now_boxbuf;
static BOXBUF	*keep_boxbuf;
static BOXBUF	*rest_boxbuf;
static BOXBUF	*hash[HASH_SIZE + 1];

#if 0
/*	Debug count routine 1	*/
int	CountBox(BOXBUF	*target_boxbuf)
{
	BOXBUF	*count_boxbuf = target_boxbuf;
	int	num = 0;
	
	if (NULL == count_boxbuf) return num;
	++num;
	for (;;) {
		if ( NULL != count_boxbuf->n ) {
			++num;
			count_boxbuf = count_boxbuf->n;
		} else {
			break;
		}
	}
	return num;
}
/*	Debug count routine 2	*/
int	CountEnemy(ENEMY	*target)
{
	ENEMY	*count = target;
	int	num = 0;
	
	if (NULL == count) return num;
	++num;
	for (;;) {
		if ( NULL != count->n ) {
			++num;
			count = count->n;
		} else {
			break;
		}
	}
	return num;
}
#endif

void	DestroyEnemy(void)
{
	ENEMY	*target = list_enemy;
	
	while (NULL != target) {
		target->hit_point = 0;
		target->crash = EN_CRASH_TIME;
		target = target->n;
	}
	return;
}

void	DestroyHebi(void)
{
	ENEMY	*target = list_enemy;
	int	count = 0;
	
	while (NULL != target) {
		if (27 == target->mod_kind) {
			count += 4;
			target->hit_point = 0;
			target->crash = EN_CRASH_TIME + count;
		}
		target = target->n;
	}
	return;
}

void	InitSetBox(void)
{
	BOXBUF	*boxbuf_ptr;
	int	i;

	if (NULL == ( boxbuf_ptr = (BOXBUF *)malloc(sizeof(BOXBUF)) )) {
		fputs("boxbuf malloc out of memory\n", stdout);
		exit(1);
	}
	chain_boxbuf = boxbuf_ptr;
	for (i = 1; i < CHAIN_LENGTH + 1; ++i) {	/*last one box won't be used*/
		if (NULL == ( boxbuf_ptr->n = (BOXBUF *)malloc(sizeof(BOXBUF)) )) {
			fputs("boxbuf malloc out of memory\n", stdout);
			exit(1);
		}
		boxbuf_ptr = boxbuf_ptr->n;
	}
	boxbuf_ptr->n = NULL;	/*	butt	*/
	/*	Initialize the buffer	*/
	for (i = 0; i < HEAD + CHAIN_LENGTH; ++i) {
		spbuf[i].x = 0;
		spbuf[i].prw = PRW;
	}
}

int	EnemyToBox(ENEMY *em)
{
	BOXMODEL	*md = model[ em->mod_kind ].chain_boxmodel;
	BOXENEMY	*enmd = em->chain_boxenemy;
	BOXBUF		*pre_boxbuf = NULL;
	ROT_M		*rot = &em->rot_m, *old_rot = &em->old_rot_m;
	int		i, pat, flag = 0, keep_hp, laser_flag;
	
	if (em->crash) --em->crash;
	keep_hp = em->crash;
	if ( laser_flag = model[ em->mod_kind ].pat ) {
		pat = MARU;
	} else if (keep_hp) {
		pat = SHIKAKU + 0xc00 * (em->counter & 1);
	} else {
		pat = SHIKAKU;
	}
	if ( rot->z != old_rot->z || rot->x != old_rot->x || rot->y != old_rot->y ) {
		flag = 1;
		old_rot->z = rot->z;	old_rot->x = rot->x;	old_rot->y = rot->y;
	}
	
	for (;;) {
		int	x, y, z;
		
		if (flag) {
			enmd->x = md->x;	enmd->y = md->y;	enmd->z = md->z;
			RoundZXY( &enmd->x, &enmd->y, &enmd->z, rot->x, rot->y, rot->z );
		}
		/*	Coordinate calculation	*/
		now_boxbuf->x = (x = (em->pos).x + enmd->x) - me.x;
		now_boxbuf->y = (y = (em->pos).y + enmd->y) - me.y;
		now_boxbuf->z = (z = (em->pos).z + enmd->z);
		/*	Can I convert it? */
		if ( (ABS(now_boxbuf->x) > HALF) || (ABS(now_boxbuf->y) > HALF) || z < -HALF ) {
			goto next_box;
		}
		/*	Ground & ceiling detection	*/
		switch (round_) {
			case	2:
				if (y > GROUND) goto next_box;
				break;
			case	4:
				if (y < TENJOU) goto next_box;
				break;
			case	6:
				if (y > SEEBOT || y < SEETOP) goto next_box;
				break;
			case	7:
				if (x > KABE || x < -KABE) goto next_box;
				break;
		}
		/*	Hit detection	*/
		if (laser_flag) {
			if (0 == me.crash && z > HALF && z < HALF + ME_ZONE && x > me.x - ME_ZONE && x < me.x + ME_ZONE && y > me.y - ME_ZONE && y < me.y + ME_ZONE) {
				em->crash = EN_CRASH_TIME - 1;	/* laser not pow */
				em->hit_point = 0;
				bokan = me.crash = ME_SHOT_CRASH_TIME;
				me.shield -= LASER_DAMAGE;
				PlaySE(SE_DAMAGE);
			}
		} else if (0 == em->crash && !ending_flag) {
			if (0 == me.crash && z > HALF - ME_ZONE && z < HALF + ME_ZONE && x > me.x - ME_ZONE && x < me.x + ME_ZONE && y > me.y - ME_ZONE && y < me.y + ME_ZONE) {
				if (BOSS_MODEL > em->mod_kind) {
					em->crash = EN_CRASH_TIME;
					--em->hit_point;
				}
				bokan = me.crash = ME_BODY_CRASH_TIME;
				me.shield -= ENEMY_DAMAGE;
				PlaySE(SE_DAMAGE);
			}
			for (i = 0; i < SHOT_NUM; ++i) {
				if (0 == shot[i].stat || z > shot[i].z + 128 || z < shot[i].z - 128 || x > shot[i].x + 48 || x < shot[i].x - 48 || y > shot[i].y + 48 || y < shot[i].y - 48) continue;
				if (BOSS_MODEL == em->mod_kind) {
					if (--boss_hp) {
						PlaySE(SE_HIT);
					} else {
						DestroyEnemy();
						PlaySE(SE_DESTROY);
						count_after_boss = 1;
					}
					em->crash = BOSS_CRASH_TIME;
					shot[i].stat = 0;
				} else if (BOSS_MODEL > em->mod_kind) {
					if (--em->hit_point) {
						PlaySE(SE_HIT);
					} else {
						PlaySE(SE_DESTROY);
					}
					em->crash = EN_CRASH_TIME;
					shot[i].stat = 0;
				}
				switch (round_) {
					case	3:
						if (25 == em->mod_kind) {
							PlaySE(SE_REFLECT);
							shot[i].dz = 64;
							shot[i].z = z + 144;
						}
						break;
					case	6:
						if (3 == em->mod_kind && 0 == em->hit_point) DestroyHebi();
						break;
					case	7:
						if (25 == em->mod_kind || 26 == em->mod_kind) {
							PlaySE(SE_REFLECT);
							shot[i].dz = 64;
							shot[i].z = z + 144;
						}
						break;
				}
				break;
			}
		} else if (0 == em->hit_point && em->crash < EN_CRASH_TIME) {/*	explosion!	*/
			if (BOSS_MODEL <= em->mod_kind) {
				boon[boon_que].stat = BOSS_BOON_TIME;
				switch (round_) {
					case	3:
						if (BOSS_MODEL == em->mod_kind) {
							boon[boon_que].stat = 0;
							force_appear_from_setbox = 3;
							force_pos = em->pos;
						}
						break;
					case	4:
						if (BOSS_MODEL == em->mod_kind) {
							boon[boon_que].stat = 0;
							force_appear_from_setbox = 4;
							force_pos = em->pos;
						}
						break;
					case	7:
						if (BOSS_MODEL == em->mod_kind) {
							if (!final_boss_flag) {
								boon[boon_que].stat = 0;
								force_appear_from_setbox = 7;
								force_pos = em->pos;
							} else {
								boon[boon_que].stat = FINAL_BOSS_BOON_TIME;
							}
						}
						break;
				}
			} else {
				boon[boon_que].stat = BOON_TIME;
			}
			/*	Explosion Handling	*/
			boon[boon_que].x = x;
			boon[boon_que].y = y;
			boon[boon_que].z = z;
			boon[boon_que].dx = (rand() & 62) - 31;
			if (2 == round_) {
				boon[boon_que].dy = (rand() & 62) - 47;
			} else {
				boon[boon_que].dy = (rand() & 62) - 31;
			}
			boon[boon_que].dz = (rand() & 62) - 31;
			boon_que = ++boon_que % HAHEN_NUM;
		}
		if (z > HALF) goto next_box;
		/*	Conversion to real surface	*/
		F3Dto2D(now_boxbuf);
		/*	Can you display it? */
		if ( now_boxbuf->rx < 0 || now_boxbuf->ry < 0 || now_boxbuf->rx > 272 || now_boxbuf->ry > 272 ) {
			goto next_box;
		}
		if (keep_hp && (em->counter & 1)) {
			now_boxbuf->pat = pat + now_boxbuf->box;
		} else {
			now_boxbuf->pat = pat + md->col * 0x100 + now_boxbuf->box;
		}
		/*	Do you still have any left?	*/
		if ( NULL != now_boxbuf->n ) {
			pre_boxbuf = now_boxbuf;
			now_boxbuf = now_boxbuf->n;
		}
next_box:
		if ( NULL == (md = md->n) ) break;
		enmd = enmd->n;
	}
	if (NULL != pre_boxbuf) {
		now_boxbuf = pre_boxbuf;
		return EXIST;
	} else {
		return NOTHING;
	}
}

int	ShotToBox(void)
{
	int	i, flag = NOTHING;

	for (i = 0; i < SHOT_NUM; ++i) {	/* Show Shots */
		if (shot[i].stat) {
			now_boxbuf->x = shot[i].x - me.x;
			now_boxbuf->y = shot[i].y - me.y;
			now_boxbuf->z = shot[i].z;
			if ( (ABS(now_boxbuf->x) > HALF) || (ABS(now_boxbuf->y) > HALF) || (ABS(now_boxbuf->z) > HALF) ) {
				continue;
			}
			F3Dto2D(now_boxbuf);
			if ( now_boxbuf->rx < 0 || now_boxbuf->ry < 0 || now_boxbuf->rx > 272 || now_boxbuf->ry > 272 ) {
				continue;
			}
			now_boxbuf->pat = 0x64f + now_boxbuf->box;
			keep_boxbuf = now_boxbuf;
			now_boxbuf = now_boxbuf->n;
			flag = EXIST;
		}
	}
	for (i = 0; i < HAHEN_NUM; ++i) {	/* Explosion Display */
		if (boon[i].stat) {
			now_boxbuf->x = boon[i].x - me.x;
			now_boxbuf->y = boon[i].y - me.y;
			now_boxbuf->z = boon[i].z;
			if ( (ABS(now_boxbuf->x) > HALF) || (ABS(now_boxbuf->y) > HALF) || (ABS(now_boxbuf->z) > HALF) ) {
				continue;
			}
			F3Dto2D(now_boxbuf);
			if ( now_boxbuf->rx < 0 || now_boxbuf->ry < 0 || now_boxbuf->rx > 272 || now_boxbuf->ry > 272 ) {
				continue;
			}
			now_boxbuf->pat = 0x0a4f + now_boxbuf->box;
			keep_boxbuf = now_boxbuf;
			now_boxbuf = now_boxbuf->n;
			flag = EXIST;
		}
	}
	return flag;
}

void	SetBoxBuf(void)
{
	static	int	jun = 1;	/* 1 or 0: Set order reversal flag */
	ENEMY	*now_enemy = list_enemy;
	int	object, shot_flg, flg = NOTHING;
	
	if (me.crash) --me.crash;	/*	Reduce invincible time	*/
	if (muteki) me.crash = 1;	/*	Invincibility judgment	*/
	
	now_boxbuf = chain_boxbuf;	/*	Initialization	*/
	
	shot_flg = ShotToBox();
	
	if (NULL == now_enemy) {	/*	No enemies	*/
		if (NOTHING == shot_flg) {
			rest_boxbuf = chain_boxbuf;
			chain_boxbuf = NULL;
			return;
		} else {
			rest_boxbuf = keep_boxbuf->n;
			keep_boxbuf->n = NULL;
			return;
		}
	}
	/*	Change the setting order	*/
	if ( (jun = 1 - jun) ) while ( NULL != now_enemy->n ) now_enemy = now_enemy->n;
	for (;;) {
		if ( EXIST == (object = EnemyToBox(now_enemy)) ) flg = EXIST;
		if (jun) {
			if ( NULL == (now_enemy = now_enemy->p) ) break;
		} else {
			if ( NULL == (now_enemy = now_enemy->n) ) break;
		}
		if (EXIST == object) {
			keep_boxbuf = now_boxbuf;
			if (NULL != now_boxbuf->n) now_boxbuf = now_boxbuf->n;
		}
	}
	/*	Process the remainder */
	if (EXIST == flg || EXIST == shot_flg) {
		if (NOTHING == object) now_boxbuf = keep_boxbuf;
		rest_boxbuf = now_boxbuf->n;
		now_boxbuf->n = NULL;
	} else {
		rest_boxbuf = chain_boxbuf;
		chain_boxbuf = NULL;
	}
	/*	for debug
	LOCATE(1, 6);	printf( "buf  = %3d", CountBox(chain_boxbuf) );
	LOCATE(1, 7);	printf( "rest = %3d", CountBox(rest_boxbuf) );
	*/
}

void	SetHash(void)
{
	int	h, value /*debug , count[HASH_SIZE+1] = {0}*/;
	BOXBUF	*trace_boxbuf = chain_boxbuf;
	BOXBUF	*move_boxbuf = chain_boxbuf;
	
	for (h = HASH_SIZE; h >= 0; --h) {
		hash[h] = NULL;	/*	Initialization	*/
	}
	if (NULL == chain_boxbuf) return;	/*	No enemies	*/
	while (NULL != move_boxbuf) {
		trace_boxbuf = trace_boxbuf->n;
		value = (move_boxbuf->z + HALF) / HASH_RATE;
		/*debug  count[value]++;*/
		if ( NULL == hash[ value ] ) { /*	There's nothing in it	*/
			hash[ value ] = move_boxbuf;
			move_boxbuf->n = NULL;
		} else {	/*	If it is there, sort it and add it	*/
			BOXBUF	*search_hash = hash[ value ];
			BOXBUF	*pre_search_hash = NULL;
			
			for (;;) {
				if ( move_boxbuf->z > search_hash->z ) {
					if ( NULL == pre_search_hash ) {
						hash[ value ] = move_boxbuf;
						move_boxbuf->n = search_hash;
						break;
					} else {
						pre_search_hash->n = move_boxbuf;
						move_boxbuf->n = search_hash;
						break;
					}
				}
				if ( NULL == search_hash->n ) {
					search_hash->n = move_boxbuf;
					move_boxbuf->n = NULL;
					break;
				}
				pre_search_hash = search_hash;
				search_hash = search_hash->n;
			}
		}
		move_boxbuf = trace_boxbuf;
	}
	chain_boxbuf = NULL;	/*	Movement complete	*/
	/*	for debug
	LOCATE(0, 0);
	for (h = 0; h <= HASH_SIZE; ++h) {
		printf("%3d ", count[h]);
	}
	*/
}

void	SetSpBuf(void)
{
	int	h, count = HEAD;
	BOXBUF	*now_spbuf;
	SPDAT	*sp_ptr = spbuf + HEAD;

	now_spbuf = NULL;		/* Initialization */
	for (h = HASH_SIZE; h >= 0; --h) { /* Scrape from an array of struct pointers */
		BOXBUF	*search_boxbuf = hash[h];

		if (NULL == search_boxbuf) continue;
		if (NULL == now_spbuf) {
			chain_boxbuf = now_spbuf = search_boxbuf;
			sp_ptr->x	= now_spbuf->rx;
			sp_ptr->y	= now_spbuf->ry;
			sp_ptr->pat	= now_spbuf->pat;
			++sp_ptr;	++count;
		} else {
			now_spbuf = now_spbuf->n = search_boxbuf;
			sp_ptr->x	= now_spbuf->rx;
			sp_ptr->y	= now_spbuf->ry;
			sp_ptr->pat	= now_spbuf->pat;
			++sp_ptr;	++count;
		}
		while (NULL != now_spbuf->n) {
			now_spbuf = now_spbuf->n;
			sp_ptr->x	= now_spbuf->rx;
			sp_ptr->y	= now_spbuf->ry;
			sp_ptr->pat	= now_spbuf->pat;
			++sp_ptr;	++count;
		}
	}/*	The butt is already there	*/
	while (count++ < CHAIN_LENGTH + HEAD) {	/* Erase the rest */
		sp_ptr->x = 0;		/* erase */
		++sp_ptr;
	}
}

void	ReturnBox(void)
{
	BOXBUF	*look_boxbuf;
	
	if (NULL == chain_boxbuf) {
		chain_boxbuf = rest_boxbuf;
		return;
	}
	for (look_boxbuf = chain_boxbuf; NULL != look_boxbuf->n; look_boxbuf = look_boxbuf->n);
	look_boxbuf->n = rest_boxbuf;
	/*	for debug
	LOCATE(1, 8);	printf( "all = %3d", CountBox(chain_boxbuf) );
	*/
}

void	SetBox(void)
{
	SetBoxBuf();
	SetHash();
	SetSpBuf();
	ReturnBox();
	/*	for debug
	LOCATE(0, 0);	printf("%2d", CountEnemy(list_enemy));
	LOCATE(0, 2);	printf("%2d", CountEnemy(rest_enemy));
	*/
}
