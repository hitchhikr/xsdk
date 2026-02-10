#ifndef	_ZLIB_H_
#define	_ZLIB_H_
void	z_init() ;
int	z_alloc( int track , int size ) ;
int	z_assign( int channel , int track ) ;
int	z_stat( int bitptr ) ;
int	z_play( int d2 , int d3 , int d4 ) ;
int	z_stop( int d2 , int d3 , int d4 ) ;
int	z_cont( int d2 , int d3 , int d4 ) ;
void	z_init_all() ;
void	z_int_stop() ;
int	se_play( int track , void *dataptr ) ;
int	se_adpcm1( char *dataptr , int size , int pan ) ;
int	se_adpcm2( int track , int pan ) ;
int	fade_out( int speed ) ;
int	play_cnv_data( int size , void *dataptr ) ;
int	z_chk_drv( void ) ;
void	*z_get_wk_tbl( void );
#endif
