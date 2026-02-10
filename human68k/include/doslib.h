#ifndef _DOS_H
#define _DOS_H

#include <sys/cdefs.h>
#include <stdio.h>

#define _DOS_IRDONLY	0x001
#define _DOS_IHIDDEN	0x002
#define _DOS_ISYS	0x004
#define _DOS_IFVOL	0x008
#define _DOS_IFDIR	0x010
#define _DOS_IFREG	0x020
#define _DOS_IFLNK	0x060 /* IFLNK | IFREG */
#define _DOS_IEXEC	0x080
#define _DOS_IFMT	0x078

#define DOS_IRDONLY	0x001
#define DOS_IHIDDEN	0x002
#define DOS_ISYS	0x004
#define DOS_IFVOL	0x008
#define DOS_IFDIR	0x010
#define DOS_IFREG	0x020
#define DOS_IFLNK	0x060 /* IFLNK | IFREG */
#define DOS_IEXEC	0x080
#define DOS_IFMT	0x078

#define _DOS_ISVOL(m)	(((m) & _DOS_IFMT) == _DOS_IFVOL)
#define _DOS_ISDIR(m)	(((m) & _DOS_IFMT) == _DOS_IFDIR)
#define _DOS_ISREG(m)	(((m) & _DOS_IFMT) == _DOS_IFREG)
#define _DOS_ISLNK(m)	(((m) & _DOS_IFMT) == _DOS_IFLNK)

#define DOS_ISVOL(m)	(((m) & DOS_IFMT) == DOS_IFVOL)
#define DOS_ISDIR(m)	(((m) & DOS_IFMT) == DOS_IFDIR)
#define DOS_ISREG(m)	(((m) & DOS_IFMT) == DOS_IFREG)
#define DOS_ISLNK(m)	(((m) & DOS_IFMT) == DOS_IFLNK)

#define _DEV_STDIN	0x0001
#define _DEV_STDOUT	0x0002
#define _DEV_NUL	0x0004
#define _DEV_CLOCK	0x0008
#define _DEV_RAW	0x0020
#define _DEV_IOCTRL	0x4000
#define _DEV_CDEV	0x8000

#define DEV_STDIN	0x0001
#define DEV_STDOUT	0x0002
#define DEV_NUL	0x0004
#define DEV_CLOCK	0x0008
#define DEV_RAW	0x0020
#define DEV_IOCTRL	0x4000
#define DEV_CDEV	0x8000

#define _DOSE_MAX       39

#define _DOSE_ILGFNC	-1
#define _DOSE_NOENT	-2
#define _DOSE_NODIR	-3
#define _DOSE_MFILE	-4
#define _DOSE_ISDIR	-5
#define _DOSE_BADF	-6
#define _DOSE_BROKNMEM	-7
#define _DOSE_NOMEM	-8
#define _DOSE_ILGMPTR	-9
#define _DOSE_ILGENV	-10
#define _DOSE_ILGFMT	-11
#define _DOSE_ILGARG	-12
#define _DOSE_ILGFNAME	-13
#define _DOSE_ILGPARM	-14
#define _DOSE_ILGDRV	-15
#define _DOSE_ISCURDIR	-16
#define _DOSE_CANTIOC	-17
#define _DOSE_NOMORE	-18
#define _DOSE_RDONLY	-19
#define _DOSE_EXISTDIR	-20
#define _DOSE_NOTEMPTY	-21
#define _DOSE_CANTREN	-22
#define _DOSE_DISKFULL	-23
#define _DOSE_DIRFULL	-24
#define _DOSE_CANTSEEK	-25
#define _DOSE_SUPER	-26
#define _DOSE_DUPTHNAM	-27
#define _DOSE_CANTSEND	-28
#define _DOSE_THFULL	-29
#define _DOSE_LCKFULL	-32
#define _DOSE_LCKERR	-33
#define _DOSE_BUSYDRV	-34
#define _DOSE_SYMLOOP	-35
#define _DOSE_FEXIST	-36	/* doseml */ 
#define _DOSE_BUFOVER	-37	/* doseml */
#define _DOSE_NOSYS	-38	/* doseml */
#define _DOSE_ALRDYEX	-39	/* doseml */
#define _DOSE_EXISTFILE	-80

