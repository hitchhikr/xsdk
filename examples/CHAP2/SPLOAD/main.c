/*
 *	sprite loader/saver
 *				programmed by chika@kmc 1994
 */


#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<iocslib.h>

#define	PCG_NUM		256		/* characters */
#define	PCG_SIZE_MAX	(PCG_NUM * 64)	/* words */

unsigned short	pcgsize = PCG_SIZE_MAX;
unsigned short	pcgdata[ PCG_SIZE_MAX ];
volatile unsigned short	*pcgarea = (volatile unsigned short *)0xeb8000;

unsigned short	palsize = 16 * 15;
unsigned short	paldata[ 16 * 16 ];
volatile unsigned short	*palarea = (volatile unsigned short *)0xe82220;

char	*pcg_ext = ".SP";
char	*pal_ext = ".PAL";
char	strbuf[ 256 ];
char	pcg_fname[ 256 ];
char	pal_fname[ 256 ];

int	load_palp = 1;			/* Load/save palette? */
int	load_pcgp = 1;			/* Do you want to load/save the PCG area? */
int	start_pcg_area = 0;		/* PCG load/save start position */

int	crtmode = 0;

#ifdef	SPSAVE
int		savep = 1;
#define	PROGRAMNAME	"spsave"
#else
int		savep = 0;
#define	PROGRAMNAME	"spload"
#endif

void	exitter( int no, char *mes )
{
	if (crtmode > 0xe) {
		CRTMOD(crtmode + 0x100);
		printf("*");
	}
	puts(mes);
	exit(no);
}

void	openerror( char *fname )
{
	sprintf( strbuf, "Unable to open file %s\n", fname );
	exitter( 1, strbuf );
}

void	usage( void )
{
	puts( "sprite pcg/palette area loader/saver \'" PROGRAMNAME "\'" );
	puts( "programmed by chika@kmc 1994/8\n" );
	puts( "How to use:" PROGRAMNAME " [options] (filename)" );
	puts( "Options:\t-p\t\tPalette data only" );
	puts( "             \t-c\t\tPCG data only" );
#ifdef	SPSAVE
	puts( "             \t-s\t\tSave\t(default)" );
	puts( "             \t-l\t\tLoad" );
#else
	puts( "             \t-l\t\tLoad\t(default)" );
	puts( "             \t-s\t\tSave" );
#endif
	puts( "             \t-b(num)\t\tWhen loading/saving PCG, start from the numth area of the PCG area." );
	puts( "             \t-n(num)\t\tSave num items when saving PCG" );
	exit( 1 );
}

/*
 *		Look from the back of the file extension,
 *      and if there is a period without a /, that is the period before the extension.
 */

void	addext( char *dst, char *src, char *ext )
{
	int	flg = 0;
	char	*srcc = src;

	strcpy(dst, src);

	while (*src != '\0') *(dst++) = *(src++);
	while (src != srcc){
		src--;
		if (*src == '\\') break;
		if (*src == '/') break;
		if (*src == '.') flg = 1;
		if (flg) break;
	}
	if (!flg) strcpy(dst, ext);
}

/*
 *		Parse command line arguments
 */

void	argchk( int argc, char **argv )
{
	int	flg = 0, tmp;
	argv += 1;
	for( ; argc > 1; argc--, argv++ ){
		if( **argv == '-' || **argv == '/' ){
			switch( *(*argv + 1) ) {
			  case 'p':
				load_palp = 1;
				load_pcgp = 0;
				break;
			  case 'c':
				load_palp = 0;
				load_pcgp = 1;
				break;
			  case 's':
			  	savep = 1;
				break;
			  case 'l':
				savep = 0;
				break;
			  case 'b':
			  	tmp = atoi( *argv + 2 );
			  	if( !tmp ) {puts( "command error" ); exit(1); }
				start_pcg_area = tmp;
				break;
			  case 'n':
			  	tmp = atoi( *argv + 2 );
			  	if( !tmp ) {puts( "command error" ); exit(1); }
				pcgsize = tmp * 64;
				break;
			  default:
				usage();
			}
		} else {
			if( flg ) usage();
			addext( pcg_fname, *argv, pcg_ext );
			addext( pal_fname, *argv, pal_ext );
			flg = 1;
		}
	}
	if( !flg ) usage();
	if( pcgsize > PCG_SIZE_MAX - start_pcg_area * 64 ) {
		pcgsize = PCG_SIZE_MAX - start_pcg_area * 64;
	}
	pcgarea = (volatile unsigned short *)0xeb8000 + (start_pcg_area * 64);
}

/*		load pcg area		*/

void	load_pcg( void )
{
	int	i;
	FILE	*fp;

	fp = fopen( pcg_fname, "rb" );
	if( !fp ) openerror( pcg_fname );
	printf( "Reading %s\n", pcg_fname );
	fread( pcgdata, 2, pcgsize, fp );
	fclose( fp );

	for( i = 0; i < pcgsize; i++ ){
		*( pcgarea++ ) = pcgdata[ i ];
	}
}

/*		load palette area	*/

void	load_pal( void )
{
	int	i;
	FILE	*fp;

	fp = fopen( pal_fname, "rb" );
	if( !fp ) openerror( pal_fname );
	printf( "Reading %s\n", pal_fname );
	fread( paldata, 2, palsize, fp );
	fclose( fp );

	for( i = 0; i < palsize; i++ ){
		*( palarea++ ) = paldata[ i ];
	}
}

/*		save pcg area		*/

void	save_pcg( void )
{
	int	i;
	FILE	*fp;

	for( i = 0; i < pcgsize; i++ ){
		pcgdata[ i ] = *( pcgarea++ );
	}

	fp = fopen( pcg_fname, "wb" );
	if( !fp ) openerror( pcg_fname );
	printf( "Writing %s\n", pcg_fname );
	fwrite( pcgdata, 2, pcgsize, fp );
	fclose( fp );
}

/*		save palette area	*/

void	save_pal( void )
{
	int	i;
	FILE	*fp;

	for( i = 0; i < palsize; i++ ){
		paldata[ i ] = *( palarea++ ); 
	}

	fp = fopen( pal_fname, "wb" );
	if( !fp ) openerror( pal_fname );
	printf( "Writing %s\n", pal_fname );
	fwrite( paldata, 2, palsize, fp );
	fclose( fp );
}


/*
 *		Main Routine
 */

int	main( int argc, char **argv )
{
	int	i;

	argchk( argc, argv );

DUMMY:	B_SUPER( 0 );
	crtmode = CRTMOD( -1 );
	if( crtmode > 0xe )	CRTMOD( 0x10c );

	if( savep ) {
		if( load_pcgp ) save_pcg();
		if( load_palp ) save_pal();
	} else {
		if( load_pcgp ) load_pcg();
		if( load_palp ) load_pal();
	}

	exitter( 0,"Complete." );
	return 0;
}
