/*	dim.c : create 3D to 2D table program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"dim.h"

unsigned short	pos_[ EDGE / DIV + 1 ][ HALF / DIV + 1 ];	/* Since it contains 0, add +1 */
unsigned char	box_[ EDGE / DIV + 1 ];				/* Size */

void	InitPos(void)
{
	int		z;
	FILE		*fp;
	const char	*filename = "3dto2d.dat";

	if ( NULL == (fp = fopen(filename, "rb")) ){
		fprintf(stdout, "%s file open error\n", filename);
		exit(1);
	}
	puts("3D table now reading ...");
	fread(pos_, 2, ( HALF / DIV + 1 ) * ( EDGE / DIV + 1 ), fp);
	for (z = 0; z < EDGE; z += DIV) {
		box_[ z / DIV ] = pos_[ z / DIV ][ BOX_ / DIV ] - pos_[ z / DIV ][ 0 ];
		if ( box_[ z / DIV ] > 16 ) box_[ z / DIV ] = 16;
	}
	fclose(fp);
}

void	F3Dto2D(BOXBUF *boxbuf)	/*	-HALF <= z <= HALF	*/
{
	int	index = (HALF - boxbuf->z) / DIV;
	unsigned short	*pos_ptr = (unsigned short *)pos_ + index * (HALF / DIV + 1);
	
	boxbuf->rx = ( boxbuf->x >= 0
		? 136 + *( pos_ptr + boxbuf->x / DIV )
		: 136 - *( pos_ptr - boxbuf->x / DIV )
		);
	boxbuf->ry = ( boxbuf->y >= 0
		? 136 + *( pos_ptr + boxbuf->y / DIV )
		: 136 - *( pos_ptr - boxbuf->y / DIV )
		);
	boxbuf->box = *(box_ + index);
}
