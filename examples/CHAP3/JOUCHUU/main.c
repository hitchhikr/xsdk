/*=============================================================*\
| Resident program skeleton                                     |
| Matsuura Hiroaki(hal@kmc.kyoto-u.ac.jp)                       |
| This program is free and without compensation.                |
| Any execution, modification, or distribution of this program  |
| is at your own risk.                                          |
\*=============================================================*/

#include	<stdio.h>
#include	<doslib.h>
#include	<iocslib.h>
#include	<stdlib.h>
#include	<string.h>
#define 	TIMER_C 	0x45		/* Vector number to hook */
#define 	KEYWORD	    "MMV"		/* Keyword */

/*-------------------------------------------------*\
| 0x000 Start of memory management pointer		    |
| 0x010 Start of process management pointer (=_PSP) |
| 0x100 Start of program area (=_PSTA)              |
\*-------------------------------------------------*/
#define 	OFFSET_PROCESS	0x10
#define 	OFFSET_PROGRAM	0x100

/* The following three variables are set in the startup routine (__main). */
extern int _PSP;		/* Start address of the process management pointer */
extern int _PSTA;		/* Start address of the program area */
extern int _HEND;		/* End address of the heap area */

/*=====================================================================*\
| Resident part                                                         |
|                                                                       |
| A routine that performs the actual processing when resident.          |
| It is easier to perform address calculations and other processes      |
| if you place the keyword that serves as a marker when the program     |
| is released from the resident state and the command transfer section  |
| to the resident state at the beginning of the resident state.         |
\*=====================================================================*/
/* Declare variables and functions in asm() as extern */
extern char keyword[4];
extern void *oldvector;
extern int command;
void process_start();
void _start();

/* This is the start address of the program area (=_PSTA) */
asm( "	keyword:	.fill	4,1,0		" );	/* keyword */
asm( "	oldvector:  .long	0		    " );	/* Where to store the original vector */
asm( "	command:	.long	0		    " );	/* Command receiving location (unused) */
asm( "	process_start: 			        " );	/* Resident execution start address */
asm( "	movem.l %d0-%d7/%a0-%a6,-(%sp)	" );	/* Registers saving */
asm( "	jsr 	prcs				    " );	/* Call the resident main routine */
asm( "	movem.l (%sp)+,%d0-%d7/%a0-%a6	" );	/* Registers restoration */
asm( "	move.l	oldvector,-(%sp)	    " );	/* Event routing */
asm( "	rts 						    " );

/*=================================================================================================*\
| Resident main routine                                                                             |
|                                                                                                   |
| This is where you program what you want to process in the resident part.                          |
| Processes that take a long time and processes that manipulate memory are absolutely not allowed.  |
\*=================================================================================================*/
void prcs()
{
	int poi;
	int mx,my,cx,cy;

	MS_CURON();
	poi = MS_CURGT();
	mx = (poi >> 16) & 0xffff;
	my = poi & 0xffff;
	poi = B_LOCATE( -1 , -1 );
	cx = (poi >> 16) & 0xffff;
	cy = poi & 0xffff;
	cx *= 8;
	cy *= 16;
	if( mx > cx )	mx--;
	if( mx < cx )	mx++;
	if( my > cy )	my--;
	if( my < cy )	my++;
	MS_CURST( mx , my );
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
	printf( "\nResident program example\n" );
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
		if( strncmp( KEYWORD , (char*)((int) addr + OFFSET_PROGRAM + (keyword - (int) _start)), 3 ) == 0 ){
			/* Extract the "original vector" stored in the resident part */
			oldvector = (void *) *(int*)((int) addr + OFFSET_PROGRAM + (keyword - (int) _start) + 4);
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
	/* Return the vector */
			B_INTVCS( TIMER_C , oldvector );
			printf( "Returns the processing of vector number $%x to the original vector $%x.\n" , TIMER_C , oldvector );
	/* Unresident */
			printf( "Uninstalling %s.\n" , argv[0] );
			MFREE( (void *) ((int) addr + OFFSET_PROCESS));
			MS_CUROF();
            return 0;
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
				while( (*sft & 1) == 0 )	prcs();
d4: 			B_SUPER( ssp );
				MS_CUROF();
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
    return 0;
}
