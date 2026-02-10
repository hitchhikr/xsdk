/*
 * image.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__IMAGE_H
#define	__IMAGE_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

void	img_set(char, int, int, int, int);
void	img_still(char);
void	img_pos(char);
void	img_color(char);
void	v_cut(char);
void	crt(char);
void	img_ht(int, int, int, int, char, char);
int	img_load(char *, int, int, int);
void	img_scrn(char, char, char);
int	keysns(void);
void	img_home(int, int, char, int, int);
int	img_save(const char *, int, int);
void	img_put(int, int, int, int, const void *, int);

#undef	__PROTO_TYPE
#else

void	img_set();
void	img_still();
void	img_pos();
void	img_color();
void	v_cut();
void	crt();
void	img_ht();
int	img_save();
int	img_load();
void	img_scrn();
void	img_put();
int	keysns();
void	img_home();

#endif

#endif
