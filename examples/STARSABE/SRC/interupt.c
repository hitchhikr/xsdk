/*	interupt.c : interrupt program	*/

#include	<stdio.h>
#include	<iocslib.h>
#include	"intrupt.h"

#define	IERB	(volatile unsigned char *)0xe88009
#define	IMRB	(volatile unsigned char *)0xe88015
#define	AER	(volatile unsigned char *)0xe88003

volatile int	v_int_count = 0;
unsigned int 	c_vector, r_vector;

void	InitInt(void)
{
	/*	vdisp	*/
	*( (volatile unsigned int *)(0x46 * 4) ) = (unsigned int)VInterrupt;
	*AER = *AER & (255-16);
	*IERB = *IERB | 64;
	*IMRB = *IMRB | 64;
	
	/*	copy key	*/
	c_vector = *( (volatile unsigned int *)(0x2c * 4) );
	*( (volatile unsigned int *)(0x2c * 4) ) = (unsigned int)CInterrupt;
	
	/*	power & reset	*/
	r_vector = *( (volatile unsigned int *)(0x2a * 4) );
	*( (volatile unsigned int *)(0x2a * 4) ) = (unsigned int)RInterrupt;
}

void	EndInt(void)
{
	*IERB = *IERB & (255-64);
	*IMRB = *IMRB & (255-64);
	*( (volatile unsigned int *)(0x2c * 4) ) = c_vector;
	*( (volatile unsigned int *)(0x2a * 4) ) = r_vector;
}
