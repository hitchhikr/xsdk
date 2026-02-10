/*	enemy.c : enemy operation program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"main.h"
#include	"enemy.h"
#include	"model.h"
#include	"dim.h"
#include	"motion.h"
#include	"matrix.h"
#include	"calc.h"
#include	"sound.h"

#define	ENEMY_MAX	64	/*	Maximum number of enemy buffers	*/
#define	BOX_MAX		512	/*	Maximum number of box buffers	*/

#define	LOCATE(x,y)	printf("\033[%d;%dH",y,x)
#define	ABS(c)		((c) > 0 ? (c) : -(c))

#define	G		2
#define	G2		4

ENEMY		*list_enemy;
HAHEN		boon[HAHEN_NUM];
int		boon_que = 0;		/* Fragment buffer queue	*/
static ENEMY	*rest_enemy;		/* Buffers that are not currently in use	*/
static ENEMY	*last_enemy;		/* Point to list_enemy's butt	*/
BOXENEMY	*rest_boxenemy;		/* Currently unused box buffers	*/
int		z_scroll[STAGEMAX] = {0, 16, 0, 24, 0, 8, 32, 0, 0};
const int	boss_hp_origin[STAGEMAX] = {46, 36, 32, 40, 64, 16, 32, 0, 0};
int		boss_hp, boss_appear_flag, final_boss_flag, count_after_boss;
int		force_appear_from_setbox;
POS		force_pos;

#if 0
/*	for debug	*/
void	PrintNumberofRestboxenemy(void)
{
	int		num;
	BOXENEMY	*trace = rest_boxenemy;

	LOCATE(4, 4);
	if (NULL == trace) {
		puts("  0");
		return;
	}
	num = 1;
	while (NULL != (trace = trace->n)) {
		++num;
	}
	printf("%3d", num);
}

void	PrintNumberofRestenemy(void)
{
	int		num;
	ENEMY	*trace = list_enemy;

	LOCATE(4, 3);
	if (NULL == trace) {
		puts("  0");
		return;
	}
	num = 1;
	while (NULL != (trace = trace->n)) {
		++num;
	}
	printf("%3d", num);
}
#endif

void	MkModel(ENEMY *enemy)
{
	BOXMODEL	*md = model[ enemy->mod_kind ].chain_boxmodel;
	BOXENEMY	*em;

	(enemy->old_rot_m).x = (enemy->old_rot_m).y = (enemy->old_rot_m).z = 0;
	(enemy->old_rot_d).x = (enemy->old_rot_d).y = 0;
	(enemy->delta).x = (enemy->delta).y = (enemy->delta).z = 0;
	enemy->old_speed = 0;
	enemy->crash = 0;

	enemy->hit_point = model[ enemy->mod_kind ].hit_point;
	em = enemy->chain_boxenemy = rest_boxenemy;
	rest_boxenemy = rest_boxenemy->n;
	for (;;) {
		em->x = md->x;
		em->y = md->y;
		em->z = md->z;
		if ( NULL == (md = md->n) ) break;
		em = em->n;
		rest_boxenemy = rest_boxenemy->n;
	}
	em->n = NULL;
}

void	BornEnemy(int model_kind, int motion_kind, int sign_x, int sign_y, int flag, 
                  short x, short y, short z, unsigned char rx, unsigned char ry, unsigned char rz)
{
	if (NULL == last_enemy) {	/*	No enemies have appeared yet	*/
		list_enemy = last_enemy = rest_enemy;
		rest_enemy = rest_enemy->n;
		last_enemy->p = NULL;
		last_enemy->n = NULL;
		rest_enemy->p = NULL;
	} else {
		last_enemy->n = rest_enemy;
		rest_enemy->p = last_enemy;
		last_enemy = last_enemy->n;
		rest_enemy = rest_enemy->n;
		last_enemy->n = NULL;
		rest_enemy->p = NULL;
	}
	last_enemy->mod_kind = model_kind;
	last_enemy->mot_kind = motion_kind;
	last_enemy->sign_x = sign_x;
	last_enemy->sign_y = sign_y;
	last_enemy->score = model[ model_kind ].score;
	MkModel(last_enemy);
	AppearEnemy(last_enemy, flag, x, y, z, rx, ry, rz);
}

void	InitEnemy(void)
{
	int		i;
	ENEMY		*now_enemy;
	BOXENEMY	*now_box;

	list_enemy = last_enemy = NULL;
	if (NULL == ( now_enemy = rest_enemy = (ENEMY *)malloc(sizeof(ENEMY)) )) {
		fputs("enemy malloc out of memory\n", stdout);
		exit(1);
	}
	now_enemy->p = NULL;
	for (i = 1; i < ENEMY_MAX; ++i) {
		if (NULL == ( now_enemy->n = (ENEMY *)malloc(sizeof(ENEMY)) )) {
			fputs("enemy malloc out of memory\n", stdout);
			exit(1);
		}
		(now_enemy->n)->p = now_enemy;
		now_enemy = now_enemy->n;
	}
	now_enemy->n = NULL;
	
	if (NULL == ( now_box = rest_boxenemy = (BOXENEMY *)malloc(sizeof(BOXENEMY)) )) {
		fputs("boxenemy malloc out of memory\n", stdout);
		exit(1);
	}
	for (i = 1; i < BOX_MAX; ++i) {
		if (NULL == ( now_box->n = (BOXENEMY *)malloc(sizeof(BOXENEMY)) )) {
			fputs("boxenemy malloc out of memory\n", stdout);
			exit(1);
		}
		now_box = now_box->n;
	}
	now_box->n = NULL;
}

