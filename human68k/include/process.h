/*
 * process.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__PROCESS_H
#define	__PROCESS_H

#define	P_WAIT	0

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	getpid(void);
int	wait(void);
int	execve(const char *, const char **, const char **);
int	execvpe(const char *, const char **, const char **);
int	execl(const char *, const char *,...);
int	execlp(const char *, const char *,...);
int	execv(const char *, const char **);
int	execvp(const char *, const char **);
int	execle(const char *, const char *,...);
int	execlpe(const char *, const char *,...);
int	spawnl(int, const char *, const char *,...);
int	spawnlp(int, const char *, const char *,...);
int	spawnv(int, const char *, const char **);
int	spawnvp(int, const char *, const char **);
int	spawnle(int, const char *, const char *,...);
int	spawnve(int, const char *, const char **, const char **);

#undef	__PROTO_TYPE
#else

int	getpid();
int	wait();
int	execve();
int	execvpe();
int	execl();
int	execlp();
int	execv();
int	execvp();
int	execle();
int	execlpe();
int	spawnl();
int	spawnlp();
int	spawnv();
int	spawnvp();
int	spawnle();
int	spawnve();

#endif

#endif
