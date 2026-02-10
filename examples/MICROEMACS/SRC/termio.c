/*
 * The functions in this file negotiate with the operating system for
 * characters, and write characters in a barely buffered fashion on the display.
 * All operating systems.
 */
#include <stdio.h>
#include <string.h>
#include <doslib.h>
#include <iocslib.h>
#include "estruct.h"
#include "edef.h"

#define  termdef 1  /* don't define "term" external */

void f_key0();
void f_key1();
void f_key2();

int nxtchar = -1;   /* character held from type ahead   */
int CRTMODE = 16;

/*
 * This function is called once to set up the terminal device streams.
 * On VMS, it translates TT until it finds the terminal, then assigns
 * a channel to it and sets it raw. On CPM it is a no-op.
 */
ttopen()
{
    /* on all screens we are not sure of the initial position of the cursor */
    ttrow = 999;
    ttcol = 999;
    /* kill the ctrl-c check        */
    H_INIT();
    BREAKCK(0);     /* BREAK OFF    */
}

/*
 * This function gets called just before we go back home to the command
 * interpreter. On VMS it puts the terminal back in a reasonable state.
 * Another no-operation on CPM.
 */
ttclose()
{
    /* restore the ctrl-c check     */
    BREAKCK(1);     /* BREAK ON     */
}

/*
 * Write a character to the display. On VMS, terminal output is buffered, and
 * we just put the characters in the big array, after checking for overflow.
 * On CPM terminal I/O unbuffered, so we just write the byte out. Ditto on
 * MS-DOS (use the very very raw console output routine).
 */
ttputc(c)
{
    C_PUTC( c & 0xff );
}

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done.
 */
