/* vdisp.c */

#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<doslib.h>

void __attribute__((interrupt)) dispatcher()	/* Interrupt handling routine */
{
	static unsigned short i;
	volatile unsigned short *vram = (volatile unsigned short *)0xc00000;

	*(vram + i) = i++;		/* Write one dot to GRAM */
}

void terminate()			/* CTRL-C processing routine */
{
	VDISPST((void *) 0, 0, 0);		/* Clear interrupt */
	exit(0);
}

int main( int argc, char *argv[] )
{
	CRTMOD(12);				/* Screen Initial Settings */
	G_CLR_ON();
    INTVCS(CTRLVC, terminate); /* Setting the processing routine when CTRL-C is pressed */

	VDISPST( dispatcher, 1 , 1 );	/* VDISP interrupt setting */
	getchar();						/* Wait until return is pressed (XX) */
	VDISPST( (void *)0, 0, 0);		/* Clear interrupt */
    return 0;
}
