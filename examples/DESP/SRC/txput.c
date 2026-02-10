/*
 *		txput.c
 *				Programmed by ìç	1992
 *
 *		I'm going to speed things up a lot from now on.
 *      Call everything on Supervisor.
 */
#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<graph.h>
#include	<basic.h>
#include	"txput.h"

#define		TRANS		*(dst) = *(src++);		\
				*(dst += 0x80) = *(src++);	\
				*(dst += 0x80) = *(src++);	\
				*(dst += 0x80) = *(src++);	\
				*(dst += 0x80) = *(src++);	\
				*(dst += 0x80) = *(src++);	\
				*(dst += 0x80) = *(src++);	\
				*(dst += 0x80) = *(src++);

#define		TX_YMAX		25
#define		TX_SPACE	128

unsigned char	fontdata[8192 + 32];

void	tx_init(char *fontname)
{
	FILE	*fp;
	char	i;

	if((fp = fopen(fontname, "rb")) == NULL) {
//		puts("txput.c:\tin function tx_init");
		puts("\tUnable to open font file");
	}
	fread(fontdata, 1, 8192 + 32, fp);
	for (i = 0; i < 16; i++) {
		*((volatile short *)0xe82200 + i) = *(short *)(fontdata + i * 2);
	}
	fclose(fp);
}

void	tx_putc(short x, short y, short chnum, char color)
{
	volatile unsigned char	*src;
	volatile unsigned char	*dst;

	src = fontdata + 32 + chnum * 32;

	if(color & 1) {
		dst = (volatile unsigned char *) 0xe00000 + x + y * 0x400;
		TRANS;
	} else {
		src += 8;
	}
	if(color & 2) {
		dst = (volatile unsigned char *) 0xe20000 + x + y * 0x400;
		TRANS;
	} else {
		src += 8;
	}
	if(color & 4) {
		dst = (volatile unsigned char *) 0xe40000 + x + y * 0x400;
		TRANS;
	} else {
		src += 8;
	}
	if(color & 8) {
		dst = (volatile unsigned char *) 0xe60000 + x + y * 0x400;
		TRANS;
	}
}

void	tx_puts(short x, short y, char s[], char color)
{
	char	ch;

	while((ch = *(s++)) != '\0') {
		tx_putc(x++, y, ch - 32, color);
	}
}


void tx_putn(short x, short y, int n, short i, char color)
{
	while(--i >= 0 & n > 0)
	{
		tx_putc(x + i, y, (n % 10) + 16, color);
		n /= 10;
	}

	i++;

	while(--i >= 0) {
		tx_putc(x + i, y, 0, color);
	}
}

void tx_putn2(short x, short y, int n, short i, char color)
{
	if (n) {
		while(--i >= 0 & n > 0)
		{
			tx_putc(x + i, y, (n % 10) + 16, color);
			n /= 10;
		}
		i++;
	} else {
		tx_putc(x + i - 1, y, 16, color);
		i--;
	}

	while(--i >= 0) {
		tx_putc(x + i, y, 0, color);
	}
}

char	*s_num(char *c, int n, short i)
{
	c[i] = '\0';
	while(--i >= 0 & n > 0)
	{
		c[i] = (n % 10) + '0';
		n /= 10;
	}

	i++;

	while(--i >= 0)
	{
		c[i] = ' ';
	}
	return c;
}

void	tx_clr1(void)
{
	short	x, y;

	for(x = (short)0; x < (short)32; x++) {
		for(y = (short)0; y < (short)32; y++) {
			tx_putc(x, y, TX_SPACE, 15);
		}
	}
}

void	tx_clr2(void)
{
	short	x, y;

	for(x = (short)0; x < (short)32; x++) {
		for(y = (short)0; y <= (short)TX_YMAX; y++) {
			tx_putc(x, y, TX_SPACE, 15);
		}
	}
}


void	txclear(void)
{
	volatile int	*addr = (volatile int *)0xe00000;
	for(; addr < (volatile int *)0xe80000; addr++) {
		*addr = 0;
	}
}

