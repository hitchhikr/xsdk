/*
 *		stick.h
 */

#ifndef	__STICK_H__
#define	__STICK_H__
 
/*	Joystick structure (union)	*/


typedef	union	stick {
	unsigned short	bin;
	struct	{
		unsigned int dummy1	: 9;
		unsigned int rb		: 1;
		unsigned int lb		: 1;
		unsigned int dummy2	: 1;
		unsigned int right	: 1;
		unsigned int left	: 1;
		unsigned int down	: 1;
		unsigned int up		: 1;
	} flg;
} STICK;

STICK JOY( int mode );

#endif
