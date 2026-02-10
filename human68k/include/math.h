/*
 * math.h X68k XC Compiler v2.10 Copyright 1990,91,92 SHARP/Hudson
 */
#ifndef	__MATH_H
#define	__MATH_H

#ifdef	FORWORD
#define	__PROTO_TYPE
#endif
#ifdef	__STDC__
#define	__PROTO_TYPE
#endif

/***** math exceptions ****/
struct	exception	{
	int	type;
	char	*name;
	double	arg1, arg2;
	double	retval;
};

struct	complex	{
	double	x,y;
};

/** Exception type codes **/
#define	DOMAIN		1
#define	SING		2
#define	OVERFLOW	3
#define	UNDERFLOW	4
#define	TLOSS		5
#define	PLOSS		6

#define	PI		3.1415926535898
#define	PID2		1.5707963267949		/* PI/2			*/
#define	PID4		0.78539816339745	/* PI/4			*/
#define	I_PI		0.31830988618379	/* 1/PI			*/
#define	I_PID2		0.63661977236758	/* 1/PID2		*/
#ifndef	HUGE_VAL
#define	HUGE_VAL	3.5953862697247E+308	/* huge double value	*/
/*			1.1125369292536E-308				*/
#endif

extern	volatile int	errno;

#ifdef	__PROTO_TYPE

double	acos(double);
double	asin(double);
double	atan(double);
double	atan2(double, double);
double	cos(double);
double	sin(double);
double	tan(double);
double	cosh(double);
double	sinh(double);
double	tanh(double);
double	exp(double);
double	frexp(double, int *);
double	ldexp(double, int);
double	log(double);
double	log10(double);
double	modf(double, double *);
double	pow(double, double);
double	sqrt(double);
double	ceil(double);
double	fabs(double);
double	floor(double);
double	fmod(double, double);
double	hypot(double, double);
double	except(int, char *, double, double, double);
int	matherr(struct exception *);
double	cabs(struct complex *);

#undef	__PROTO_TYPE
#else

double	acos();
double	asin();
double	atan();
double	atan2();
double	cos();
double	sin();
double	tan();
double	cosh();
double	sinh();
double	tanh();
double	exp();
double	frexp();
double	ldexp();
double	log();
double	log10();
double	modf();
double	pow();
double	sqrt();
double	ceil();
double	fabs();
double	floor();
double	fmod();
double	hypot();
double	except();
int	matherr();
double	cabs();

#endif

#endif
