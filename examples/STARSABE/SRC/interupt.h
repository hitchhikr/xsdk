/*	interupt.h : interrupt header	*/

#define	VDISPWAIT_COUNT	2

extern	volatile int	v_int_count;

void	InitInt(void);
void	EndInt(void);
