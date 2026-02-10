/*	model.h : set model header	*/

typedef	struct boxmodel {
	struct boxmodel	*n;
	short		x;
	short		y;
	short		z;
	int		col;
} BOXMODEL;

typedef struct modeltl {
	BOXMODEL	*chain_boxmodel;
	int		pat;
	int		hit_point;
	int		score;
} MODELTL;

extern MODELTL	*model;

void	InitModel(void);
void	InitStageModel(void);
