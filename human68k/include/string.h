/*
 * string.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__STRING_H
#define	__STRING_H

#include	<stddef.h>

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

extern	char	*sys_errlist[];
extern	int	sys_nerr;

char	*strerror(int);
#define	strerror(EXP)	((EXP) <= sys_nerr ? sys_errlist[(EXP)] : "Unknown error\n")

#ifdef	__PROTO_TYPE

char	*strlwr(char *);
char	*strupr(char *);
char	*strset(char *, char);
char	*strrev(char *);
void	strsrt(char **, int);
void	*memcpy(void *, const void *, size_t);
char	*strcpy(char *, const char *);
char	*strncpy(char *, const char *, size_t);
char	*strcat(char *, const char *);
char	*strncat(char *, const char *, size_t);
int	memcmp(const void *, const void *, size_t);
int	memcmpi(const void *, const void *, size_t);
int	memicmp(const void *, const void *, size_t);
int	strcmpi(const char *, const char *);
int	stricmp(const char *, const char *);
int	strcmp(const char *, const char *);
int	strncmp(const char *, const char *, size_t);
void	*memchr(const void *, int, size_t);
char	*strchr(const char *, int);
size_t	strcspn(const char *, const char *);
char	*strpbrk(const char *, const char *);
char	*strrchr(const char *, int);
size_t	strspn(const char *, const char *);
char	*strtok(char *, const char *);
void	*memset(void *, int, size_t);
size_t	strlen(const char *);
char	*strnset(char *, char, size_t);
char	*strdup(const char *);
void	strins(const char *, char *);
void	strsfn(const char *, char *, char *, char *, char *);
void	strmfe(char *, const char *, const char *);
void	strmfn(char *, const char *, const char *, const char *, const char *);
void	strmfp(char *, const char *, const char *);
int	stcgfe(char *, const char *);
int	strbpl(char **, int, const char *);
int	stcgfn(char *, const char *);
void	*memccpy(void *, const void *, char, size_t);
void	movedata(const void *, void *, size_t);
void	setmem(void *, size_t, int);
void	movmem(const void *, void *, size_t);
void	swmem(void *, void *, size_t);
void	repmem(void *, const void *, size_t, int);
char	*strstr(const char *, const char *);
void	*memmove(void *, const void *, size_t);

#undef	__PROTO_TYPE
#else

void	*memcpy();
char	*strcpy();
char	*strncpy();
char	*strcat();
char	*strncat();
int	memcmp();
int	memcmpi();
int	memicmp();
int	strcmpi();
int	stricmp();
int	strcmp();
int	strncmp();
void	*memchr();
char	*strchr();
char	*strpbrk();
char	*strrchr();
char	*strtok();
void	*memset();
char	*strnset();
char	*strlwr();
char	*strupr();
char	*strset();
char	*strrev();
char	*strdup();
void	strins();
void	strsfn();
void	strmfe();
void	strmfn();
void	strmfp();
int	stcgfe();
int	strbpl();
int	stcgfn();
void	strsrt();
void	*memccpy();
void	movedata();
void	setmem();
void	movmem();
void	swmem();
void	repmem();
char	*strstr();
void	*memmove();
size_t	strcspn();
size_t	strspn();
size_t	strlen();

#endif

#endif
