/*	enemy.h : enemy move program	*/

#define		HAHEN_NUM		64	/*	Maximum number of exploded fragments	*/
#define		EN_CRASH_TIME		8
#define		BOSS_CRASH_TIME		12
#define		ME_SHOT_CRASH_TIME	12
#define		ME_BODY_CRASH_TIME	24
#define		ENEMY_SPEED_DIV		16
#define		BOSS_MODEL		24
#define		ME_ZONE			64
#define		LASER_DAMAGE		8
#define		ENEMY_DAMAGE		16

typedef	struct rot_m {
	unsigned char	x;
	unsigned char	y;
	unsigned char	z;
} ROT_M;

typedef	struct rot_d {
	unsigned char	x;
	unsigned char	y;
} ROT_D;

typedef	struct pos {
	short	x;
	short	y;
	short	z;
} POS;

typedef	struct boxenemy {
	struct boxenemy	*n;
	short		x;
	short		y;
	short		z;
} BOXENEMY;

typedef struct	shottl	{
	short	kind;
	short	count;
} SHOTTL;

typedef struct	borntl	{
	short	mod_k;
	short	mot_k;
	short	count;
	short	sign;
	short	flag;	/* 0 --- reference pos only, 1 --- reference pos & rot */
} BORNTL;

typedef	struct	motion	{
	short		count;		/*2*/
	ROT_M		rot;		/*3*/
	short		veloc;		/*2*/
	ROT_D		vec;		/*2*/
	POS		pos;		/*6*/
	SHOTTL		shot;		/*4*/
	BORNTL		born;		/*10*/		/* 29 */
	struct motion	*n;
} MOTION;

typedef	struct enemy {
	struct enemy	*n;
	struct enemy	*p;
	BOXENEMY	*chain_boxenemy;
	int		mod_kind;
	int		mot_kind;
	POS		pos;
	ROT_M		rot_m;	/*	Shape direction	*/
	ROT_D		rot_d;	/*	Direction of movement	*/
	ROT_M		old_rot_m;
	ROT_D		old_rot_d;
	int		speed;
	int		old_speed;
	POS		delta;
	int		crash;
	int		hit_point;
	int		score;
	MOTION		*step;
	int		counter;
	int		sign_x;
	int		sign_y;
} ENEMY;

typedef	struct hahen {
	int	stat;	/*	0 --- nothing	1 --- exist	*/
	int	x;
	int	y;
	int	z;
	int	dx;
	int	dy;
	int	dz;
} HAHEN;

extern	ENEMY	*list_enemy;
extern	HAHEN	boon[];
extern	int	boon_que;
extern	int	z_scroll[];
extern	int	boss_hp, boss_appear_flag, final_boss_flag, count_after_boss;
extern	int	force_appear_from_setbox;
extern	POS	force_pos;

void	BornEnemy(int, int, int, int, int, short, short, short, unsigned char, unsigned char, unsigned char);
void	InitEnemy(void);
void	InitStageEnemy(void);
void	Enemy(void);
void	SweepEnemy(void);
