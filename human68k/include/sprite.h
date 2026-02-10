/*
 * sprite.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__SPRITE_H
#define	__SPRITE_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

void	sp_clr(int, int);
int	sp_color(char, int, int);
void	sp_disp(char);
void	sp_move(char, int, int, int);
void	sp_off(int, int);
void	sp_on(int, int);
void	sp_pat(char, char *, int);
int	sp_init(void);
int	sp_set(char, int, int, int, int);
int	sp_stat(char, char);
int	bg_scroll(char, int, int);
int	bg_set(char, int, int);
int	bg_stat(char, char);
int	bg_fill(char, int);
int	bg_put(char, char, char, int);
int	bg_get(char, char, char);
void	sp_def(char, const char *, int);

#undef	__PROTO_TYPE
#else

void	sp_clr();
int	sp_color();
void	sp_def();
void	sp_disp();
void	sp_move();
void	sp_off();
void	sp_on();
void	sp_pat();
int	sp_init();
int	sp_set();
int	sp_stat();
int	bg_scroll();
int	bg_set();
int	bg_stat();
int	bg_fill();
int	bg_put();
int	bg_get();

#endif

#endif
