#include	<stdio.h>
#include	<stdlib.h>
#include	<iocslib.h>
#include	<doslib.h>

int spload(char *file)
{
	FILE	*fp;
	unsigned int	buff[32];
    volatile unsigned int	*spr;
	int	i,j;

	spr = (volatile unsigned int *)0x00eb8000;
	if ((fp = fopen(file,"rb")) == NULL){		/* Sprite Data */
		return -1;
	}

	*(volatile short *)0xeb0808 &= 0xffffd;
	for(i = 0; i < 128; i++){
		fread(buff,sizeof(unsigned int),8 * 4,fp);
		for(j = 0; j < 8*4; j++) *(spr + i*32 + j) = buff[j];
	}
	*(volatile short *)0xeb0808 |= 0x0200;
	fclose(fp);
	return 0;
}

int palload(char *file)
{
	int	i;
	unsigned short	buff[16*16];
	volatile unsigned short	*pal;
	FILE	*fp;

	pal = (volatile unsigned short *)0x00e82200;

	if ((fp = fopen(file,"rb")) == NULL){		/* Sprite Data */
		return -1;
	}

	fread(&buff,sizeof(unsigned short),16 * 16,fp);
	*(volatile short *)0xeb0808 &= 0xffffd;
	for(i = 0; i < 15*16; i++) *(pal + i + 16) = buff[i];
	*(volatile short *)0xeb0808 |= 0x0200;
	fclose(fp);
	return 0;
}
