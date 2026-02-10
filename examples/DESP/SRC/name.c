#include	<stdio.h>
#include	<string.h>
#include	<iocslib.h>
#include	"dsp.h"
#include	"disp2.h"
#include	"txput.h"
#include	"sound.h"
#include	"dspmove.h"

static	char	topname[5][50] = { "CHIKA", "TABACCO", "OGI", "HUYU", "NKF" };
static	int	topscore[5] = { 3000, 2500, 2000, 1500, 1000 };

/*                         0123456789012345678901234567890123456789	*/
static	char	*inpchr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789. %$";
static	char	*kbd2chr= "??1234567890???%?QWERTYUIOP??$ASDFGHJKL???ZXCVBNM?.??";

#define	INPCHR_LEN		40

#define	MAXMOJI			9
#define	RIGHT			8
#define	LEFT			4
#define	UP			1
#define	DOWN			2

/*
 *		Name display
 */

void	namedisp(void)
{
	int	i;

	tx_puts(7, 3, "BEST FIVE WARRIORS", 12);

	for(i = 0; i < 5; i++){
		tx_putn(3, 8 + i * 3, i + 1, 1, 12);
		tx_puts(6, 8 + i * 3, topname[i], 14);
		tx_putn(22, 8 + i * 3, topscore[i], 6, 14);
		tx_puts(28, 8 + i * 3, "0", 14);
	}
}

#define	NAME_Y	10

/*
 *		Name Entry
 */

void	nameinput(int score)
{
	int	i, j, rank, kbd;
	int	inp, moji = 0;
	int	x = 0, vx = 0;

	rank = 0;

	while(rank <= 4 && topscore[rank] > score) rank++;
	if (rank == 5) return;

	for(j = 3; j >= rank; j--) {
		strcpy(topname[j + 1], topname[j]);
		topscore[j + 1] = topscore[j];
	}
	topname[rank][0] = '\0';
	topscore[rank] = score;

	tx_clr2();

	for(i = 0; i < 5; i++){
		tx_putn(3, NAME_Y + i * 3, i + 1, 1, 12);
		tx_puts(6, NAME_Y + i * 3, topname[i], 14);
		tx_putn(22, NAME_Y + i * 3, topscore[i], 6, 14);
		tx_puts(28, NAME_Y + i * 3, "0", 14);
	}

	tx_puts(6, 2, "ENTER YOUR INITIALS", 12);
	tx_putn(3, 5, rank + 1, 1, 12);
	tx_puts(28, 5, "0", 14);
	tx_putn(22, 5, topscore[rank], 6, 14);

	bgm(4);

	while(B_KEYSNS())			// clear key buffer
		B_KEYINP();
	inp = inpchr[0];
	while (inp != '$' && moji <= MAXMOJI) {
		while (1) {
			vx = 0;

			i = JOY(0);
			if (!(i & RIGHT)) vx = 1;
			else if (! (i & LEFT)) vx = -1;
			if (vx) {
				while(JOY(0) != 255);
				x += vx;
				if (x < 0) x = INPCHR_LEN - 1;
				else if (x >= INPCHR_LEN) x = 0;
				inp = inpchr[x];
			}
			tx_putc(6 + moji, 5, inp - 32, 14);

			if ((i & 96) != 96) {
				break;
			}

			if (B_KEYSNS()) {
				kbd = B_KEYINP() / 256;
				if (kbd > 0 && kbd < 0x34) {
					if (kbd2chr[kbd] != '?') {
						inp = kbd2chr[kbd];
						break;
					}
				}
			}
			vdispwait();

		}
		sd_on(SD_GO);
		while((JOY(0) & 96) != 96);
		if (inp == '%') {			// Back Space
			if (moji) {
				tx_putc(5 + moji, NAME_Y + rank * 3, 0, 14);
				tx_putc(5 + moji, 5, 0, 14);
				tx_putc(6 + moji, 5, 0, 14);
				moji--;
			}
		} else if (inp != '$') {		// Normal Char
			tx_putc(6 + moji, NAME_Y + rank * 3, inp - 32, 14);
			tx_putc(6 + moji, 5, inp - 32, 14);
			topname[rank][moji++] = inp;
		} else {				// End
			tx_putc(6 + moji, 5, 0, 14);
			topname[rank][moji++] = '\0';
			sd_on(SD_SHOT);
		}
	}

	bgm_fade(24);

}
