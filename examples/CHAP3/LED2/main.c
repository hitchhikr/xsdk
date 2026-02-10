/*=================================================================================*\
| A resident program that flashes the LED in sync with music                        |
| programmed by chika@kmc and hal@kmc                                               |
| This program is free and without compensation.                                    |
| Any execution, modification, or distribution of this program is at your own risk. |
\*=================================================================================*/

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<stdlib.h>
#include	<string.h>
#include	"zlib.h"

#define 	TIMER_C 	0x45		/* Vector number to hook */
#define 	KEYWORD	    "LEDX"		 /* keyword */

#define 	OFFSET_PROCESS	0x10
#define 	OFFSET_PROGRAM	0x100

/* The following three variables are set in the startup routine (__main). */
extern int _PSP;		/* Start address of the process management pointer */
extern int _PSTA;		/* Start address of the program area */
extern int _HEND;		/* End address of the heap area */

/*================================================================*\
| Resident part														   |
\*================================================================*/
/* Declare variables and functions in asm() as extern */
extern char keyword[5];
extern void *oldvector;
extern short *seq_tbl;
void process_start();
void _start();

/* This is the start address of the program area (=_PSTA) */
asm( "	keyword:	.fill	5,1,0	    " );	/* keyword */
asm( "              .even               " );
asm( "	oldvector:  .long	0		    " );	/* Where to store the original vector */
asm( "	seq_tbl:	.long	0		    " );	/* Z-MUSIC Worktable start address */
asm( "	process_start: 			        " );	/* Resident execution start address */
asm( "	movem.l %d0-%d7/%a0-%a6,-(%sp)	" );	/* Registers saving */
asm( "	jsr 	led_control		        " );	/* Call the resident main routine */
asm( "	movem.l (%sp)+,%d0-%d7/%a0-%a6	" );	/* Registers restoration */
asm( "	move.l	oldvector,-(%sp)	    " );	/* Event routing */
asm( "	rts 						    " );

/*=================================================*\
| Resident main routine Function to turn LED on/off |
\*=================================================*/
void	led_control( void )
{
	int i;
	unsigned char j = 0;

	/* The track numbers for each LED are,
       from left to right, full-width, hiragana, INS, CAPS, code input, romaji, kana. */
	static int trno[ 7 ] = { 6, 5, 0, 3, 4, 1, 2 };

	/* Look at the work and decide whether to turn on or off each LED.
       If the remaining gate time is 3 or more and there is no rest, it will turn on. */
	for( i = 0; i < 7; i++ ) {
		j *= 2;
		if( *( seq_tbl + 1 + 128 * trno[ i ] ) >= 3 &&
			!(*((unsigned char *)seq_tbl + 0x42 + 256 * trno[ i ] )
			& 0x80 ) ) {
		} else {
				j++;
		}
	}

	/* Send data to the keyboard */
	*(volatile unsigned char *)0xe8802f = j | 0x80;
	*(volatile unsigned char *)0xe8802d = 0x05;
}

/*===================================*\
| From here, the non-resident section |
\*===================================*/
/*==========*\
| How to use |
\*==========*/
void usage( execname , addr )
char *execname;
int *addr;
{
	printf( "\nLED flashing resident program --- zmusic must be resident beforehand\n" );
	printf( "%s {-,/}(option)\n" , execname );
	printf( "option: s   : I will be stationed\n" );
	printf( "        r   : Cancel residency\n" );
	printf( "        t   : Test the execution of a resident routine without resident\n" );
	printf( "              Press SHIFT to exit\n" );
	printf( "        h,? : This will help\n\n" );
	printf( "%s is currently resident\n" , execname );
	if( addr != NULL ){
		printf( "Resident address $%x, vector number $%x (original vector $%x)\n\n" , addr , TIMER_C , oldvector );
	}
	exit( 1 );
}

