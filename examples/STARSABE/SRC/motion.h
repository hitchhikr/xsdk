/*	motion.h : enemy motion header	*/

#define	LASER	1024	/*	motion kind	*/

void	AppearEnemy(ENEMY *, int, short, short, short, unsigned char, unsigned char, unsigned char);
void    DoMotion(ENEMY *);
void	InitMotion(void);
void	InitStageMotion(void);
