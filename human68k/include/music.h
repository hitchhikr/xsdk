/*
 * music.h X68k XC Compiler v2.10 Copyright 1992 SHARP/Luvex
 */
#ifndef	__MUSIC_H
#define	__MUSIC_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	m_alloc(char, int);
int	m_assign(char, char);
int	m_atoi(char);
int	m_cont(int, int, int, int, int, int, int, int);
int	m_free(char);
int	m_init(int);
int	m_play(int, int, int, int, int, int, int, int);
int	m_stat(int);
int	m_stop(int, int, int, int, int, int, int, int);
int	m_tempo(int);
int	m_trk(char, const char *);
int	m_vget(char, char *);
int	m_vset(char, const char *);

#undef	__PROTO_TYPE
#else

int	m_alloc();
int	m_assign();
int	m_atoi();
int	m_cont();
int	m_free();
int	m_init();
int	m_play();
int	m_stat();
int	m_stop();
int	m_tempo();
int	m_trk();
int	m_vget();
int	m_vset();

#endif

#endif
