/*	matrix.c : round matrix program	*/

#include	<stdio.h>
#include	<math.h>
#include	"matrix.h"

#define		PAI		((double)3.1415926535)

short	sin_t[R_MAX];
short	cos_t[R_MAX];

void	InitMatrix(void)
{
	int	i;
	
	for (i = 0; i < R_MAX; ++i) {
		sin_t[i] = (short)( SIN_MAX * sin( 2 * PAI * i / R_MAX ) );
	}
	for (i = 0; i < R_MAX; ++i) {
		cos_t[i] = sin_t[ ((R_MAX / 4) + i) % R_MAX ];
	}
}

/*	Calculate rotation around the z-axis -> around the x-axis -> around the y-axis	*/

void	RoundZXY(short *nx, short *ny, short *nz, unsigned char cx, unsigned char cy, unsigned char cz)
{
	short	x = *nx, y = *ny;
	short	*s = sin_t + cz, *c = cos_t + cz;

	/*	Rotates (direction remains the same!)	*/
	*nx = ( x * (*c) - y * (*s) ) / SIN_MAX;
	*ny = ( x * (*s) + y * (*c) ) / SIN_MAX;
	x = *nx;	y = *ny;
	
	/*	nod one's head	*/
	s = sin_t + cx, c = cos_t + cx;
	*ny = ( y * (*c) - *nz * (*s) ) / SIN_MAX;
	*nz = ( y * (*s) + *nz * (*c) ) / SIN_MAX;
	
	/*	Turn it around (not sideways!) */
	s = sin_t + cy, c = cos_t + cy;
	*nx = ( x * (*c) - *nz * (*s) ) / SIN_MAX;
	*nz = ( x * (*s) + *nz * (*c) ) / SIN_MAX;
}

void	RoundXY(short *nx, short *ny, short *nz, unsigned char cx, unsigned char cy)
{
	short	x = *nx, y = *ny;
	short	*s = sin_t + cx, *c = cos_t + cx;
	
	/*	nod one's head	*/
	*ny = ( y * (*c) - *nz * (*s) ) / SIN_MAX;
	*nz = ( y * (*s) + *nz * (*c) ) / SIN_MAX;
	
	/*	Turn it around (not sideways!) */
	s = sin_t + cy, c = cos_t + cy;
	*nx = ( x * (*c) - *nz * (*s) ) / SIN_MAX;
	*nz = ( x * (*s) + *nz * (*c) ) / SIN_MAX;
}
