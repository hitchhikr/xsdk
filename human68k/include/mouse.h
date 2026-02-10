/*
 * mouse.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__MOUSE_H
#define	__MOUSE_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	mouse(int);
int	msarea(int, int, int, int);
int	msbtn(int, int, int);
int	mspos(int *, int *);
int	msstat(int *, int *, int *, int *);
int	setmspos(int, int);

#undef	__PROTO_TYPE
#else

int	mouse();
int	msarea();
int	msbtn();
int	mspos();
int	msstat();
int	setmspos();

#endif

#endif
