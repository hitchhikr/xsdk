/*	matrix.h : round matrix header	*/

#define		R_MAX		256
#define		SIN_MAX		((short)256)

extern short	sin_t[R_MAX];
extern short	cos_t[R_MAX];

void	InitMatrix(void);
void	RoundZXY(short *, short *, short *, unsigned char, unsigned char, unsigned char);
void	RoundXY(short *, short *, short *, unsigned char, unsigned char);
