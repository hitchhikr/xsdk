/*
 * stdlib.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__STDLIB_H
#define	__STDLIB_H

#include	<stddef.h>

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifndef	HUGE_VAL
#define	HUGE_VAL	3.5953862697247E+308
#endif

#define	RAND_MAX	32767
/* It is used as a fixed value in libc.a and should not be changed or redefined to a different value. */

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS	0
#endif
#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE	1
#endif

extern	volatile int	errno;
extern	volatile int	_doserrno;
extern	unsigned char	**environ;
extern	unsigned int	_PSP;
extern	int		_fmode;
extern	char		*sys_errlist[];
extern	int		sys_nerr;

#define	max(a,b)	(((a) > (b)) ? (a) : (b))
#define	min(a,b)	(((a) < (b)) ? (a) : (b))

#ifdef	__PROTO_TYPE

int	rand(void);
void	srand(unsigned);
int	free(void *);
void	abort(void);
void	exit(int);
void	_exit(int);
int	abs(int);
short	wabs(short);
long	labs(long);
int	rlsml(char *, long);
void	*getml(long);
void	rbrk(void);
void	rstmem(void);
long	chkml(void);
long	sizmem(void);
int	bldmem(int);
int	allmem(void);
void	lqsort(long *, int);
void	sqsort(short *, int);
void	fqsort(float *, int);
void	dqsort(double *, int);
void	tqsort(char **, int);
char	*itoa(int, char *, int);
char	*ltoa(long, char *, int);
char	*wtoa(short, char *, int);
char	*fcvt(double, int, int *, int *);
char	*gcvt(double, int, char *);
char	*uitoa(unsigned int, char *, int);
char	*ultoa(unsigned long, char *, int);
char	*uwtoa(unsigned short, char *, int);
char	*ecvt(double, int, int *, int *);
int	atexit(void (*)(void));
div_t	div(int, int);
ldiv_t	ldiv(long, long);
int	atoi(const char *);
long	atol(const char *);
void	*calloc(size_t, size_t);
void	*malloc(size_t);
void	*realloc(void *, size_t);
char	*getenv(const char *);
int	system(const char *);
void	*bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *));
void	qsort(void *, size_t, size_t, int (*)(const void *, const void *));
char	*sbrk(size_t);
int	rlsmem(char *, size_t);
void	*getmem(size_t);
int	putenv(const char *);
void	swaw(const int *, int *, int);
void	swab(const char *, char *, int);
short	atow(const char *);
double	atof(const char *);
long	strtol(const char *, char **, int);
unsigned long	strtoul(const char *, char **, int);
double	strtod(const char *, char **);

#undef	__PROTO_TYPE
#else

int	atoi();
long	atol();
int	rand();
void	srand();
void	*calloc();
int	free();
void	*malloc();
void	*realloc();
void	abort();
void	exit();
void	_exit();
char	*getenv();
int	system();
void	*bsearch();
void	qsort();
int	abs();
short	wabs();
long	labs();
char	*sbrk();
int	rlsmem();
int	rlsml();
void	*getmem();
void	*getml();
void	rbrk();
void	rstmem();
long	chkml();
long	sizmem();
int	bldmem();
int	allmem();
int	putenv();
void	swaw();
void	swab();
void	lqsort();
void	sqsort();
void	fqsort();
void	dqsort();
void	tqsort();
char	*itoa();
char	*ltoa();
char	*wtoa();
char	*fcvt();
char	*gcvt();
short	atow();
char	*uitoa();
char	*ultoa();
char	*uwtoa();
char	*ecvt();
double	atof();
long	strtol();
unsigned long	strtoul();
double	strtod();
int	atexit();
div_t	div();
ldiv_t	ldiv();

#endif

#endif
