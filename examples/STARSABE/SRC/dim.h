/*	dim.h : create 3D to 2D table header	*/

#include	"setbox.h"

#define	DIV		4
#define	EDGE		2047
#define	HALF		(EDGE / 2)	/*	1023	*/

#define	SKY		0
#define	GROUND		(HALF / 2 + 128)
#define	TENJOU		-(GROUND)
#define	SEETOP		-(HALF / 2 + 128)
#define	SEEBOT		(HALF / 4 + 128)
#define	KABE		SEEBOT

#define	LIMIT_XY	(HALF + (HALF / 2))	/*	1534(1023 + 511)	*/
#define	LIMIT_Z		(HALF + (HALF / 8))	/*	1150(1023 + 127)	*/
#define	BOX_		28		/*	The length of one side of the box	*/
#define	HASH_RATE	64
#define	HASH_SIZE	((EDGE + 1) / HASH_RATE)

extern	unsigned short	pos_[ EDGE / DIV + 1 ][ HALF / DIV + 1 ];
extern	unsigned char	box_[ EDGE / DIV + 1 ];

void	InitPos(void);
void	F3Dto2D(BOXBUF *);
