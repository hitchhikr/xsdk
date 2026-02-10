/*
 * jfctype.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__JFCTYPE_H
#define	__JFCTYPE_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

int	iskpun(int);
int	iskmoji(int);
int	iskana(int);
int	isalnmkana(int);
int	isprkana(int);
int	isalkana(int);
int	isgrkana(int);
int	iskanji2(int);
int	jislower(int);
int	iskanji(int);
int	jisupper(int);
int	jisalpha(int);
int	jisl2(int);
int	jisl1(int);
int	jisl0(int);
int	jiszen(int);
int	jisdigit(int);
int	jiskata(int);
int	jishira(int);
int	jiskigou(int);
int	jisspace(int);
int	ispnkana(int);
int	hantozen(int);
int	zentohan(int);

#undef	__PROTO_TYPE
#else

int	iskpun();
int	iskmoji();
int	iskana();
int	isalnmkana();
int	isprkana();
int	isalkana();
int	isgrkana();
int	iskanji2();
int	jislower();
int	iskanji();
int	jisupper();
int	jisalpha();
int	jisl2();
int	jisl1();
int	jisl0();
int	jiszen();
int	jisdigit();
int	jiskata();
int	jishira();
int	jiskigou();
int	jisspace();
int	ispnkana();
int	hantozen();
int	zentohan();

#endif

#endif
