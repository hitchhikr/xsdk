/*
 * stick.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__STICK_H
#define	__STICK_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	stick(int);
int	strig(int);

#undef	__PROTO_TYPE
#else

int	stick();
int	strig();

#endif

#endif
