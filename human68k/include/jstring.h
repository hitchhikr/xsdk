/*
 * jstring.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__JSTRING_H
#define	__JSTRING_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	jstrcmp(const unsigned char *, const unsigned char *);
int	jstrncmp(const unsigned char *, const unsigned char *, int); 
unsigned char	*jstrrchr(const unsigned char *, int);
unsigned char	*jstrchr(const unsigned char *, int);

#undef	__PROTO_TYPE
#else

int	jstrcmp();
int	jstrncmp();
unsigned char	*jstrrchr();
unsigned char	*jstrchr();

#endif

#endif
