/*
 *	Display.h
 */

#define		WIDE		14

#define		X1		8
#define		X2		10
#define		SL_X		( X1 - 2 )
#define		SR_X		( X1 + WIDE )
#define		SL_X2		( SL_X / 2 )
#define		SR_X2		( SR_X / 2 )

#define		XL		( X1 / 2 )
#define		XR		( XL + 6 )

#define		FILENAME_Y	17
#define		SRCDOTS_Y	19
#define		OBJDOTS_Y	20
#define		TRANS_Y		22
#define		PATSAVE_Y	23
#define		PALETMAKE_Y	24
#define		AUTO_Y		25
#define		OBJFILE_Y	28

#define		RGB( r , g , b )	( ( r ) * 64 + ( b ) * 2 + ( g ) * 2048 )

void	momobox( int x , int y , int length , char *str ) ;
void	disp() ;
int	picread( char *fname ) ;
void	pr_objdots( int dots ) ;
void	pr_srcdots( int dots ) ;

void	srcbox( int dots , int c ) ;
void	dstbox( int dots , int c ) ;