#define DOSE_MAX       39

#define DOSE_ILGFNC	-1
#define DOSE_NOENT	-2
#define DOSE_NODIR	-3
#define DOSE_MFILE	-4
#define DOSE_ISDIR	-5
#define DOSE_BADF	-6
#define DOSE_BROKNMEM	-7
#define DOSE_NOMEM	-8
#define DOSE_ILGMPTR	-9
#define DOSE_ILGENV	-10
#define DOSE_ILGFMT	-11
#define DOSE_ILGARG	-12
#define DOSE_ILGFNAME	-13
#define DOSE_ILGPARM	-14
#define DOSE_ILGDRV	-15
#define DOSE_ISCURDIR	-16
#define DOSE_CANTIOC	-17
#define DOSE_NOMORE	-18
#define DOSE_RDONLY	-19
#define DOSE_EXISTDIR	-20
#define DOSE_NOTEMPTY	-21
#define DOSE_CANTREN	-22
#define DOSE_DISKFULL	-23
#define DOSE_DIRFULL	-24
#define DOSE_CANTSEEK	-25
#define DOSE_SUPER	-26
#define DOSE_DUPTHNAM	-27
#define DOSE_CANTSEND	-28
#define DOSE_THFULL	-29
#define DOSE_LCKFULL	-32
#define DOSE_LCKERR	-33
#define DOSE_BUSYDRV	-34
#define DOSE_SYMLOOP	-35
#define DOSE_FEXIST	-36	/* doseml */ 
#define DOSE_BUFOVER	-37	/* doseml */
#define DOSE_NOSYS	-38	/* doseml */
#define DOSE_ALRDYEX	-39	/* doseml */
#define DOSE_EXISTFILE	-80

#define _BREAKCK_OFF	0
#define _BREAKCK_ON	1
#define _BREAKCK_KILL	2
#define _BREAKCK_CHECK	-1
#define _CTRL_C_OFF	0x0100
#define _CTRL_C_ON	0x0101
#define _CTRL_C_CHECK	0x01ff
#define _CTRL_N_OFF	0x0200
#define _CTRL_N_ON	0x0201
#define _CTRL_N_CHECK	0x02ff
#define _CTRL_P_ON	0x0301
#define _CTRL_P_OFF	0x0300
#define _CTRL_P_CHECK	0x03ff
#define _CTRL_Q_OFF	0x0400
#define _CTRL_Q_ON	0x0401
#define _CTRL_Q_CHECK	0x04ff
#define _CTRL_S_OFF	0x0500
#define _CTRL_S_ON	0x0501
#define _CTRL_S_CHECK	0x05ff

#define BREAKCK_CHECK	-1
#define BREAKCK_KILL	2
#define BREAKCK_ON	1
#define BREAKCK_OFF	0
#define CTRL_C_CHECK	0x01ff
#define CTRL_C_ON	0x0101
#define CTRL_C_OFF	0x0100
#define CTRL_N_CHECK	0x02ff
#define CTRL_N_ON	0x0201
#define CTRL_N_OFF	0x0200
#define CTRL_P_CHECK	0x03ff
#define CTRL_P_ON	0x0301
#define CTRL_P_OFF	0x0300
#define CTRL_Q_CHECK	0x04ff
#define CTRL_Q_ON	0x0401
#define CTRL_Q_OFF	0x0400
#define CTRL_S_CHECK	0x05ff
#define CTRL_S_ON	0x0501
#define CTRL_S_OFF	0x0500

#ifndef __dosmode_t_defined__
#define __dosmode_t_defined__
typedef int dosmode_t;
#endif

