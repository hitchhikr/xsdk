/*
 * basic0.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__BASIC0_H
#define	__BASIC0_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

static	char	STRCRLF[3]={13,10,0};
static	char	STRTAB[2]={9,0};

#ifdef	__PROTO_TYPE

char	*b_dateS(char *);
char	*b_timeS(char *);
char	*b_dayS(char *);
int	b_free(void);
char	*b_inkeyS(char *);
char	*b_inkey0(char *);
char	*b_striS(char *, int);
char	*b_strfS(char *, double);
int	csrlin(void);
int	pos(void);
char	*b_spaceS(char *, int);
void	beep(void);
void	cls(void);
void	b_exit(int);
void	width(int);
void	color(int);
void	console(int, int, int);
void	b_input(char *, int,...);
void	b_linput(char *, int);
void	locate(int, int);
void	b_iprint(int);
void	b_fprint(double);
void	b_tprint(int);
void	b_ilprint(int);
void	b_flprint(double);
void	b_tlprint(int);
void	screen(int, int, int, int);
double	pi(void);
double	b_pi(double);
int	abs(int);
double	fabs(double);
void	b_csw(int);
void	b_tpalet(int, int, int, int);
void	b_init(void);
int	b_int(double);
char	*using(char *, const char *,...);
void	b_sprint(const char *);
void	b_slprint(const char *);
void	key(int, const char *);
void	b_setdate(const char *);
void	b_settime(const char *);
void	b_strncpy(int, char *, const char *);
int	b_strcmp(const char *, int, const char *);
char	*b_stradd(char *, const char *, const char *,...);

#undef	__PROTO_TYPE
#else

char	*b_dateS();
char	*b_timeS();
char	*b_dayS();
int	b_free();
char	*b_inkeyS();
char	*b_inkey0();
char	*b_striS();
char	*b_strfS();
int	csrlin();
int	pos();
char	*b_spaceS();
char	*using();
void	beep();
void	cls();
void	b_exit();
void	width();
void	color();
void	console();
void	b_input();
void	b_linput();
void	locate();
void	b_sprint();
void	b_iprint();
void	b_fprint();
void	b_tprint();
void	b_slprint();
void	b_ilprint();
void	b_flprint();
void	b_tlprint();
void	key();
void	screen();
double	pi();
double	b_pi();
int	abs();
double	fabs();
void	b_setdate();
void	b_settime();
void	b_csw();
void	b_tpalet();
void	b_strncpy();
int	b_strcmp();
char	*b_stradd();
void	b_init();
int	b_int();

#endif

#endif
