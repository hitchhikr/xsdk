/*
 *	Graph.h
 *		DesperadO Graphic Display Routine
 */

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<graph.h>
#include	"dsp.h"

#define		PALSIZE		32768
extern	unsigned short	gpalet[32][256];	//Contains the palette of the graphic.
extern	void	gtrans(volatile unsigned short *dst, volatile unsigned char *src);

int	trans1(unsigned char *cmdp, unsigned short *colp, volatile void *dstp);

#if	0

/*
 *	PICLOAD TO VRAM
 *
 *		plane = 0, 1       GVRAM
 *		mode = 0 --- Usually
 *		mode = 1 --- 8-color mode (256 colors divided into 16*16)
 *		mode = 2 --- 16-color mode (256 colors divided into 16*16)
 */


int	dsp_picload(char *fname, char plane, char mode)
{
	FILE	*fp;

	char	buf[2] = "  ";
	unsigned short	*paletbuf;
	unsigned short	*src, *dst, *dst0;
	unsigned int	col, len;
	unsigned short	*picdata;
	unsigned short	paletnum, maxpaletnum;
	int		i, filesize;

	if ((fp = fopen(fname, "rb")) == NULL) {
		return 1;
	}
	filesize = filelength(fileno(fp));
	if ((picdata = (unsigned short *) MALLOC(filesize))
	     >= (unsigned short *) 0x81000000) {
		printf("Unable to allocate memory. (in graph.c)\n");
		dspexit();
	}
	if ((paletbuf = (unsigned short *) MALLOC(PALSIZE * 2 + 4))
	     >= (unsigned short *) 0x81000000) {
		printf("Unable to allocate memory. (in graph.c)\n");
		dspexit();
	}

	for(i = 0; i < PALSIZE;) {
		paletbuf[i++] = 0xffff;
		paletbuf[i++] = 0xffff;
		paletbuf[i++] = 0xffff;
		paletbuf[i++] = 0xffff;
	}
	paletbuf[0] = 0;

	while(*buf != 0x1a) {
		fread(buf, 1, 1, fp);
	}

	fread(picdata, 2, filesize / 2, fp);
	fclose(fp);

	src = picdata + 2;
	if (plane) {
		dst0 = (dst = (volatile unsigned short *) 0xc80000) + 512 * 512;
		paletnum = 129;
		maxpaletnum = 255;
	} else {
		dst0 = (dst = (volatile unsigned short *) 0xc00000) + 512 * 512;
		paletnum = 1;
		maxpaletnum = 127;
	}
	if (mode == 1) {
		paletnum += 15;
		maxpaletnum = paletnum + 96;
	}
	if (mode == 2) {
		maxpaletnum = paletnum + 14;
	}

	switch(mode) {
	  case 0:
		while(dst < dst0) {
			col = *(src ++);
			len = *(src ++);
			if (paletbuf[col / 2] == 0xffff) {
				paletbuf[col / 2] = paletnum;
				gpalet[31][paletnum] = col;
				col = paletnum;
				if (paletnum != maxpaletnum) 
					paletnum++;
			} else {
				col = paletbuf[col / 2];
			}
			while(len) {
				*(dst ++) = col;
				len --;
			}
		}
		break;
	  case 1:
		while(dst < dst0) {
			col = *(src ++);
			len = *(src ++);
			if (paletbuf[col / 2] == 0xffff) {
				paletbuf[col / 2] = paletnum;
				gpalet[31][paletnum] = col;
				col = paletnum;
				if (paletnum != maxpaletnum) 
					paletnum += 16;
			} else {
				col = paletbuf[col / 2];
			}
			while(len) {
				*(dst) = (*dst & 0x0f) | col;
				dst++; len --;
			}
		}
		break;
	  case 2:
		while(dst < dst0) {
			col = *(src ++);
			len = *(src ++);
			if (paletbuf[col / 2] == 0xffff) {
				paletbuf[col / 2] = paletnum;
				gpalet[31][paletnum ] = col;
				gpalet[31][paletnum + 16] = col;
				gpalet[31][paletnum + 32] = col;
				gpalet[31][paletnum + 48] = col;
				gpalet[31][paletnum + 64] = col;
				gpalet[31][paletnum + 80] = col;
				gpalet[31][paletnum + 96] = col;
				gpalet[31][paletnum + 112] = col;
				col = paletnum;
				if (paletnum != maxpaletnum) 
					paletnum ++;
			} else {
				col = paletbuf[col / 2];
			}
			while(len) {
				*(dst) = (*dst & 0xf0) | col;
				dst++; len --;
			}
		}
		break;
	  default:
	  	break;
	}

	MFREE((int)paletbuf);
	MFREE((int)picdata);
	return 0;
}