struct dos_inpptr {
	unsigned char	max;
	unsigned char	length;
	char		buffer[256];
};
#define INPPTR dos_inpptr

struct dos_nameckbuf {
	char	drive[2];
	char	path[65];
	char	name[19];
	char	ext[5];
};
#define NAMECKBUF dos_nameckbuf

union dos_fcb {
	struct {
		unsigned char	dupcnt;
		unsigned char	devattr;
		void		*deventry;
		char		nouse_1[8];
		unsigned char	openmode;
		char		nouse_2[21];
		char		name1[8];
		char		ext[3];
		char		nouse_3;
		char		name2[10];
		char		nouse_4[38];
	} __attribute__((__packed__)) chr;
	struct {
		unsigned char	dupcnt;
		unsigned	mode	: 1;
		unsigned	unknown : 2;
		unsigned	physdrv : 5;
		void		*deventry;
		unsigned int	fileptr;
		unsigned int	exclptr;
		unsigned char	openmode;
		unsigned char	entryidx;
		unsigned char	clustidx;
		char		nouse_2;
		unsigned short	acluster;
		unsigned int	asector;
		void		*iobuf;
		unsigned long	dirsec;
		unsigned int	fptrmax;
		char		name1[8];
		char		ext[3];
		unsigned char	attr;
		char		name2[10];
		unsigned short	time;
		unsigned short	date;
		unsigned short	fatno;
		unsigned long	size;
		char		nouse_4[28];
	} __attribute__((__packed__)) blk;
};
#define FCBS dos_fcb

struct dos_indos {
	unsigned short	indosf;
	unsigned char	doscmd;
	unsigned char	fat_flg;
	unsigned short	retry_count;
	unsigned short	retry_time;
	unsigned short	verifyf;
	unsigned char	breakf;
	unsigned char	ctrlpf;
	unsigned char	reserved;
	unsigned char	wkcurdrv;
};

struct dos_mep {
	void	*prev_mp;
	void	*parent_mp;
	void	*block_end;
	void	*next_mp;
};

struct dos_psp {
	char		*env;
	void		*exit;
	void		*ctrlc;
	void		*errexit;
	struct dos_comline	*comline;
	unsigned char	handle[12];
	void		*bss;
	void		*heap;
	void		*stack;
	void		*usp;
	void		*ssp;
	unsigned short	sr;
	unsigned short	abort_sr;
	void		*abort_ssp;
	void		*trap10;
	void		*trap11;
	void		*trap12;
	void		*trap13;
	void		*trap14;
	unsigned int	osflg;
	unsigned char	reserve_1[28];
	char		exe_path[68];
	char		exe_name[24];
	char		reserve_2[36];
};
#define PDBADR dos_psp

struct dos_comline {
	unsigned char	len;
	char		buffer[255];
};
#define COMLINE dos_comline

struct dos_namestbuf {
	unsigned char	flg;
	unsigned char	drive;
	char		path[65];
	char		name1[8];
	char		ext[3];
	char		name2[10];
};
#define NAMESTBUF dos_namestbuf

struct dos_freeinf {
	unsigned short	free;
	unsigned short	max;
	unsigned short	sec;
	unsigned short	byte;
};
#define FREEINF dos_freeinf

struct dos_dpbptr {
	unsigned char	drive;
	unsigned char	unit;
	unsigned short	byte;
	unsigned char	sec;
	unsigned char	shift;
	unsigned short	fatsec;
	unsigned char	fatcount;
	unsigned char	fatlen;
	unsigned short	dircount;
	unsigned short	datasec;
	unsigned short	maxfat;
	unsigned short	dirsec;
	int		driver;
	unsigned char	ide;
	unsigned char	flg;
	struct dos_dpbptr *next;
	unsigned short	dirfat;
	char		dirbuf[64];
} __attribute__((__packed__)) __attribute__((aligned (2)));
#define DPBPTR dos_dpbptr

