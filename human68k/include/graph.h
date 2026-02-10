/*
 * graph.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__GRAPH_H
#define	__GRAPH_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

void	apage(char);
void	box(int, int, int, int, int, int);
void	circle(int, int, int, int, int, int, int);
void	contrast(int);
void	fill(int, int, int, int, int);
void	get(int, int, int, int, void *, int);
void	home(int, int, int);
int	hsv(int, int, int);
void	line(int, int, int, int, int, int);
void	paint(int, int, int);
void	palet(int, int);
int	point(int, int);
void	pset(int, int, int);
int	rgb(int, int, int);
void	vpage(char);
void	window(int, int, int, int);
void	wipe(void);
void	put(int, int, int, int, const void *, int);
void	symbol(int, int, const char *, char, char, int, int, int);

#undef	__PROTO_TYPE
#else

void	apage();
void	box();
void	circle();
void	contrast();
void	fill();
void	get();
void	home();
int	hsv();
void	line();
void	paint();
void	palet();
int	point();
void	pset();
void	put();
int	rgb();
void	symbol();
void	vpage();
void	window();
void	wipe();

#endif

#endif
