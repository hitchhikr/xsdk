/*
 *		Display.h
 */

#define		DIV_RADAR_X		(65536 / 128)
#define		DIV_RADAR_Y		(65536 / 128)

#define		RADAR_X			192-6	// (0,0) point on the radar
#define		RADAR_Y			221

#define		RADAR_SX		RADAR_X - 64
#define		RADAR_EX		RADAR_X + 63

#define		MASKREG	((volatile unsigned short *)0xe8002e)	// Text Bitmask Register
#define		R21	((volatile unsigned short *)0xe8002a)

extern	const	unsigned short	textmask[] ;

void	tx_dispinit( int DesperadO, int score );
void	tx_dispinit1( int DesperadO, int score );
void	speedmeter( short speed );		// Speedometer display
void	radar_scope( short scx, short scy, short scx0 , short scy0 );
void	scoredisp( int score );
void	energy_disp1( int ene );		// Energy display (first)
void	energy_disp2( int ene );		// Energy display (when decreasing)
void	warn_display();
void	warn( int flg );
void	tarai_dir_display(int flg);

#if 0
void	radar_set( short x , short y , short c ) ;
void	radar_reset( short x , short y , short c ) ;
#endif

static inline void	radar_set( short x, short y, short c )
{
	short	sx, sy;

	sx = RADAR_X + x / DIV_RADAR_X;
	sy = RADAR_Y + y / DIV_RADAR_Y;

	*MASKREG = textmask[ sx & 15 ];
	*R21 = 0x300 + c * 16;

	*((volatile unsigned short *)0xe00000 + sy * 64 + sx / 16) = 0xffff;
	*R21 = 0;
}

static inline void	radar_reset( short x, short y, short c )
{
	short	sx, sy;

	sx = RADAR_X + x / DIV_RADAR_X;
	sy = RADAR_Y + y / DIV_RADAR_Y;

	*(volatile unsigned short *)MASKREG = textmask[sx & 15];
	*(volatile unsigned short *)R21 = 0x300 + 12 * 16;
	*((volatile unsigned short *)0xe00000 + sx / 16 + sy * 64) = 0;
	*(volatile unsigned short *)R21 = 0;

//	*MASKREG = textmask[ sx & 15 ];
//	*((volatile unsigned short *)0xe40000 + sx / 16 + sy * 64) = 0;
//	*((volatile unsigned short *)0xe60000 + sx / 16 + sy * 64) = 0;
}
