/*
 * audio.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__AUDIO_H
#define	__AUDIO_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

void	a_rec(void *, int, int, int);
int	a_stat(void);
void	a_end(void);
void	a_stop(void);
void	a_cont(void);
void	a_play(const void *, int, int, int, int);

#undef	__PROTO_TYPE
#else

void	a_play();
void	a_rec();
int	a_stat();
void	a_end();
void	a_stop();
void	a_cont();

#endif

#endif
