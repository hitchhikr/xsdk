/*
 * music3.h X68k XC Compiler v2.10 Copyright 1992 SHARP/Luvex
 */
#ifndef	__MUSIC3_H
#define	__MUSIC3_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	m_antoff(int);
int	m_bend(int, char);
int	m_chan(char, int);
int	m_click(int);
int	m_clkchg(int, int);
int	m_ctrlres(int);
int	m_dirout(int, char *);
int	m_enable(int);
int	m_end(int);
int	m_errget(void);
int	m_freea(void);
int	m_ifchk(void);
int	m_in(void);
int	m_mcont(int);
int	m_mdreg(char, char, char);
int	m_meas(void);
int	m_meter(char, char);
int	m_mod(int, char);
int	m_modsns(int);
int	m_mplay(int);
int	m_mstop(int);
int	m_mstvol(int);
int	m_mute(int, int, int, int, int, int, int, int);
int	m_ntoff(char, char, int);
int	m_nton(char, char, int);
int	m_opmexc(int);
int	m_opmlfq(int);
int	m_opmreg(int, int);
int	m_out(char);
int	m_pan(int, int);
int	m_panflt(int, int);
int	m_pcmbsy(void);
int	m_pcmclr(void);
int	m_pcmget(char, char *, int);
int	m_pcmlen(int);
int	m_pcmon(char, int, int);
int	m_pcmrec(char, int, char *);
int	m_pcmset(char, char, int, char *);
int	m_pgmflt(int, int);
char	*m_pnmget(char);
int	m_pnmset(char, char *);
int	m_prog(int, int);
int	m_recint(int);
int	m_reclen(void);
int	m_recset(char, char *, int);
int	m_recsta(int);
int	m_recstp(void);
int	m_sndset(const char *, int);
int	m_solo(int);
int	m_start(int);
int	m_sync(int);
int	m_sysch(char *);
int	m_thru(int);
char	*m_tnmget(char);
int	m_tnmset(char, char *);
int	m_trns(int, int);
int	m_use(int);
int	m_vel(int, int);
int	m_version(void);
int	m_vol(int, int);
int	m_volflt(int, int);
int	m_wrk(int);
int	m_ycom(int);

#undef	__PROTO_TYPE
#else

int	m_antoff();
int	m_bend();
int	m_chan();
int	m_click();
int	m_clkchg();
int	m_ctrlres();
int	m_dirout();
int	m_enable();
int	m_end();
int	m_errget();
int	m_freea();
int	m_ifchk();
int	m_in();
int	m_mcont();
int	m_mdreg();
int	m_meas();
int	m_meter();
int	m_mod();
int	m_modsns();
int	m_mplay();
int	m_mstop();
int	m_mstvol();
int	m_mute();
int	m_ntoff();
int	m_nton();
int	m_opmexc();
int	m_opmlfq();
int	m_opmreg();
int	m_out();
int	m_pan();
int	m_panflt();
int	m_pcmbsy();
int	m_pcmclr();
int	m_pcmget();
int	m_pcmlen();
int	m_pcmon();
int	m_pcmrec();
int	m_pcmset();
int	m_pgmflt();
char	*m_pnmget();
int	m_pnmset();
int	m_prog();
int	m_recint();
int	m_reclen();
int	m_recset();
int	m_recsta();
int	m_recstp();
int	m_sndset();
int	m_solo();
int	m_start();
int	m_sync();
int	m_sysch();
int	m_thru();
char	*m_tnmget();
int	m_tnmset();
int	m_trns();
int	m_use();
int	m_vel();
int	m_version();
int	m_vol();
int	m_volflt();
int	m_wrk();
int	m_ycom();

#endif

#endif