Term_flush()
{
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all. More complex in VMS that almost anyplace else, which figures. Very
 * simple on CPM, because the system can do exactly what you want.
 */
ttgetc()
{
    static int c = 0;     /* character read */

    /* if a char already is ready, return it */
    if (nxtchar >= 0)
    {
        c = nxtchar;
        nxtchar = -1;
        return( c );
    }

    if( (c == 'E' - '@') || (c == 'X' - '@') )
    {
        if(BITSNS(0x0e) & 2)
        {
            /* necessary    CTRL Key    */
            if(BITSNS(0x7) & 0x50)
            {
                /* either Cursor UP or DOWN */
                if( K_KEYSNS()!=0 )
                {
                    K_KEYINP();
                }
                return(c);  /* Rapid Repeat Key     */
            }
        }
    }
    C_CURON();

    /* call the dos to get a char */
    while(K_KEYSNS() == 0 );
    c = K_KEYINP();
    if( (c == 'R' - '@') || (c == 'C' - '@') )
    {
        keydrops();
    }
    return( c & 255 );
}

#if TYPEAH
/* typahead:    Check to see if any characters are already in the keyboard buffer */
typahead()
{
    return(kbhit() != 0);
}
#endif

/*
 * The routines in this file provide support for the X68000
 * It compiles into nothing if there's no X68000 driver
 */
#define NROW 31             /* Screen size.                 */
#define NCOL 96             /* Edit if you want to.         */
#define MARGIN 8            /* size of minimim margin and   */
#define SCRSIZ 64           /* scroll size for extended lines */
#define NPAUSE 80           /* # times thru update to pause */
#define BEL 0x07            /* BEL character.               */
#define ESC 0x1B            /* ESC character.               */
#define SPACE 32            /* space character              */

int Term_move();
int Term_eeol();
int Term_eeop();
int Term_beep();
int Term_open();
int Term_rev();
int Term_close();
int Term_putc();
int Term_fcol();
int Term_bcol();

/*
 * Standard terminal interface dispatch table. Most of the fields point into
 * "termio" code.
 */
TERM term =
{
    NROW - 1,
    NCOL,
    MARGIN,
    SCRSIZ,
    NPAUSE,
    Term_open,
    Term_close,
    ttgetc,
    Term_putc,
    Term_flush,
    Term_move,
    Term_eeol,
    Term_eeop,
    Term_beep,
    Term_rev
};

Term_move(row, col)
{
    B_LOCATE( col, row );
    C_CUROFF();
}

Term_eeol()     /* erase to the end of the line */
{
    B_ERA_ED();
}

Term_putc(ch)   /* put a character at the current position in the current colors */
int ch;
{
    B_PUTC(ch & 0xff);
}

Term_eeop()
{
    B_CLR_ED();
}

Term_rev(state)
int state;      /* TRUE = reverse, FALSE = normal */
{
    if(state)
    {
        B_COLOR(13);
    }
    else
    {
        B_COLOR(3);
    }
}

Term_beep()
{
    C_PUTC(BEL);
}

Term_open()
{
    switch(CRTMODE = CRTMOD(-1))
    {
        case 16:
            term.t_nrow = NROW - 1;
            term.t_ncol = NCOL;
            break;

        case 17:
            term.t_nrow = 24;
            term.t_ncol = (B_CONSOL(-1, -1, -1, -1) >> 16) + 1;
            /* 84 chars / 128 chars */
            break;

        case 18:
            term.t_nrow = 49;
            term.t_ncol = 128;
            break;
    }
    if(CRTMODE < 16)
    {
        if(CRTMODE & 2)
        {
            term.t_nrow = 16 - 1;
            term.t_ncol = 32;
        }
        else
        {
            term.t_nrow = NROW - 1;
            term.t_ncol = 64;
        }
    }
    
    C_CLS_AL();
    f_key0();
    f_key2();
    revexist = TRUE;
    ttopen();
}

Term_close()
{
    f_key1();
    ttclose();
    B_COLOR(3);
}

Term_hello()
{
}

/*
 * fnkey.c -- fnkey
 *
 */

/* this file is originally made by CIVIC (FNKEY.C) */
struct k_tbl
{
    char f01[32],   f02[32],  f03[32], f04[32], f05[32],
         f06[32],   f07[32],  f08[32], f09[32], f10[32],
         f11[32],   f12[32],  f13[32], f14[32], f15[32],
         f16[32],   f17[32],  f18[32], f19[32], f20[32],
         r_up[6], r_down[6],   ins[6],  del[6],
           up[6],   left[6], right[6], down[6],
        h_clr[6],   help[6],   clr[6], undo[6];
};
static char *fndef[32] =
{
    "\xFE" "Frst\x1F"    "0",
    "\xFE" "Last\x1F"    "1",
    "\xFE" "Repl\x1F"    "2",
    "\xFE" "Srch\x1F"    "3",
    "\xFE" "Find\x1F"    "4",
    "\xFE" "Rng.\x1F"    "5",
    "\xFE" "Del.\x1F"    "6",
    "\xFE" "Copy\x1F"    "7",
    "\xFE" "Pste\x1F"    "8",
    "\xFE" "Tag.\x1F"    "9",
    "\xFE" "  F11  \x1F" "A",
    "\xFE" "  F12  \x1F" "B",
    "\xFE" "  F13  \x1F" "C",
    "\xFE" "  F14  \x1F" "D",
    "\xFE" "  F15  \x1F" "E",
    "\xFE" "  F16  \x1F" "F",
    "\xFE" "  F17  \x1F" "G",
    "\xFE" "  F18  \x1F" "H",
    "\xFE" "  F19  \x1F" "I",
    "\xFE" "  F20  \x1F" "J",

    "\x03",     /* ROLL DOWN */
    "\x12",     /* ROLL UP   */
    "\x0F",     /* INS       */
    "\x7F",     /* DEL       */
    "\x05",     /* UP        */
    "\x13",     /* LEFT      */
    "\x04",     /* RIGHT     */
    "\x18",     /* DOWN      */
    "\x1B" "A", /* CLR       */
    "\x1B" "?", /* HELP      */
    "\x1F" "^", /* HOME      */
    "\x1F" "_"  /* UNDO      */
};

static struct k_tbl new, old;
static int fnkeyset = 0, menucmd = 0;

static void f_get(tbl)
struct k_tbl *tbl;
{
    FNCKEYGT(0, (char *) tbl);
}

static void f_set(tbl)
struct k_tbl *tbl;
{
    FNCKEYST(0, (char *) tbl);
}

static void f_def(t)
register struct k_tbl *t;
{
    register int i;
    register char *p;
    register char **fn;
    p = t->f01;
    fn = &fndef[0];

    for(i = 0 ; i < 20; i++)
    {
        strncpy(p, *fn++, 32);
        p += 32;
    }
    for(i = 20; i < 32; i++)
    {
        strncpy(p, *fn++, 6);
        p += 6;
    }
}

void f_key0()
/* save original function key definition and set new one */
{
    if (fnkeyset == 0)
    {
        if (menucmd == 0)
        {
            f_get(&old);
        }
        else
        {
            f_def(&new);
            f_set(&new);
            fnkeyset = 1;
        }
    }
}

void f_key1()
/* restore original function key definition */
{
    if ( fnkeyset == 1)
    {
        f_set(&old);
        fnkeyset = 0;
    }
}

void f_key2()
/* set new key definition when reading menu.cmd file */
{
    menucmd = 1;
    fnkeyset = 1;
    f_def(&new);
    f_set(&new);
}