void	InitStageEnemy(void)
{
	int	i;
	
	for (i = 0; i < HAHEN_NUM; ++i) boon[i].stat = 0;

	boss_hp = boss_hp_origin[round_ - 1];
	final_boss_flag = boss_appear_flag = count_after_boss = 0;
}

void	MoveEnemy(ENEMY *em)
{
	short	cx = 0, cy = 0, cz = em->speed;

	if ( (LASER != em->mot_kind) && ( (em->rot_d).x != (em->old_rot_d).x || 
         (em->rot_d).y != (em->old_rot_d).y || em->speed != em->old_speed ) ) {
		RoundXY(&cx, &cy, &cz, (em->rot_d).x, (em->rot_d).y);
		(em->old_rot_d).x = (em->rot_d).x;
		(em->old_rot_d).y = (em->rot_d).y;
		em->old_speed = em->speed;
		(em->delta).x = cx;
		(em->delta).y = cy;
		(em->delta).z = cz;
	}
	(em->pos).x += (em->delta).x / ENEMY_SPEED_DIV;
	(em->pos).y += (em->delta).y / ENEMY_SPEED_DIV;
	
	switch (round_) {
		case	7:
			if (!boss_appear_flag || 1 == em->mod_kind) {
				(em->pos).z += (em->delta).z / ENEMY_SPEED_DIV + z_scroll[ round_ - 1 ];
			} else {
				(em->pos).z += (em->delta).z / ENEMY_SPEED_DIV;
			}
			break;
		default:
			(em->pos).z += (em->delta).z / ENEMY_SPEED_DIV + z_scroll[ round_ - 1 ];
			break;
	}
}

void	DeleteEnemy(ENEMY *target)
{
	BOXENEMY	*trace = target->chain_boxenemy;
	
	if ( NULL == target->p ) {
		if ( NULL == target->n ) {
			list_enemy = last_enemy = NULL;
		} else {
			list_enemy = list_enemy->n;
			list_enemy->p = NULL;
		}
	} else {
		if ( NULL == target->n ) {
			(target->p)->n = NULL;
			last_enemy = target->p;
		} else {
			(target->p)->n = target->n;
			(target->n)->p = target->p;
		}
	}
	while ( NULL != trace->n ) trace = trace->n;
	trace->n = rest_boxenemy;
	rest_boxenemy = target->chain_boxenemy;
	target->n = rest_enemy;
	rest_enemy->p = target;
	target->p = NULL;
	rest_enemy = target;
}

void	DeleteCheck(ENEMY *target)
{
	if (0 == target->hit_point && EN_CRASH_TIME - 1 == target->crash) {	/*	Destroyed!	*/
		if (0 == boss_appear_flag || BOSS_MODEL == target->mod_kind) score += target->score;
		DeleteEnemy(target);
		return;
	}
	if ( ABS( (target->pos).x ) > LIMIT_XY || ABS( (target->pos).y ) > LIMIT_XY || ABS( (target->pos).z ) > LIMIT_Z ) {
		DeleteEnemy(target);
	}
}

void	MoveBoon(void)
{
	int	i;

	for (i = 0; i < HAHEN_NUM; ++i) {
		if (0 == boon[i].stat) continue;
		
		boon[i].x += boon[i].dx;
		boon[i].y += boon[i].dy;
		boon[i].z += boon[i].dz;
		--boon[i].stat;
		switch (round_) {
			case	2:
				boon[i].dy += G2;
				if (boon[i].y > GROUND) boon[i].stat = 0;
				break;
			case	4:
				boon[i].dy += G;
				if (boon[i].y < TENJOU) boon[i].stat = 0;
				break;
			case	6:
				boon[i].dy -= G;
				if (boon[i].y > SEEBOT || boon[i].y < SEETOP) boon[i].stat = 0;
				break;
		}
	}
}

void	Enemy(void)
{
	ENEMY	*now_enemy = list_enemy, *target;

	while (NULL != now_enemy) {
		DoMotion(now_enemy);
		MoveEnemy(now_enemy);
		now_enemy = (target = now_enemy)->n;
		DeleteCheck(target);
	}
	MoveBoon();
	
	if (count_after_boss) ++count_after_boss;
	
	/* for debug
	PrintNumberofRestenemy();
	PrintNumberofRestboxenemy();
	*/
}

void	SweepEnemy(void)
{
	ENEMY	*now_enemy = list_enemy, *target;

	while (NULL != (target = now_enemy)) {
		now_enemy = now_enemy->n;
		DeleteEnemy(target);
	}
}
