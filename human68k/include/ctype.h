/*
 * ctype.h X68k XC Compiler v2.00 Copyright 1990 SHARP/Hudson
 */
#ifndef	__CTYPE_H
#define	__CTYPE_H

#define	__U	1
#define	__L	2
#define	__N	4
#define	__S	8
#define	__P	16
#define	__C	32
#define	__B	64
#define	__X	128

extern	char _ctype[];

/*********** ctype macros ****************/
#define	isalpha(CH)	(_ctype[(CH)]&(__U|__L))
#define	isupper(CH)	(_ctype[(CH)]&__U)
#define	islower(CH)	(_ctype[(CH)]&__L)
#define	isdigit(CH)	(_ctype[(CH)]&__N)
#define	isodigit(CH)	((_ctype[(CH)]&__N)&&((CH)<'8'))
#define	isxdigit(CH)	(_ctype[(CH)]&__X)
#define	isspace(CH)	(_ctype[(CH)]&__S)
#define	ispunct(CH)	(_ctype[(CH)]&(__P|__B))
#define	isalnum(CH)	(_ctype[(CH)]&(__U|__L|__N))
#define	isprint(CH)	(_ctype[(CH)]&(__P|__U|__L|__N|__B))
#define	isgraph(CH)	(_ctype[(CH)]&(__P|__U|__L|__N))
#define	iscntrl(CH)	(_ctype[(CH)]&__C)
#define	isascii(CH)	((unsigned)(CH)<=127)
#define	iscsym(CH)	(isalnum(CH)||((CH)==0x5f))
#define	iscsymf(CH)	(isalpha(CH)||((CH)==0x5f))
#define	toupper(CH)	(islower(CH)?((CH)-('a'-'A')):(CH))
#define	tolower(CH)	(isupper(CH)?((CH)+('a'-'A')):(CH))
#define	_toupper(CH)	((CH)-('a'-'A'))
#define	_tolower(CH)	((CH)+('a'-'A'))
#define	toascii(CH)	((CH)&127)

#ifndef	NULL
#define	NULL	0
#endif

#endif
