
#define   __SPS__

/* You cannot reserve more than the number below. */
#define   SPS_MAX	256

#define   CHR4x4   0
#define   CHR3x3   1
#define   CHR2x2   2
#define   CHR1x1   3
#define   CHR4x2   4
#define   CHR3x2   5
#define   CHR4x3   6

/* for animation */
#define   PCG_4x4_START 176
#define   PCG_4x4_ADRS  ( 0xeb8000 + 128 * PCG_4x4_START )
#define   PCG_4x4_MAX   5
#define   PCG_4x4_SIZE  2048

#define   PCG_3x3_START 32
#define   PCG_3x3_ADRS  ( 0xeb8000 + 128 * PCG_3x3_START )
#define   PCG_3x3_MAX   16
#define   PCG_3x3_SIZE  1152

#define   PCG_2x2_START 32
#define   PCG_2x2_ADRS  ( 0xeb8000 + 128 * PCG_2x2_START )
#define   PCG_2x2_MAX   0
#define   PCG_2x2_SIZE  512

typedef struct SPS {
	/* list pointer */
	struct SPS *_post , *_pre ;

	/* table number */
	short _table_num ;

	/* minus...not disp  CHR4x4...4x4  etc */
	short condition ;

	/* Center of gravity coordinates */
	short x , y ;

	/* pointer to character pattern address */
	unsigned char *chr ;

	/* pallete block number */
	unsigned char palet ;

	/* Only valid when chr == NULL. */
	/* However, CHR1x1 and CHR2x2 do not transfer patterns, so you will always see this. */
	unsigned char num ;

} SPS ;

typedef struct {
	short x ;
	short y ;
	union {
		short dummy ;
		struct {
			unsigned v_rev : 1 ;
			unsigned h_rev : 1 ;
			unsigned dummy : 2 ;
			unsigned color : 4 ;
			unsigned code  : 8 ;
		} sp ;
	} ctrl ;
	union {
		short dummy ;
		struct {
			unsigned dummy : 13 ;
			unsigned ext   : 1 ;
			unsigned prw   : 2 ;
		} sp ;
	} prw ;
} SP_REG ;

/* Assembler programs */
void SpsReg( void ) ;

/* C programs */
void SpsInit( void ) ;
void SpsExit( void ) ;
short Spsys( void ) ;
SPS *SpsAlloc( short size ) ;		/* size ... CHR4x4 etc */
void SpsFree( SPS *buff ) ;			/* Don't release the same thing twice */
void	ScrInit(int screenmode);