int	dsp_picload_to_text(char *fname, char *fname2, char plane)
{
	FILE	*fp1;
	FILE	*fp2;

	char	buf[2] = "  ";
	unsigned short	*paletbuf;
	unsigned short	*src;
	unsigned char	*dst;
	unsigned int	col, len, x;
	unsigned short	*picdata;
	short		paletnum, paletnummax;
	int		i, filesize1, filesize2, mlcsize;

	if ((fp1 = fopen(fname, "rb")) == NULL) {
		return 1;
	}
	if ((fp2 = fopen(fname2, "rb")) == NULL) {
		return 1;
	}

	filesize1 = filelength(fileno(fp1));
	filesize2 = filelength(fileno(fp2));

	if (filesize1 > filesize2) mlcsize = filesize1; else mlcsize = filesize2;
	if ((picdata = (unsigned short *) MALLOC(mlcsize))
	     >= (unsigned short *) 0x81000000) {
		printf("Unable to allocate memory. (in graph.c)\n");
		dspexit();
	}
	if ((paletbuf = (unsigned short *) MALLOC(PALSIZE * 2 + 4))
	     >= (unsigned short *) 0x81000000) {
		printf("Unable to allocate memory. (in graph.c)\n");
		dspexit();
	}

	for(i = 0; i < PALSIZE;) {
		paletbuf[i++] = 0xffff;
		paletbuf[i++] = 0xffff;
		paletbuf[i++] = 0xffff;
		paletbuf[i++] = 0xffff;
	}
	paletbuf[0] = 0;

	while(*buf != 0x1a) {
		fread(buf, 1, 1, fp1);
	}
	*buf = 0;
	while(*buf != 0x1a) {
		fread(buf, 1, 1, fp2);
	}

	fread(picdata, 2, filesize1 / 2, fp1);
	fclose(fp1);

	src = picdata + 2;

	if (plane) {
		paletnum = 129;
		paletnummax = 255;
	} else {
		paletnum = 1;
		paletnummax = 128;
	}

	len = 0;

	for(i = 0; i < 4; i++) {

		dst = (volatile unsigned char *) 0xe08000 + 0x20000 * i;

		if (i == 2) {
			fread(picdata, 2, filesize2 / 2, fp2);
			fclose(fp2);
			src = picdata + 2;
			len = 0;
		}

		x = 98304;

		while(x) {

			while(len && x) {
				*(dst ++) = col;
				len --;
				x--;
			}

			if (len == 0) {
				col = *(src ++);
				len = *(src ++);

				if (paletbuf[col / 2] == 0xffff) {
					paletbuf[col / 2] = paletnum;
					gpalet[31][paletnum] = col;
					col = paletnum;
					if (paletnum < paletnummax) paletnum++;
				} else {
					col = paletbuf[col / 2];
				}

			}
		}
	}

	MFREE((int)paletbuf);
	MFREE((int)picdata);
	return 0;
}

#endif

/*
 *	It's pretty rough, but please bear with me.
 */

#define	GDIV_Y	12		// 12 * 32 = 384 (^^)
#define	GY1	192 / GDIV_Y

int	draw_to_graphic(short x0, short x1)
{
	volatile unsigned short	*dst;
	volatile unsigned char	*src;
	short	i, y0;

	y0 = x0 & 0x1e;
	x0 = x0 & 0x03e0;
	x1 = x1 & 0x01e0;

	dst = (volatile unsigned short *)(0xc80000 + x1 * 2 + y0 * 1024 * GDIV_Y);

	if (x0 < 512) {
		if (y0 < GY1) {
			src = (volatile unsigned char *)(0xe08000 + x0 + y0 * 512 * GDIV_Y);
		} else {
			src = (volatile unsigned char *)(0xe28000 + x0 + (y0 - GY1) * 512 * GDIV_Y);
		}
	} else {
		x0 &= 0x1fe;
		if (y0 < GY1) {
			src = (volatile unsigned char *)(0xe48000 + x0 + y0 * 512 * GDIV_Y);
		} else {
			src = (volatile unsigned char *)(0xe68000 + x0 + (y0 - GY1) * 512 * GDIV_Y);
		}
	}
	gtrans(dst, src);
}




/*
 *	Extraction Routine
 */
void	decode(FILE *fp, volatile void *dst)
{
	unsigned char	*cmdp;
	unsigned short	*colp;
	long		cmdsize;
	long		colsize;

	fread(&cmdsize, 4, 1, fp);
	fread(&colsize, 4, 1, fp);
	if ((cmdp = (unsigned char *) MALLOC(cmdsize))
	     >= (unsigned char *) 0x81000000) {
		dsperror("Unable to allocate memory. (in graph.c)");
	}
	if ((colp = (unsigned short *) MALLOC(colsize))
	     >= (unsigned short *) 0x81000000) {
		dsperror("Unable to allocate memory. (in graph.c)");
	}
	fread(cmdp, 1, cmdsize, fp);
	fread(colp, 2, colsize, fp);

	trans1(cmdp, colp, dst);

	MFREE(colp);
	MFREE(cmdp);
}

void	dp_load(char *filename)
{
	FILE	*fp;
	long	dummy;
	volatile unsigned short	*crtc;
    unsigned short	crtcdata;

	crtc = (volatile unsigned short *) 0xe80028;
	crtcdata = *crtc;
	*crtc = (crtcdata & 0xf8ff) | 0x300;		/* VramMemMode = VM16 */

	fp = fopen(filename, "rb");

	if (fp == NULL) {
		dsperror("Can't Read file in function dp_load");
	}

	fread(gpalet[31], 2, 256, fp);
	fread(&dummy, 4, 1, fp);

	decode(fp, (volatile void *)0xc00000);

	fclose(fp);

	*crtc = (crtcdata & 0xf8ff) | 0x100;		/* VramMemMode = VM8 */
}
