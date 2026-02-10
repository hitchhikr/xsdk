/*
 * stddef.h X68k XC Compiler v2.00 Copyright 1990 SHARP/Hudson
 */
#ifndef	__STDDEF_H
#define	__STDDEF_H

extern	volatile int	errno;

#ifndef	__DIV_T
#define	__DIV_T
typedef	struct	{
	int	quot;
	int	rem;
}	div_t;
typedef	struct	{
	long	quot;
	long	rem;
}	ldiv_t;
#endif

#ifndef	CLK_TCK
#define	CLK_TCK	100.0
#endif

#ifndef	CLOCKS_PAR_SEC
#define	CLOCKS_PAR_SEC	100.0
#endif

#define	offsetof(TYPE,MEMBER)	((size_t)&((TYPE *)0)->MEMBER)

#ifndef	SEEK_SET
#define	SEEK_SET	0
#endif
#ifndef	SEEK_CUR
#define	SEEK_CUR	1
#endif
#ifndef	SEEK_END
#define	SEEK_END	2
#endif

#ifndef	NULL
#define	NULL	0
#endif

#ifndef	EOF
#define	EOF	(-1)
#endif

#ifndef	EOS
#define	EOS	'\0'
#endif

typedef	long	ptrdiff_t;

#ifndef	__SIZE_T
#define	__SIZE_T
typedef	unsigned	size_t;
#endif

#ifndef	__TIME_T
#define	__TIME_T
typedef	long	time_t;
#endif

#ifndef	__CLOCK_T
#define	__CLOCK_T
typedef	long	clock_t;
#endif

#ifndef	__FPOS_T
#define	__FPOS_T
typedef	long	fpos_t;
#endif

#endif