struct dos_filbuf {
	unsigned char	searchatr;
	unsigned char	driveno;
	unsigned long	dirsec;
	unsigned short	dirlft;
	unsigned short	dirpos;
	char		filename[8];
	char		ext[3];
	unsigned char	atr;
	unsigned short	time;
	unsigned short	date;
	unsigned int	filelen;
	char		name[23];
} __attribute__((__packed__)) __attribute__((aligned (2)));
#define FILBUF dos_filbuf

struct dos_exfilbuf {
	unsigned char	searchatr;
	unsigned char	driveno;
	unsigned long	dirsec;
	unsigned short	dirlft;
	unsigned short	dirpos;
	char		filename[8];
	char		ext[3];
	unsigned char	atr;
	unsigned short	time;
	unsigned short	date;
	unsigned int	filelen;
	char		name[23];
	char		drive[2];
	char		path[65];
	char		unused[21];
} __attribute__((__packed__)) __attribute__((aligned (2)));
#define EXFILBUF dos_exfilbuf

struct dos_dregs {
	int	d0;
	int	d1;
	int	d2;
	int	d3;
	int	d4;
	int	d5;
	int	d6;
	int	d7;
	int	a0;
	int	a1;
	int	a2;
	int	a3;
	int	a4;
	int	a5;
	int	a6;
};
#define DREGS dos_dregs

struct dos_prcctrl {
	long		length;
	unsigned char	*buf_ptr;
	unsigned short	command;
	unsigned short	your_id;
};
#define PRCCTRL dos_prcctrl

struct dos_prcptr {
	struct dos_prcptr *next_ptr;
	unsigned char	wait_flg;
	unsigned char	counter;
	unsigned char	max_counter;
	unsigned char	doscmd;
	unsigned int	psp_id;
	unsigned int	usp_reg;
	unsigned int	d_reg[8];
	unsigned int	a_reg[7];
	unsigned short	sr_reg;
	unsigned int	pc_reg;
	unsigned int	ssp_reg;
	unsigned short	indosf;
	unsigned int	indosp;
	struct dos_prcctrl *buf_ptr;
	unsigned char	name[16];
	long		wait_time;
} __attribute__((__packed__));
#define PRCPTR dos_prcptr

__BEGIN_DECLS

