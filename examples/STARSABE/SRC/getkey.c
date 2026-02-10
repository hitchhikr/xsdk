/*	getkey.c : joystick and keyboard input program	*/

#include	<stdio.h>
#include	<iocslib.h>

#define		PORTA	(volatile unsigned char *)0xe9a001
#define		PORTB	(volatile unsigned char *)0xe9a003
#define		PORTC	(volatile unsigned char *)0xe9a005

#define		KEYPORT	(volatile unsigned char *)0x800

#define		G26		9
#define		G48		8
#define		GXZ		5
#define		B2		16
#define		B4		128
#define		B6		2
#define		B8		16
#define		BX		8
#define		BZ		4

/*	1	B(Z)	A(X)	1	R	L	D	U	*/

void	InitKey(void)
{
	*PORTC &= 0x0f;		/* Joystick port initialization */
}

int	GetKey(void)
{
	int	keyboard = 0xff, g26, g48, gxz;

	g26 = *(KEYPORT + G26);
	g48 = *(KEYPORT + G48);
	gxz = *(KEYPORT + GXZ);
	/* Make keyboard input the same format as the joystick */
	keyboard =	128 + (!(gxz & BZ)) * 64 + (!(gxz & BX)) * 32 + 16 +
			(!(g26 & B6)) * 8 + (!(g48 & B4)) * 4 +
			(!(g26 & B2)) * 2 + (!(g48 & B8));
	
	return	( (*PORTA) & keyboard );
}

unsigned char	KeySns(int group)
{
	return	*(KEYPORT + group);
}
