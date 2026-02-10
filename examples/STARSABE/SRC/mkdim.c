/*	mkdim.c : create 3D to 2D table program	*/

#include	<stdio.h>
#include	<stdlib.h>
#include	"dim.h"

static unsigned short	pos[ EDGE / DIV + 1][ HALF / DIV + 1 ];/* Since it contains 0, add +1 */

/*
	Compile this source file alone to create mkdim.x.
	You can create 3DTO2D.DAT (perspective conversion table) using mkdim <distance from viewpoint>.
	The existing 3DTO2D.DAT was created with mkdim 144.
*/

void	main(int ac, char **av)
{
	int		z, xy, eye_d;
	FILE		*fp;
	const char	*filename = "3dto2d.dat";

	if (ac != 2) {
		fputs("mkdim EYE-distance", stdout);
		exit(1);
	}
	eye_d = atoi(*(av + 1));
	printf("DIV	= %5d\n", DIV);
	printf("EDGE	= %5d\n", EDGE);
	printf("HALF	= %5d\n", HALF);
	for (z = 0; z < EDGE ; z += DIV) {
		int	d = eye_d + z;
		
		for (xy = 0; xy <= HALF; xy += DIV) {
			/*	Calculate the coordinates when projected from 3D space onto the screen	*/
			pos[ z / DIV ][ xy / DIV ] = xy * eye_d / d;
		}
	}

	if ( NULL == (fp = fopen(filename, "wb")) ){
		fprintf(stdout, "%s file open error\n", filename);
		exit(1);
	}
	fwrite(pos, 2, (HALF / DIV + 1) * ( EDGE / DIV + 1), fp);
}
