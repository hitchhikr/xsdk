#ifndef _IOCS_H
#define _IOCS_H

//#include <sys/cdefs.h>
//#include <sys/types.h>

typedef unsigned short iocs_color_t;

struct iocs_boxptr {
  short	x1;
  short	y1;
  short	x2;
  short	y2;
  iocs_color_t	color;
  unsigned short	linestyle;
};
#define BOXPTR iocs_boxptr

struct iocs_circleptr {
  short		x;
  short		y;
  unsigned short	radius;
  iocs_color_t		color;
  short		start;
  short		end;
  unsigned short	ratio;
};
#define CIRCLEPTR iocs_circleptr

struct iocs_fillptr {
  short	x1;
  short	y1;
  short	x2;
  short	y2;
  iocs_color_t	color;
};
#define FILLPTR iocs_fillptr

struct iocs_fntbuf {
  short		xl;
  short		yl;
  unsigned char	buffer[72];
};
#define FNTBUF iocs_fntbuf

struct iocs_getptr {
  short	x1;
  short	y1;
  short	x2;
  short	y2;
  void	*buf_start;
  void	*buf_end;
};
#define GETPTR iocs_getptr

struct iocs_lineptr {
  short		x1;
  short		y1;
  short		x2;
  short		y2;
  iocs_color_t		color;
  unsigned short	linestyle;
};
#define LINEPTR iocs_lineptr

struct iocs_paintptr {
  short	x;
  short	y;
  iocs_color_t	color;
  void		*buf_start;
  void		*buf_end;
} __attribute__((__packed__));
#define PAINTPTR iocs_paintptr

struct iocs_pointptr {
  short	x;
  short	y;
  iocs_color_t	color;
};
#define POINTPTR iocs_pointptr

struct iocs_psetptr {
  short	x;
  short	y;
  iocs_color_t	color;
};
#define PSETPTR iocs_psetptr

struct iocs_putptr {
  short	x1;
  short	y1;
  short	x2;
  short	y2;
  const void	*buf_start;
  const void	*buf_end;
};
#define PUTPTR iocs_putptr

struct iocs_symbolptr {
  short		x1;
  short		y1;
  unsigned char	*string_address;
  unsigned char	mag_x;
  unsigned char	mag_y;
  iocs_color_t		color;
  unsigned char	font_type;
  unsigned char	angle;
} __attribute__((__packed__));
#define SYMBOLPTR iocs_symbolptr

struct iocs_regs {
  int	d0;
  int	d1;
  int	d2;
  int	d3;
  int	d4;
  int	d5;
  int	d6;
  int	d7;
  int	a1;
  int	a2;
  int	a3;
  int	a4;
  int	a5;
  int	a6;
};
#define REGS iocs_regs

struct iocs_chain {
  void		*addr;
  unsigned short	len;
} __attribute__((__packed__));
#define CHAIN iocs_chain

struct iocs_chain2 {
  void			*addr;
  unsigned short	len;
  const struct iocs_chain2 *next;
} __attribute__((__packed__));
#define CHAIN2 iocs_chain2

struct iocs_clipxy {
  short	xs;
  short	ys;
  short	xe;
  short	ye;
};
#define CLIPXY iocs_clipxy

struct iocs_patst {
  short	offsetx;
  short	offsety;
  short	shadow[16];
  short	pattern[16];
};
#define PATST iocs_patst

struct iocs_tboxptr {
  unsigned short	vram_page;
  short		x;
  short		y;
  short		x1;
  short		y1;
  unsigned short	line_style;
};
#define TBOXPTR iocs_tboxptr

struct iocs_txfillptr {
  unsigned short	vram_page;
  short		x;
  short		y;
  short		x1;
  short		y1;
  unsigned short	fill_patn;
};
#define TXFILLPTR iocs_txfillptr

struct iocs_trevptr {
  unsigned short	vram_page;
  short		x;
  short		y;
  short		x1;
  short		y1;
};
#define TREVPTR iocs_trevptr

