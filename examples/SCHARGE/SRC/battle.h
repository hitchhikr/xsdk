#ifndef	_battle_h_
#define	_battle_h_

#define	DIVX	16
#define	DIVY	DIVX

#define	XMIN	-256
#define	XMAX	(3840+256)
#define	YMIN	0
#define	YMAX	3840
#define	SX0	16
#define	SY0	16
#define	MXMIN	0
#define	MXMAX	3840
#define	MYMIN	256
#define	MYMAX	3584

#define	SCX(x)	( (x) / DIVX + SX0 )
#define	SCY(y)	( (y) / DIVY + SY0 )


#define	DISABLE		0
#define	ABLE		1
#define	PLAYERNUM	2

/*
    The power of a config structure attack is calculated as follows:
    ((charge - cnf.charge_to_sword) * cnf.mul_sword / CONFIG_MULDIV)
 */

typedef struct	_cnf {
	int	energy_max;
	int	charge_max;
	int	charge_to_sword;
	int	charge_to_laser;
/*	About damage		*/
	int	d_min_sword;		/* Minimum punch power */
	int	d_min_laser;		/* Minimum laser power */
	int	mul_sword;
	int	mul_laser;
/*	About speed	*/
	int	man_vspeed;	/*	Speed of your aircraft		*/
	int	man_speed;	/*	Your ship's speed (max)	*/
	int	laser_speed;	/*	Laser Speed	*/
	int	inv_time;	/*	Invincible time after being attacked	*/
	int	wait_damage;	/*	Stiffness time after being attacked	*/
	int	wait_laser;	/*	Stiffness time caused by laser firing	*/
	int	wait_sword;	/*	Stiffness time due to hit		*/
} config;
#define	CONFIG_MULDIV	1024

/*
 *		Own structure
 */

typedef struct	_man {
	short	cond;
	short	Random;
	short	x, y;
	short	vx, vy;			/* Speed when thrown */
	short	dead_mode;		/* Death flg */
	short	dir;
	short	charge;
	char	lbflg, rbflg;
	short	energy;
	short	hit;
	short	hitx, hity;
	short	hitdir;
	short	life;
} man;

/*
 *		Laser Structure
 */

#define	SWORD	0
#define	LASER	1

typedef struct	_laser {
	short	cond;
	short	type;
	short	x, y;
	short	vx, vy;
	short	energy;
	short	dir;
} laser;

/*
 *		Trigonometric functions
 */
#define	SINDIV	256

extern	short	_sin[];
#define	COS( dir )	( _sin[(dir) & 7] )
#define	SIN( dir )	( _sin[((dir)+6) & 7] )
#define	ABS( x )	(((x)>=0) ? (x): -(x))
#define	MODE_HUMAN	1
#define	MODE_CPU	2

extern	man	mandat[ PLAYERNUM ];
extern	laser	laserdat[ PLAYERNUM ];
extern	int	count;
extern	config	*nowcnf;
extern	int	commode[ PLAYERNUM ];
extern	int	nmiflg;				/* When triggered, it becomes 1 */

void	disp_push_start( void );

#endif
