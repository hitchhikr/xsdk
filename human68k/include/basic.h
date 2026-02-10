/*
 * basic.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__BASIC_H
#define	__BASIC_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifdef	__PROTO_TYPE

char	*b_binS(char *, int);
char	*b_chrS(char *, int);
char	*ecvt(double, int, int *, int *);
char	*fcvt(double, int, int *, int *);
double	fix(double);
char	*gcvt(double, int, char *);
char	*b_hexS(char *, int);
char	*b_itoa(char *, int);
char	*b_octS(char *, int);
int	dskf(int);
int	b_fclose(int);
int	b_fcloseall(void);
int	b_feof(int);
int	b_fgetc(int);
int	b_fputc(char, int);
int	b_fread(void *, int, int, int);
int	b_freads(char *, int, int);
int	b_fseek(int, int, int);
double	atan(double);
double	cos(double);
double	exp(double);
double	log(double);
double	pow(double, double);
int	rand(void);
void	randomize(int);
double	rnd(void);
double	sgn(double);
double	sin(double);
double	sqrt(double);
void	srand(unsigned);
double	tan(double);
char	*strnset(char *, char, unsigned);
char	*strrev(char *);
char	*strset(char *, char);

#ifndef	__CTYPE_H
int	isalnum(int);
int	isalpha(int);
int	isascii(int);
int	iscntrl(int);
int	isdigit(int);
int	isgraph(int);
int	islower(int);
int	isprint(int);
int	ispunct(int);
int	isspace(int);
int	isupper(int);
int	isxdigit(int);
int	toascii(int);
int	tolower(int);
int	toupper(int);
#endif

int	asc(const char *);
double	atof(const char *);
int	atoi(const char *);
double	val(const char *);
int	b_fopen(const char *, const char *);
int	b_fwrite(const void *, int, int, int);
int	b_fwrites(const char *, int);
int	instr(int, const char *, const char *);
char	*b_leftS(char *, const char *, int);
char	*b_midS(char *, const char *, int, int);
char	*b_mirrorS(char *, const char *);
char	*b_rightS(char *, const char *, int);
int	b_strchr(const char *, char);
char	*b_stringS(char *, int, const char *);
int	b_strrchr(const char *, char);
char	*b_strtok(char *, const char *);
int	frename(const char *, const char *);
int	fdelete(const char *);
int	child(const char *);

#undef	__PROTO_TYPE
#else

int	asc();
double	atof();
int	atoi();
char	*b_binS();
char	*b_chrS();
char	*ecvt();
char	*fcvt();
double	fix();
char	*gcvt();
char	*b_hexS();
char	*b_itoa();
char	*b_octS();
double	val();
int	dskf();
int	b_fclose();
int	b_fcloseall();
int	b_feof();
int	b_fgetc();
int	b_fopen();
int	b_fputc();
int	b_fread();
int	b_freads();
int	b_fseek();
int	b_fwrite();
int	b_fwrites();
double	atan();
double	cos();
double	exp();
double	log();
double	pow();
int	rand();
void	randomize();
double	rnd();
double	sgn();
double	sin();
double	sqrt();
void	srand();
double	tan();
int	instr();
char	*b_leftS();
char	*b_midS();
char	*b_mirrorS();
char	*b_rightS();
int	b_strchr();
char	*b_stringS();
char	*strnset();
int	b_strrchr();
char	*strrev();
char	*strset();
char	*b_strtok();
int	frename();
int	fdelete();
int	child();

#ifndef	__CTYPE_H
int	isalnum();
int	isalpha();
int	isascii();
int	iscntrl();
int	isdigit();
int	isgraph();
int	islower();
int	isprint();
int	ispunct();
int	isspace();
int	isupper();
int	isxdigit();
int	toascii();
int	tolower();
int	toupper();
#endif

#endif

#endif