/*======================*\
| Non-resident part main |
\*======================*/
int main( argc , argv )
int argc;
char **argv;
{
	int ssp;
	int *addr;

/*-------------------------------------------------------------------------------------------*\
| Check if it's already resident.                                                             |
| When this part of the processing is finished, the variables will have the following values: |
|                                                                                             |
| When you were already resident                            When you are not yet resident     |
| addr memory management pointer for the resident process	NULL                              |
\*-------------------------------------------------------------------------------------------*/
d1: ssp = B_SUPER( 0 );
	addr = (int*)(_PSP - OFFSET_PROCESS);	/* Previous memory management pointer */
	oldvector = 0;

	while( addr != NULL ){		/* If it's NULL, then it's over. */
		/* Move to the previous memory management pointer */
		addr = (int*) *addr;
		/* Compare the beginning of the program with keyword */
		if( strncmp( KEYWORD , (char*)((int) addr + OFFSET_PROGRAM + (keyword - (int) _start)), 4 ) == 0 ){
			/* Extract the "original vector" stored in the resident part */
			oldvector = (void *) *(int*)((int) addr + OFFSET_PROGRAM + (keyword - (int) _start) + 6);
			break;
		}
	}
d2: B_SUPER( ssp );
/*------------------------------------------------------------------------------------------*\
| Check the arguments. It's not hospitable to suddenly have someone stay there all the time! |
\*------------------------------------------------------------------------------------------*/
	if( argc == 1 ) 	usage( argv[0] , addr );
	if( ( argv[1][0] != '/' ) && ( argv[1][0] != '-' ) )	usage( argv[0] , addr );
	switch( argv[1][1] ){
		case 'H':
		case 'h':
		case '?':	usage( argv[0] , addr );
/*---------*\
| Permanent |
\*---------*/
		case 's':
		case 'S':
		{
			if( addr != NULL ){
				printf( "%s is already resident. I can't stay here any longer.\n" , argv[0] );
				exit( 1 );
			}
	/* A set of keyword */
			strcpy( keyword , KEYWORD );
	/* Get the start address of the work area */
			seq_tbl = z_get_wk_tbl();
	/* Hooking a Vector */
			oldvector = B_INTVCS( TIMER_C , process_start );
			printf( "Hook vector number $%x. (Original vector $%x)\n" , TIMER_C , oldvector );
	/* End resident */
			printf( "Made %s resident.\n" , argv[0] );
			KEEPPR( _HEND - _PSTA , 0 );
		}
/*---------------*\
| Cancel resident |
\*---------------*/
		case 'r':
		case 'R':
		{
			if( addr == NULL ){
				printf( "%s is not resident.\n" , argv[0] );
				exit( 1 );
			}
	/* Turn the LED back on */		    
			LEDMOD( 2, 0 );
	/* Return the vector */
			B_INTVCS( TIMER_C , oldvector );
			printf( "Returns the processing of vector number $%x to the original vector $%x.\n" , TIMER_C , oldvector );
	/* Unresident */
			printf( "Uninstalling %s.\n" , argv[0] );
			MFREE( (void *) ((int) addr + OFFSET_PROCESS));
			MS_CUROF();
			exit( 0 );
		}
/*-------------------------*\
| Testing Resident Routines |
\*-------------------------*/
		case 't':
		case 'T':
		{
			volatile char *sft = (char*)0x80e;

			if( addr == NULL ){
d3: 			ssp = B_SUPER( 0 );

		/* Get the start address of the work area */
				seq_tbl = z_get_wk_tbl();

				while( (*sft & 1) == 0 )	led_control();

		/* Turn the LED back on */
				LEDMOD( 2, 0 );

d4: 			B_SUPER( ssp );
				exit( 0 );
			}else{
				printf( "%s is already resident. No execution test of resident routines is performed.\n" , argv[0] );
				exit( 1 );
			}
		}
/*---------------*\
| Optional Errors |
\*---------------*/
		default:
		{
			printf( "Invalid option %s\n" , argv[1] );
			usage( argv[0] , addr );
		}
	}
}