struct iocs_xlineptr {
  unsigned short	vram_page;
  short		x;
  short		y;
  short		x1;
  unsigned short	line_style;
};
#define XLINEPTR iocs_xlineptr

struct iocs_ylineptr {
  unsigned short	vram_page;
  short		x;
  short		y;
  short		y1;
  unsigned short	line_style;
};
#define YLINEPTR iocs_ylineptr

struct iocs_tlineptr {
  unsigned short	vram_page;
  short		x;
  short		y;
  short		x1;
  short		y1;
  unsigned short	line_style;
};
#define TLINEPTR iocs_tlineptr

/*
 * for SCSI calls
 */

struct iocs_readcap {
  unsigned int block;
  unsigned int size;
};
#define READCAP iocs_readcap

struct iocs_inquiry {
  unsigned char	unit;
  unsigned char	info;
  unsigned char	ver;
  unsigned char	reserve;
  unsigned char	size;
  unsigned char	buff[0];	/* actually longer */
};
#define INQUIRY iocs_inquiry

//__BEGIN_DECLS

extern int	_iocs_trap15 (struct iocs_regs *, struct iocs_regs *);
#define TRAP15 _iocs_trap15
extern void	_iocs_abortjob (void) __attribute__((__noreturn__));
#define ABORTJOB _iocs_abortjob
extern void	_iocs_abortrst (void);
#define ABORTRST _iocs_abortrst
extern void	_iocs_adpcmain (const struct iocs_chain *, int, int);
#define ADPCMAIN _iocs_adpcmain
extern void	_iocs_adpcmaot (const struct iocs_chain *, int, int);
#define ADPCMAOT _iocs_adpcmaot
extern void	_iocs_adpcminp (void *, int, int);
#define ADPCMINP _iocs_adpcminp
extern void	_iocs_adpcmlin (const struct iocs_chain2 *, int);
#define ADPCMLIN _iocs_adpcmlin
extern void	_iocs_adpcmlot (const struct iocs_chain2 *, int);
#define ADPCMLOT _iocs_adpcmlot
extern void	_iocs_adpcmmod (int);
#define ADPCMMOD _iocs_adpcmmod
extern void	_iocs_adpcmout (const void *, int, int);
#define ADPCMOUT _iocs_adpcmout
extern int	_iocs_adpcmsns (void);
#define ADPCMSNS _iocs_adpcmsns
extern int	_iocs_akconv (int, int) __attribute__((const, __pure__));
#define AKCONV _iocs_akconv
extern int	_iocs_alarmget (int *, int *, int *);
#define ALARMGET _iocs_alarmget
extern int	_iocs_alarmmod (int);
#define ALARMMOD _iocs_alarmmod
extern int	_iocs_alarmset (int, int, int);
#define ALARMSET _iocs_alarmset
extern int	_iocs_apage (int);
#define APAGE _iocs_apage
extern int	_iocs_b_assign (int, int, int, const void *);
#define B_ASSIGN _iocs_b_assign
extern int	_iocs_b_badfmt (int, int, int);
#define B_BADFMT _iocs_b_badfmt
extern int	_iocs_b_bpeek (const void *);
#define B_BPEEK _iocs_b_bpeek
extern void	_iocs_b_bpoke (void *, int);
#define B_BPOKE _iocs_b_bpoke
extern void	_iocs_b_clr_al (void);
#define B_CLR_AL _iocs_b_clr_al
extern void	_iocs_b_clr_ed (void);
#define B_CLR_ED _iocs_b_clr_ed
extern void	_iocs_b_clr_st (void);
#define B_CLR_ST _iocs_b_clr_st
extern int	_iocs_b_color (int);
#define B_COLOR _iocs_b_color
extern int	_iocs_b_consol (int, int, int, int);
#define B_CONSOL _iocs_b_consol
extern void	_iocs_b_curdef (void *); /*1.3/iocs.x*/
#define B_CURDEF _iocs_b_curdef
extern void	_iocs_b_curmod (int); /*1.3/iocs.x*/
#define B_CURMOD _iocs_b_curmod
extern void	_iocs_b_curoff (void);
#define B_CUROFF _iocs_b_curoff
extern void	_iocs_b_curon (void);
#define B_CURON _iocs_b_curon
extern void	_iocs_b_curpat (int); /* 1.3 & iocs.x */
#define B_CURPAT _iocs_b_curpat
extern void	_iocs_b_curpat1 (void); /* 1.3 & iocs.x */
#define B_CURPAT1 _iocs_b_curpat1
extern void	_iocs_b_del (int);
#define B_DEL _iocs_b_del
extern void	_iocs_b_down (int);
#define B_DOWN _iocs_b_down
extern void	_iocs_b_down_s (void);
#define B_DOWN_S _iocs_b_down_s
extern int	_iocs_b_drvchk (int, int);
#define B_DRVCHK _iocs_b_drvchk
extern int	_iocs_b_drvsns (int);
#define B_DRVSNS _iocs_b_drvsns
extern int	_iocs_b_dskini (int, const void *, int);
#define B_DSKINI _iocs_b_dskini
extern int	_iocs_b_eject (int);
#define B_EJECT _iocs_b_eject
extern void	_iocs_b_era_al (void);
#define B_ERA_AL _iocs_b_era_al
extern void	_iocs_b_era_ed (void);
#define B_ERA_ED _iocs_b_era_ed
extern void	_iocs_b_era_st (void);
#define B_ERA_ST _iocs_b_era_st
extern int	_iocs_b_format (int, int, int, const void *);
#define B_FORMAT _iocs_b_format
extern void	_iocs_b_ins (int);
#define B_INS _iocs_b_ins
extern void *_iocs_b_intvcs (int vector, void *addr);
#define B_INTVCS _iocs_b_intvcs
extern int	_iocs_b_keyinp (void);
#define B_KEYINP _iocs_b_keyinp
extern int	_iocs_b_keysns (void);
#define B_KEYSNS _iocs_b_keysns
extern void	_iocs_b_left (int);
#define B_LEFT _iocs_b_left
extern int	_iocs_b_locate (int, int);
#define B_LOCATE _iocs_b_locate
extern int	_iocs_b_lpeek (const void *);
#define B_LPEEK _iocs_b_lpeek
extern void	_iocs_b_lpoke (void *, int);
#define B_LPOKE _iocs_b_lpoke
extern void	_iocs_b_memset (void *, const void *, int);
#define B_MEMSET _iocs_b_memset
extern void	_iocs_b_memstr (const void *, void *, int);
#define B_MEMSTR _iocs_b_memstr
extern int	_iocs_b_print (const char *);
#define B_PRINT _iocs_b_print
extern int	_iocs_b_putc (int);
#define B_PUTC _iocs_b_putc
extern int	_iocs_b_putmes (int, int, int, int, const char *);
#define B_PUTMES _iocs_b_putmes
extern int	_iocs_b_read (int, int, int, void *);
#define B_READ _iocs_b_read
extern int	_iocs_b_readdi (int, int, int, void *);
#define B_READDI _iocs_b_readdi
extern int	_iocs_b_readdl (int, int, int, void *);
#define B_READDL _iocs_b_readdl
extern int	_iocs_b_readid (int, int, void *);
#define B_READID _iocs_b_readid
extern int	_iocs_b_recali (int);
#define B_RECALI _iocs_b_recali
extern void	_iocs_b_right (int);
#define B_RIGHT _iocs_b_right
extern void	_iocs_b_scroll (int); /* 1.3 & iocs.x */
#define B_SCROLL _iocs_b_scroll
extern int	_iocs_b_seek (int, int);
#define B_SEEK _iocs_b_seek
extern int	_iocs_b_sftsns (void);
#define B_SFTSNS _iocs_b_sftsns
extern int	_iocs_b_super (int);
#define B_SUPER _iocs_b_super
extern void	_iocs_b_up (int);
#define B_UP _iocs_b_up
extern void	_iocs_b_up_s (void);
#define B_UP_S _iocs_b_up_s
extern int	_iocs_b_verify (int, int, int, const void *);
#define B_VERIFY _iocs_b_verify
extern int	_iocs_b_wpeek (const void *);
#define B_WPEEK _iocs_b_wpeek
extern void	_iocs_b_wpoke (void *, int);
#define B_WPOKE _iocs_b_wpoke
extern int	_iocs_b_write (int, int, int, const void *);
#define B_WRITE _iocs_b_write
extern int	_iocs_b_writed (int, int, int, const void *);
#define B_WRITED _iocs_b_writed
extern int	_iocs_bgctrlgt (int);
#define BGCTRLGT _iocs_bgctrlgt
extern int	_iocs_bgctrlst (int, int, int);
#define BGCTRLST _iocs_bgctrlst
extern int	_iocs_bgscrlgt (int, int *, int *);
#define BGSCRLGT _iocs_bgscrlgt
extern int	_iocs_bgscrlst (int, int, int);
#define BGSCRLST _iocs_bgscrlst
extern int	_iocs_bgtextcl (int, int);
#define BGTEXTCL _iocs_bgtextcl
extern int	_iocs_bgtextgt (int, int, int);
#define BGTEXTGT _iocs_bgtextgt
extern int	_iocs_bgtextst (int, int, int, int);
#define BGTEXTST _iocs_bgtextst
extern int	_iocs_bindatebcd (int);
#define BINDATEBCD _iocs_bindatebcd
extern int	_iocs_bindateget (void);
#define BINDATEGET _iocs_bindateget
extern void	_iocs_bindateset (int);
#define BINDATESET _iocs_bindateset
extern int	_iocs_bitsns (int);
#define BITSNS _iocs_bitsns
extern int	_iocs_bootinf (void) __attribute__((const, __pure__));
#define BOOTINF _iocs_bootinf
extern int	_iocs_box (const struct iocs_boxptr *);
#define BOX _iocs_box
extern int	_iocs_cache_md (int); /* 1.3 only */
#define CACHE_MD _iocs_cache_md
extern int	_iocs_cache_st (void); /* 1.3 only */
#define CACHE_ST _iocs_cache_st
extern int	_iocs_circle (const struct iocs_circleptr *);
#define CIRCLE _iocs_circle
extern void	_iocs_clipput (int, int, const struct iocs_fntbuf *, const struct iocs_clipxy *);
#define CLIPPUT _iocs_clipput
extern int	_iocs_contrast (int);
#define CONTRAST _iocs_contrast
extern int	_iocs_crtcras (const void *, int);
#define CRTCRAS _iocs_crtcras
extern int	_iocs_crtmod (int);
#define CRTMOD _iocs_crtmod
extern int	_iocs_dakjob (char *);
#define DAKJOB _iocs_dakjob
extern int	_iocs_dateasc (int, char *);
#define DATEASC _iocs_dateasc
extern int	_iocs_datebin (int);
#define DATEBIN _iocs_datebin
extern int	_iocs_datecnv (const char *);
#define DATECNV _iocs_datecnv
extern void	_iocs_dayasc (int, char *);
#define DAYASC _iocs_dayasc
extern int	_iocs_defchr (int, int, const void *);
#define DEFCHR _iocs_defchr
extern void	_iocs_densns (void);
#define DENSNS _iocs_densns
extern int	_iocs_dmamode (void);
#define DMAMODE _iocs_dmamode
extern void	_iocs_dmamov_a (const struct iocs_chain *, void *, int, int);
#define DMAMOV_A _iocs_dmamov_a
extern void	_iocs_dmamov_l (const struct iocs_chain2 *, void *, int);
#define DMAMOV_L _iocs_dmamov_l
extern void	_iocs_dmamove (void *, void *, int, int);
#define DMAMOVE _iocs_dmamove
extern int	_iocs_drawmode (int); /* 1.3 & iocs.x */
#define DRAWMODE _iocs_drawmode
extern int	_iocs_fill (const struct iocs_fillptr *);
#define FILL _iocs_fill
extern int	_iocs_fntget (int, int, struct iocs_fntbuf *);
#define FNTGET _iocs_fntget
extern void	_iocs_g_clr_on (void);
#define G_CLR_ON _iocs_g_clr_on
extern int	_iocs_getgrm (struct iocs_getptr *);
#define GETGRM _iocs_getgrm
extern int	_iocs_gpalet (int, int);
#define GPALET _iocs_gpalet
extern int	_iocs_hanjob (char *);
#define HANJOB _iocs_hanjob
extern int	_iocs_home (int, int, int);
#define HOME _iocs_home
extern int	_iocs_hsvtorgb (int, int, int) __attribute__((const, __pure__));
#define HSVTORGB _iocs_hsvtorgb
extern int	_iocs_hsyncst (const void *);
#define HSYNCST _iocs_hsyncst
extern int	_iocs_init_prn (int, int);
#define INIT_PRN _iocs_init_prn
extern int	_iocs_inp232c (void);
#define INP232C _iocs_inp232c
extern void	_iocs_iplerr (void) __attribute__((__noreturn__));
#define IPLERR _iocs_iplerr
extern int	_iocs_isns232c (void);
#define ISNS232C _iocs_isns232c
extern int	_iocs_jissft (int) __attribute__((const, __pure__));
#define JISSFT _iocs_jissft
extern int	_iocs_joyget (int);
#define JOYGET _iocs_joyget
extern void	_iocs_ledmod (int, int);
#define LEDMOD _iocs_ledmod
extern int	_iocs_line (const struct iocs_lineptr *);
#define LINE _iocs_line
extern int	_iocs_lof232c (void);
#define LOF232C _iocs_lof232c
extern int	_iocs_mpu_stat (void); /* 1.3 only */
#define MPU_STAT _iocs_mpu_stat
extern int	_iocs_ms_curgt (void);
#define MS_CURGT _iocs_ms_curgt
extern void	_iocs_ms_curof (void);
#define MS_CUROF _iocs_ms_curof
extern void	_iocs_ms_curon (void);
#define MS_CURON _iocs_ms_curon
extern int	_iocs_ms_curst (int, int);
#define MS_CURST _iocs_ms_curst
extern int	_iocs_ms_getdt (void);
#define MS_GETDT _iocs_ms_getdt
extern void	_iocs_ms_init (void);
#define MS_INIT _iocs_ms_init
extern int	_iocs_ms_limit (int, int, int, int);
#define MS_LIMIT _iocs_ms_limit
extern int	_iocs_ms_offtm (int, int);
#define MS_OFFTM _iocs_ms_offtm
extern int	_iocs_ms_ontm (int, int);
#define MS_ONTM _iocs_ms_ontm
extern void	_iocs_ms_patst (int, const struct iocs_patst *);
#define MS_PATST _iocs_ms_patst
extern void	_iocs_ms_sel (int);
#define MS_SEL _iocs_ms_sel
extern void	_iocs_ms_sel2 (const short *);
#define MS_SEL2 _iocs_ms_sel2
extern int	_iocs_ms_stat (void);
#define MS_STAT _iocs_ms_stat
extern int _iocs_ontime (void);
#define ONTIME _iocs_ontime
extern int	_iocs_opmintst (const void *);
#define OPMINTST _iocs_opmintst
extern void	_iocs_opmset (int, int);
#define OPMSET _iocs_opmset
extern int	_iocs_opmsns (void);
#define OPMSNS _iocs_opmsns
extern void	_iocs_os_curof (void);
#define OS_CUROF _iocs_os_curof
extern void	_iocs_os_curon (void);
#define OS_CURON _iocs_os_curon
extern int	_iocs_osns232c (void);
#define OSNS232C _iocs_osns232c
extern void	_iocs_out232c (int);
#define OUT232C _iocs_out232c
extern void	_iocs_outlpt (int);
#define OUTLPT _iocs_outlpt
extern void	_iocs_outprn (int);
#define OUTPRN _iocs_outprn
extern int	_iocs_paint (struct iocs_paintptr *);
#define PAINT _iocs_paint
extern int	_iocs_point (const struct iocs_pointptr *);
#define POINT _iocs_point
extern int	_iocs_prnintst (const void *);
#define PRNINTST _iocs_prnintst
extern int	_iocs_pset (const struct iocs_psetptr *);
#define PSET _iocs_pset
extern int	_iocs_putgrm (const struct iocs_putptr *);
#define PUTGRM _iocs_putgrm
extern int	_iocs_rmacnv (int, char *, char *);
#define RMACNV _iocs_rmacnv
extern int	_iocs_romver (void) __attribute__((const, __pure__));
#define ROMVER _iocs_romver
extern int	_iocs_s_cmdout (int, void *);
#define S_CMDOUT _iocs_s_cmdout
extern int	_iocs_s_datain (int, void *);
#define S_DATAIN _iocs_s_datain
extern int	_iocs_s_dataout (int, void *);
#define S_DATAOUT _iocs_s_dataout
extern int	_iocs_s_format (int, int);
#define S_FORMAT _iocs_s_format
extern int	_iocs_s_inquiry (int, int, struct iocs_inquiry *);
#define S_INQUIRY _iocs_s_inquiry
extern int	_iocs_s_msgin (void *);
#define S_MSGIN _iocs_s_msgin
extern int	_iocs_s_msgout (void *);
#define S_MSGOUT _iocs_s_msgout
extern int	_iocs_s_pamedium (int, int);
#define S_PAMEDIUM _iocs_s_pamedium
extern int	_iocs_s_phase (void);
#define S_PHASE _iocs_s_phase
extern int	_iocs_s_read (int, int, int, int, void *);
#define S_READ _iocs_s_read
extern int	_iocs_s_readcap (int, struct iocs_readcap *);
#define S_READCAP _iocs_s_readcap
extern int	_iocs_s_readext (int, int, int, int, void *);
#define S_READEXT _iocs_s_readext
extern int	_iocs_s_reassign (int, int, void *);
#define S_REASSIGN _iocs_s_reassign
extern int	_iocs_s_request (int, int, void *);
#define S_REQUEST _iocs_s_request
extern void	_iocs_s_reset (void);
#define S_RESET _iocs_s_reset
extern int	_iocs_s_rezerounit (int);
#define S_REZEROUNIT _iocs_s_rezerounit
extern int	_iocs_s_seek (int, int);
#define S_SEEK _iocs_s_seek
extern int	_iocs_s_select (int);
#define S_SELECT _iocs_s_select
extern int	_iocs_s_startstop (int, int);
#define S_STARTSTOP _iocs_s_startstop
extern int	_iocs_s_stsin (void *);
#define S_STSIN _iocs_s_stsin
extern int	_iocs_s_testunit (int);
#define S_TESTUNIT _iocs_s_testunit
extern int	_iocs_s_write (int, int, int, int, void *);
#define S_WRITE _iocs_s_write
extern int	_iocs_s_writeext (int, int, int, int, void *);
#define S_WRITEEXT _iocs_s_writeext
extern void	_iocs_scroll (int, int, int);
#define SCROLL _iocs_scroll
extern int	_iocs_set232c (int);
#define SET232C _iocs_set232c
extern int	_iocs_sftjis (int) __attribute__((const, __pure__));
#define SFTJIS _iocs_sftjis
extern int	_iocs_skey_mod (int, int, int);
#define SKEY_MOD _iocs_skey_mod
extern void	_iocs_skeyset (int);
#define SKEYSET _iocs_skeyset
extern int	_iocs_snsprn (void);
#define SNSPRN _iocs_snsprn
extern int	_iocs_sp_cgclr (int);
#define SP_CGCLR _iocs_sp_cgclr
extern int	_iocs_sp_defcg (int, int, const void *);
#define SP_DEFCG _iocs_sp_defcg
extern int	_iocs_sp_gtpcg (int, int, void *);
#define SP_GTPCG _iocs_sp_gtpcg
extern int	_iocs_sp_init (void);
#define SP_INIT _iocs_sp_init
extern void	_iocs_sp_off (void);
#define SP_OFF _iocs_sp_off
extern int	_iocs_sp_on (void);
#define SP_ON _iocs_sp_on
extern int	_iocs_sp_reggt (int, int *, int *, int *, int *);
#define SP_REGGT _iocs_sp_reggt
extern int	_iocs_sp_regst (int, int, int, int, int, int);
extern int	SP_REGST (int, int, int, int, int);
extern int	_iocs_spalet (int, int, int);
#define SPALET _iocs_spalet
extern int	_iocs_symbol (const struct iocs_symbolptr *);
#define SYMBOL _iocs_symbol
extern void	_iocs_tcolor (int);
#define TCOLOR _iocs_tcolor
extern void	_iocs_textget (int, int, struct iocs_fntbuf *);
#define TEXTGET _iocs_textget
extern void	_iocs_textput (int, int, const struct iocs_fntbuf *);
#define TEXTPUT _iocs_textput
extern int	_iocs_tgusemd (int, int);
#define TGUSEMD _iocs_tgusemd
extern int	_iocs_timeasc (int, char *);
#define TIMEASC _iocs_timeasc
extern int	_iocs_timebcd (int);
#define TIMEBCD _iocs_timebcd
extern int	_iocs_timebin (int);
#define TIMEBIN _iocs_timebin
extern int	_iocs_timecnv (const char *);
#define TIMECNV _iocs_timecnv
extern int	_iocs_timeget (void);
#define TIMEGET _iocs_timeget
extern int	_iocs_timerdst (const void *, int, int);
#define TIMERDST _iocs_timerdst
extern void	_iocs_timeset (int);
#define TIMESET _iocs_timeset
extern int	_iocs_tpalet (int, int);
#define TPALET _iocs_tpalet
extern int	_iocs_tpalet2 (int, int);
#define TPALET2 _iocs_tpalet2
extern void	_iocs_tvctrl (int);
#define TVCTRL _iocs_tvctrl
extern void	_iocs_txbox (const struct iocs_tboxptr *);
#define TXBOX _iocs_txbox
extern void	_iocs_txfill (const struct iocs_txfillptr *);
#define TXFILL _iocs_txfill
extern void	_iocs_txline (struct iocs_tlineptr *); /* 1.3 & iocs.x */
#define TXLINE _iocs_txline
extern void	_iocs_txrascpy (int, int, int);
#define TXRASCPY _iocs_txrascpy
extern void	_iocs_txrev (const struct iocs_trevptr *);
#define TXREV _iocs_txrev
extern void	_iocs_txxline (const struct iocs_xlineptr *);
#define TXXLINE _iocs_txxline
extern void	_iocs_txyline (const struct iocs_ylineptr *);
#define TXYLINE _iocs_txyline
extern int	_iocs_vdispst (const void *, int, int);
#define VDISPST _iocs_vdispst
extern int	_iocs_vpage (int);
#define VPAGE _iocs_vpage
extern int	_iocs_window (int, int, int, int);
#define WINDOW _iocs_window
extern int	_iocs_wipe (void);
#define WIPE _iocs_wipe

//__END_DECLS

#endif /* _IOCS_H */