extern void	_dos_allclose (void);
#define ALLCLOSE _dos_allclose
extern int	_dos_bindno (const char *, const char *);
#define BINDNO _dos_bindno
extern int	_dos_breakck (int);
#define BREAKCK _dos_breakck
extern int	_dos_c_cls_al (void);
#define C_CLS_AL _dos_c_cls_al
extern int	_dos_c_cls_ed (void);
#define C_CLS_ED _dos_c_cls_ed
extern int	_dos_c_cls_st (void);
#define C_CLS_ST _dos_c_cls_st
extern int	_dos_c_color (int);
#define C_COLOR _dos_c_color
extern int	_dos_c_curoff (void);
#define C_CUROFF _dos_c_curoff
extern int	_dos_c_curon (void);
#define C_CURON _dos_c_curon
extern int	_dos_c_del (int);
#define C_DEL _dos_c_del
extern int	_dos_c_down (int);
#define C_DOWN _dos_c_down
extern int	_dos_c_down_s (void);
#define C_DOWN_S _dos_c_down_s
extern int	_dos_c_era_al (void);
#define C_ERA_AL _dos_c_era_al
extern int	_dos_c_era_ed (void);
#define C_ERA_ED _dos_c_era_ed
extern int	_dos_c_era_st (void);
#define C_ERA_ST _dos_c_era_st
extern int	_dos_c_fnkmod (int);
#define C_FNKMOD _dos_c_fnkmod
extern int	_dos_c_ins (int);
#define C_INS _dos_c_ins
extern int	_dos_c_left (int);
#define C_LEFT _dos_c_left
extern int	_dos_c_locate (int, int);
#define C_LOCATE _dos_c_locate
extern int	_dos_c_print (const char *);
#define C_PRINT _dos_c_print
extern int	_dos_c_putc (int);
#define C_PUTC _dos_c_putc
extern int	_dos_c_right (int);
#define C_RIGHT _dos_c_right
extern int	_dos_c_up (int);
#define C_UP _dos_c_up
extern int	_dos_c_up_s (void);
#define C_UP_S _dos_c_up_s
extern int	_dos_c_width (int);
#define C_WIDTH _dos_c_width
extern int	_dos_c_window (int, int);
#define C_WINDOW _dos_c_window
extern void	_dos_change_pr (void);
#define CHANGE_PR _dos_change_pr
extern int	_dos_chdir (const char *);
#define CHDIR _dos_chdir
extern int	_dos_chgdrv (int);
#define CHGDRV _dos_chgdrv
extern dosmode_t	_dos_chmod (const char *, dosmode_t);
#define CHMOD _dos_chmod
extern int	_dos_cinsns (void);
#define CINSNS _dos_cinsns
extern int	_dos_close (int);
#define CLOSE _dos_close
extern int	_dos_cominp (void);
#define COMINP _dos_cominp
extern int	_dos_common_ck (const char *);
#define COMMON_CK _dos_common_ck
extern int	_dos_common_del (const char *);
#define COMMON_DEL _dos_common_del
extern int	_dos_common_fre (const char *, int, int, int);
#define COMMON_FRE _dos_common_fre
extern int	_dos_common_lk (const char *, int, int, int);
#define COMMON_LK _dos_common_lk
extern int	_dos_common_rd (const char *, int, char *, int);
#define COMMON_RD _dos_common_rd
extern int	_dos_common_wt (const char *, int, const char *, int);
#define COMMON_WT _dos_common_wt
extern void	_dos_comout (int);
#define COMOUT _dos_comout
extern int	_dos_consns (void);
#define CONSNS _dos_consns
extern int	_dos_coutsns (void);
#define COUTSNS _dos_coutsns
extern int	_dos_create (const char *, dosmode_t);
#define CREATE _dos_create
extern void	_dos_ctlabort (void) __attribute__((__noreturn__));
#define CTRLVC 0xFFF1
extern int	_dos_curdir (int, char *);
#define CURDIR _dos_curdir
extern int	_dos_curdrv (void);
#define CURDRV _dos_curdrv
extern int	_dos_delete (const char *);
#define DELETE _dos_delete
extern void	_dos_diskred (void *, int, int, int);
#define DISKRED _dos_diskred
extern void	_dos_diskred2 (void *, int, int, int);
#define DISKRED2 _dos_diskred2
extern void	_dos_diskwrt (const void *, int, int, int);
#define DISKWRT _dos_diskwrt
extern void	_dos_diskwrt2 (const void *, int, int, int);
#define DISKWRT2 _dos_diskwrt2
extern int	_dos_drvctrl (int, int);
#define DRVCTRL _dos_drvctrl
extern void	_dos_drvxchg (int, int);
#define DRVXCHG _dos_drvxchg
extern int	_dos_dskfre (int, struct dos_freeinf *);
#define DSKFRE _dos_dskfre
extern int	_dos_dup (int);
#define DUP _dos_dup
extern int	_dos_dup0 (int, int);
#define DUP0 _dos_dup0
extern int	_dos_dup2 (int, int);
#define DUP2 _dos_dup2
extern void	_dos_errabort (void) __attribute__((__noreturn__));
#define ERRJVC 0xFFF2
extern int	_dos_exec2 (int, const char *, const char *, const char *);
#define EXEC2 _dos_exec2
extern int	_dos_execonly (void *);
#define EXECONLY _dos_execonly
extern int	_dos_exfiles (struct dos_exfilbuf *, const char *, int);
#define EXFILES _dos_exfiles
extern void	_dos_exit (void) __attribute__((__noreturn__));
#define EXIT _dos_exit
extern void	_dos_exit2 (int) __attribute__((__noreturn__));
#define EXIT2 _dos_exit2
extern int	_dos_exnfiles (struct dos_exfilbuf *);
#define EXNFILES _dos_exnfiles
extern int	_dos_fatchk (const char *, unsigned short *);
#define FATCHK _dos_fatchk
extern int	_dos_fatchk2 (const char *, unsigned short *, int);
#define FATCHK2 _dos_fatchk2
extern void	_dos_fflush (void);
#define FFLUSH _dos_fflush
extern int	_dos_fflush_set (int);
#define FFLUSH_SET _dos_fflush_set
extern int	_dos_fgetc (int);
#define FGETC _dos_fgetc
extern int	_dos_fgets (struct dos_inpptr *, int);
#define FGETS _dos_fgets
extern int	_dos_filedate (int, int);
#define FILEDATE _dos_filedate
extern int	_dos_filelength (FILE *);
#define FILELENGTH _dos_filelength
extern int	_dos_files (struct dos_filbuf *, const char *, int);
#define FILES _dos_files
extern void	_dos_fnckeygt (int, char *);
#define FNCKEYGT _dos_fnckeygt
extern void	_dos_fnckeyst (int, const char *);
#define FNCKEYST _dos_fnckeyst
extern void	_dos_fputc (int, void *);
#define FPUTC _dos_fputc
extern void	_dos_fputs (const char *, void *);
#define FPUTS _dos_fputs
extern union dos_fcb *	_dos_get_fcb_adr (unsigned int);
#define GET_FCB_ADR _dos_get_fcb_adr
extern int	_dos_get_pr (int, struct dos_prcptr *);
#define GET_PR _dos_get_pr
extern int	_dos_getassign (const char *, char *);
#define GETASSIGN _dos_getassign
extern int	_dos_getc (void);
#define GETC _dos_getc
extern int	_dos_getchar (void);
#define GETCHAR _dos_getchar
extern int	_dos_getdate (void);
#define GETDATE _dos_getdate
extern int	_dos_getdpb (int, struct dos_dpbptr *);
#define GETDPB _dos_getdpb
extern int	_dos_getenv (const char *, const char *, char *);
#define GETENV _dos_getenv
extern struct dos_psp *	_dos_getpdb (void);
#define GETPDB _dos_getpdb
extern int	_dos_gets (struct dos_inpptr *);
#define GETS _dos_gets
extern int	_dos_getss (struct dos_inpptr *);
#define GETSS _dos_getss
extern int	_dos_gettim2 (void);
#define GETTIM2 _dos_gettim2
extern int	_dos_gettime (void);
#define GETTIME _dos_gettime
extern void	_dos_hendspic (int);
#define HENDSPIC _dos_hendspic
extern int	_dos_hendspio (void);
#define HENDSPIO _dos_hendspio
extern int	_dos_hendspip (int, const char *);
#define HENDSPIP _dos_hendspip
extern int	_dos_hendspir (int, const char *);
#define HENDSPIR _dos_hendspir
extern void	_dos_hendspmc (void);
#define HENDSPMC _dos_hendspmc
extern int	_dos_hendspmo (void);
#define HENDSPMO _dos_hendspmo
extern int	_dos_hendspmp (int, const char *);
#define HENDSPMP _dos_hendspmp
extern int	_dos_hendspmr (int, const char *);
#define HENDSPMR _dos_hendspmr
extern void	_dos_hendspsc (void);
#define HENDSPSC _dos_hendspsc
extern int	_dos_hendspso (void);
#define HENDSPSO _dos_hendspso
extern int	_dos_hendspsp (int, const char *);
#define HENDSPSP _dos_hendspsp
extern int	_dos_hendspsr (int, const char *);
#define HENDSPSR _dos_hendspsr
extern struct dos_indos *_dos_indosflg (void);
#define INDOSFLG _dos_indosflg
extern int	_dos_inkey (void);
#define INKEY _dos_inkey
extern int	_dos_inpout (int);
#define INPOUT _dos_inpout
extern void *_dos_intvcg (int);
#define INTVCG _dos_intvcg
extern void *_dos_intvcs (int, void *);
#define INTVCS _dos_intvcs
extern int	_dos_ioctrldvctl (int, int, char *);
#define IOCTRLDVCTL _dos_ioctrldvctl
extern int	_dos_ioctrldvgt (int);
#define IOCTRLDVGT _dos_ioctrldvgt
extern int	_dos_ioctrlfdctl (int, int, char *);
#define IOCTRLFDCTL _dos_ioctrlfdctl
extern int	_dos_ioctrlfdgt (int);
#define IOCTRLFDGT _dos_ioctrlfdgt
extern int	_dos_ioctrlgt (int);
#define IOCTRLGT _dos_ioctrlgt
extern int	_dos_ioctrlis (int);
#define IOCTRLIS _dos_ioctrlis
extern int	_dos_ioctrlos (int);
#define IOCTRLOS _dos_ioctrlos
extern int	_dos_ioctrlrd (int, char *, int);
#define IOCTRLRD _dos_ioctrlrd
extern int	_dos_ioctrlrh (int, char *, int);
#define IOCTRLRH _dos_ioctrlrh
extern int	_dos_ioctrlrtset (int, int);
#define IOCTRLRTSET _dos_ioctrlrtset
extern int	_dos_ioctrlst (int, int);
#define IOCTRLST _dos_ioctrlst
extern int	_dos_ioctrlwd (int, const char *, int);
#define IOCTRLWD _dos_ioctrlwd
extern int	_dos_ioctrlwh (int, const char *, int);
#define IOCTRLWH _dos_ioctrlwh
extern void	_dos_k_insmod (int);
#define K_INSMOD _dos_k_insmod
extern int	_dos_k_keybit (int);
#define K_KEYBIT _dos_k_keybit
extern int	_dos_k_keyinp (void);
#define K_KEYINP _dos_k_keyinp
extern int	_dos_k_keysns (void);
#define K_KEYSNS _dos_k_keysns
extern int	_dos_k_sftsns (void);
#define K_SFTSNS _dos_k_sftsns
extern void	_dos_keeppr (int, int) __attribute__((__noreturn__));
#define KEEPPR _dos_keeppr
extern int	_dos_keysns (void);
#define KEYSNS _dos_keysns
extern int	_dos_kflushgc (void);
#define KFLUSHGC _dos_kflushgc
extern int	_dos_kflushgp (void);
#define KFLUSHGP _dos_kflushgp
extern int	_dos_kflushgs (struct dos_inpptr *);
#define KFLUSHGS _dos_kflushgs
extern int	_dos_kflushin (void);
#define KFLUSHIN _dos_kflushin
extern int	_dos_kflushio (int);
#define KFLUSHIO _dos_kflushio
extern int	_dos_kill_pr (void);
#define KILL_PR _dos_kill_pr
extern int	_dos_load (const char *, const struct dos_comline *, const char *);
#define LOAD _dos_load
extern int	_dos_loadexec (const char *, const struct dos_comline *, const char *);
#define LOADEXEC _dos_loadexec
extern int	_dos_loadonly (const char *, const void *, const void *);
#define LOADONLY _dos_loadonly
extern int	_dos_lock (int, int, int);
#define LOCK _dos_lock
extern int	_dos_makeassign (const char *, const char *, int);
#define MAKEASSIGN _dos_makeassign
extern int	_dos_maketmp (const char *, int);
#define MAKETMP _dos_maketmp
extern void *_dos_malloc (int);
#define MALLOC _dos_malloc
extern void *_dos_malloc0 (int, int, int);
#define MALLOC0 _dos_malloc0
extern void *_dos_malloc2 (int, int);
#define MALLOC2 _dos_malloc2
extern int	_dos_memcpy (void *, void *, int);
#define BUS_ERR _dos_memcpy
extern int	_dos_mfree (void *);
#define MFREE _dos_mfree
extern int	_dos_mkdir (const char *);
#define MKDIR _dos_mkdir
extern int	_dos_move (const char *, const char *);
#define MOVE _dos_move
extern int	_dos_nameck (const char *, struct dos_nameckbuf *);
#define NAMECK _dos_nameck
extern int	_dos_namests (const char *, struct dos_namestbuf *);
#define NAMESTS _dos_namests
extern int	_dos_newfile (const char *, dosmode_t);
#define NEWFILE _dos_newfile
extern int	_dos_nfiles (struct dos_filbuf *);
#define NFILES _dos_nfiles
extern int	_dos_open (const char *, int);
#define OPEN _dos_open
extern int	_dos_open_pr (const char *, int, int, int, int, int, struct dos_prcctrl *, long);
#define OPEN_PR _dos_open_pr
extern void *	_dos_os_patch (unsigned int, void *);
#define OS_PATCH _dos_os_patch
extern int	_dos_pathchk (const char *, const struct dos_comline *, const char *);
#define PATHCHK _dos_pathchk
extern void	_dos_print (const char *);
#define PRINT _dos_print
extern void	_dos_prnout (int);
#define PRNOUT _dos_prnout
extern int	_dos_prnsns (void);
#define PRNSNS _dos_prnsns
extern void	_dos_pspset (struct dos_psp *);
#define PSPSET _dos_pspset
extern void	_dos_putchar (int);
#define PUTCHAR _dos_putchar
extern int	_dos_rassign (const char *);
#define RASSIGN _dos_rassign
extern int	_dos_read (int, char *, int);
#define READ _dos_read
extern void	_dos_retshell (void) __attribute__((__noreturn__));
#define EXITVC 0xFFF0
extern int	_dos_rmdir (const char *);
#define RMDIR _dos_rmdir
extern void *_dos_s_malloc (int, int);
#define S_MALLOC _dos_s_malloc
extern void *_dos_s_malloc0 (int, int, int);
#define S_MALLOC0 _dos_s_malloc0
extern int	_dos_s_mfree (void *);
#define S_MFREE _dos_s_mfree
extern int	_dos_s_process (int, int, int, int);
#define S_PROCESS _dos_s_process
extern long	_dos_seek (int, int, int);
#define SEEK _dos_seek
extern int	_dos_send_pr (int, int, int, char *, long);
#define SEND_PR _dos_send_pr
extern int	_dos_setblock (void *, int);
#define SETBLOCK _dos_setblock
extern int	_dos_setdate (int);
#define SETDATE _dos_setdate
extern int	_dos_setenv (const char *, const char *, const char *);
#define SETENV _dos_setenv
extern struct dos_psp *	_dos_setpdb (struct dos_psp *);
#define SETPDB _dos_setpdb
extern int	_dos_settim2 (int);
#define SETTIM2 _dos_settim2
extern int	_dos_settime (int);
#define SETTIME _dos_settime
extern int	_dos_sleep_pr (long);
#define SLEEP_PR _dos_sleep_pr
extern int	_dos_super (int);
#define SUPER _dos_super
extern void	_dos_super_jsr (void (*)(void), struct dos_dregs *, struct dos_dregs *);
#define SUPER_JSR _dos_super_jsr
extern int	_dos_suspend_pr (int);
#define SUSPEND_PR _dos_suspend_pr
extern long	_dos_time_pr (void);
#define TIME_PR _dos_time_pr
extern int	_dos_unlock (int, int, int);
#define UNLOCK _dos_unlock
extern void	_dos_verify (int);
#define VERIFY _dos_verify
extern int	_dos_verifyg (void);
#define VERIFYG _dos_verifyg
extern int	_dos_vernum (void) __attribute_pure__;
#define VERNUM _dos_vernum
extern int	_dos_wait (void);
#define WAIT _dos_wait
extern int	_dos_write (int, const char *, int);
#define WRITE _dos_write

__END_DECLS

#endif /* _DOS_H */
