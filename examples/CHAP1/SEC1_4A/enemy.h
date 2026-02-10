/*******************************
		  e n e m y . h
*******************************/

#define ENEMY_NUM 5

#define ENEMY_CODE 0x0102
#define ENEMY_PCG 2 	  /* Sprite PCG data number */
#define ENEMY_PAL 2 	  /* Palette table number */

#define ENEMY_DEAD -1
#define ENEMY_LIVE 1

typedef struct ENEMY {
	short flag; 		  /* Type (dead/alive) */
	short x, y; 		  /* Coordinates */
	short vx, vy;		  /* Speed */
} ENEMY;

void enemy_init(void);
void enemy_move(void);

