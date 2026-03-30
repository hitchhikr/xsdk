/*
 * conio.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__CONIO_H
#define	__CONIO_H

#include	<stdarg.h>

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

char	*cgets(char *);
int	getch(void);
int	getche(void);
int	kbhit(void);
int	putch(int);
int	ungetch(int);
int	cprintf(const char *,...);
int	cputs(const char *);
int	cscanf(const char *,...);
int	vcprintf(const char *, va_list);

#undef	__PROTO_TYPE
#else

char	*cgets();
int	cprintf();
int	cputs();
int	cscanf();
int	getch();
int	getche();
int	kbhit();
int	putch();
int	ungetch();
int	vcprintf();

#endif

#endif
