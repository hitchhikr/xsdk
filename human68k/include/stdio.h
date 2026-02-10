/*
 * stdio.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__STDIO_H
#define	__STDIO_H

#include	<stddef.h>
#include	<stdarg.h>
#include	<fcntl.h>

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

#ifndef	BUFSIZ
#define	BUFSIZ	1024
#endif
#ifndef	_NFILE
#define	_NFILE	40
#endif
#ifndef	FOPEN_MAX
#define	FOPEN_MAX	40
#endif
/* CLIB.L を変更しなければ意味がありません */

#define	TMP_MAX	65535
#define	L_tmpnam	128
#define	FILENAME_MAX	128
/* CLIB.L で固定の値として使用しています
 * 変更したり、別な値を再定義してはいけません */

typedef	struct	_iobuf	{
	char	*_ptr;
	int	_cnt;
	char	*_base;
	int	_flag;
	int	_bsize;
	char	_file;
	char	_pback;
	char	*_fname;
}	FILE;
extern	FILE	_iob[_NFILE];
extern	unsigned	_liobuf[96];

#define	stdin	(&_iob[0])
#define	stdout	(&_iob[1])
#define	stderr	(&_iob[2])
#define	stdaux	(&_iob[3])
#define	stdprn	(&_iob[4])

#define	_IOFBF		0x00
#define	_IOREAD		0x01
#define	_IOWRT		0x02
#define	_IORW		0x100
#define	_IONBF		0x08
#define	_IOMYBUF	0x10
#define	_IOEOF		0x20
#define	_IOERR		0x40
#define	_IOLBF		0x80

#ifdef	MACRO
	#define getc(f) (--(f)->_cnt >= 0 ? 0xff & *(f)->_ptr++ : _filbuf(f))
	#define putc(c,f) (--(f)->_cnt >= 0 ? 0xff & (*(f)->_ptr++ = (c)) : _flsbuf((c),(f)))
	#define getchar() getc(stdin)
	#define putchar(c) putc((c),stdout)
	#define feof(f) ((f)->_flag & _IOEOF)
	#define ferror(f) ((f)->_flag & _IOERR)
	#define fileno(f) ((f)->_file)
	#define	clearerr(f) (((f)->_flag &= ~(_IOEOF+_IOERR)),(_liobuf[fileno(f)] &= ~O_EOF))
#endif

#ifdef	__PROTO_TYPE

#ifndef	MACRO
int	getc(FILE *);
int	putc(int, FILE *);
int	getchar(void);
int	putchar(int);
int	feof(FILE *);
int	ferror(FILE *);
int	fileno(FILE *);
void	clearerr(FILE *);
#endif

int	fclose(FILE *);
int	fflush(FILE *);
void	setbuf(FILE *, char *);
int	fgetc(FILE *);
char	*fgets(char *, int, FILE *);
int	fputc(int, FILE *);
char	*gets(char *);
int	ungetc(int, FILE *);
int	fseek(FILE *, long, int);
long	ftell(FILE *);
void	rewind(FILE *);
int	fcloseall(void);
int	flushall(void);
int	fputchar(int);
void	clrerr(FILE *);
void	fmode(FILE *, int);
int	getw(FILE *);
int	fgetchar(void);
int	getl(FILE *);
int	putl(long, FILE *);
int	putw(short, FILE *);
void	setnbf(FILE *);
FILE	*tmpfile(void);
char	*tmpnam(char *);
int	rmtmp(void);
int	rmtemp(void);
FILE	*fopen(const char *, const char *);
FILE	*freopen(const char *, const char *, FILE *);
int	setvbuf(FILE *, char *, int, size_t);
int	fprintf(FILE *, const char *,...);
int	vfprintf(FILE *, const char *, va_list);
int	fscanf(FILE *, const char *,...);
int	printf(const char *,...);
int	vprintf(const char *, va_list);
int	scanf(const char *,...);
int	sprintf(char *, const char *,...);
int	vsprintf(char *, const char *, va_list);
int	sscanf(const char *, const char *,...);
int	fputs(const char *, FILE *);
int	puts(const char *);
size_t	fread(void *, size_t, size_t, FILE *);
size_t	fwrite(const void *, size_t, size_t, FILE *);
void	perror(const char *);
FILE	*fdopen(int, const char *);
int	fgetpos(FILE *, fpos_t *);
int	fsetpos(FILE *, const fpos_t *);
int	remove(const char *);
FILE	*tempfile(const char *,const char *);
char	*tempnam(const char *,const char *);

#undef	__PROTO_TYPE
#else

#ifndef	MACRO
int	getc();
int	putc();
int	getchar();
int	putchar();
int	feof();
int	ferror();
int	fileno();
void	clearerr();
#endif

int	fclose();
int	fflush();
FILE	*fopen();
FILE	*freopen();
void	setbuf();
int	setvbuf();
int	fprintf();
int	vfprintf();
int	fscanf();
int	printf();
int	vprintf();
int	scanf();
int	sprintf();
int	vsprintf();
int	sscanf();
int	fgetc();
char	*fgets();
int	fputc();
int	fputs();
char	*gets();
int	puts();
int	ungetc();
int	fseek();
long	ftell();
void	rewind();
void	perror();
int	fcloseall();
int	flushall();
int	fputchar();
void	clrerr();
void	fmode();
int	getw();
int	fgetchar();
int	getl();
int	putl();
int	putw();
void	setnbf();
FILE	*fdopen();
int	fgetpos();
int	fsetpos();
int	remove();
FILE	*tmpfile();
FILE	*tempfile();
char	*tmpnam();
char	*tempnam();
int	rmtmp();
int	rmtemp();
size_t	fread();
size_t	fwrite();

#endif

#endif
