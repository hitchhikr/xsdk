/*
 * The routines in this file move the cursor around on the screen. They
 * compute a new value for the cursor, then adjust ".". The display code
 * always updates the cursor location, so only moves between lines, or
 * functions that adjust the top line in the window and invalidate the
 * framing, are hard.
 */
#include <stdio.h>
#include <stdlib.h>
#include "estruct.h"
#include "edef.h"

extern tabmask;

/*
 * Move the cursor to the
 * beginning of the current line.
 * Trivial.
 */
gotobol(f, n)
{
    curwp->w_doto = 0;
    return (TRUE);
}

/*
 * Move the cursor backwards by "n" characters. If "n" is less than zero call
 * "forwchar" to actually do the move. Otherwise compute the new cursor
 * location. Error if you try and move out of the buffer. Set the flag if the
 * line pointer for dot changes.
 */
backchar(f, n)
register int    n;
{
    register LINE *lp;
    register WINDOW *cwp;

    if (n < 0)
    {
        return (forwchar(f, -n));
    }
    cwp = curwp;
    while (n--)
    {
        if (cwp->w_doto == 0)
        {
            if ((lp=lback(cwp->w_dotp)) == curbp->b_linep)
            {
                return (FALSE);
            }
            cwp->w_dotp = lp;
            cwp->w_doto = llength(lp);
            cwp->w_flag |= WFMOVE;
        }
        else
        {
            cwp->w_doto--;
#ifdef KANJI
            if (chklmsk(cwp->w_dotp,cwp->w_doto))
            {
                cwp->w_doto--;
            }
#endif
        }
    }
    return (TRUE);
}

/*
 * Move the cursor to the end of the current line. Trivial. No errors.
 */
gotoeol(f, n)
{
    curwp->w_doto = llength(curwp->w_dotp);
    return (TRUE);
}

/*
 * Move the cursor forwwards by "n" characters. If "n" is less than zero call
 * "backchar" to actually do the move. Otherwise compute the new cursor
 * location, and move ".". Error if you try and move off the end of the
 * buffer. Set the flag if the line pointer for dot changes.
 */
forwchar(f, n)
register int n;
{
    register WINDOW *cwp;

    if (n < 0)
    {
        return (backchar(f, -n));
    }

    cwp = curwp;
    while (n--)
    {
        if (cwp->w_doto == llength(cwp->w_dotp))
        {
next1:
            if (cwp->w_dotp == curbp->b_linep)
            {
                return (FALSE);
            }
            cwp->w_dotp = lforw(cwp->w_dotp);
            cwp->w_doto = 0;
            cwp->w_flag |= WFMOVE;
        }
        else
        {                /* Ken */
#ifdef KANJI
            register int pchar;

            pchar = lgetcc() ;
            if (mskanjip(pchar))
            {
                cwp->w_doto++;
                if (cwp->w_doto == llength(cwp->w_dotp))
                {
                    goto next1; /* do not bother */
                }
            }               /* Ken */
#endif
            cwp->w_doto++;
        }
    }
    return (TRUE);
}

gotoline(f, n)      /*  move to a particular line.
                        argument (n) must be a positive integer for
                        this to actually do anything     */

{
    register WINDOW *cwp;

    if (n < 1)      /* if a bogus argument...then leave */
    {
        return(FALSE);
    }
    cwp = curwp;
    /* first, we go to the start of the buffer */
    cwp->w_dotp = lforw(curbp->b_linep);
    cwp->w_doto = 0;
    return(forwline(f, n - 1));
}

/*
 * Goto the beginning of the buffer. Massive adjustment of dot. This is
 * considered to be hard motion; it really isn't if the original value of dot
 * is the same as the new value of dot. Normally bound to "M-<".
 */
gotobob(f, n)
{
    register WINDOW *cwp;

    cwp = curwp;
    cwp->w_dotp = lforw(curbp->b_linep);
    cwp->w_doto = 0;
    cwp->w_flag |= WFHARD;
    cwp->w_linenum=0;
    return (TRUE);
}

/*
 * Move to the end of the buffer. Dot is always put at the end of the file
 * (ZJ). The standard screen code does most of the hard parts of update.
 * Bound to "M->".
 */
gotoeob(f, n)
{
    register WINDOW *cwp;

    cwp = curwp;
    cwp->w_dotp = curbp->b_linep;
    cwp->w_doto = 0;
    cwp->w_flag |= WFHARD;
    cwp->w_linenum=0;
    return (TRUE);
}

/*
 * Move forward by full lines. If the number of lines to move is less than
 * zero, call the backward line function to actually do it. The last command
 * controls how the goal column is set. Bound to "C-N". No errors are
 * possible.
 */
forwline(f, n)
{
    register LINE *dlp;
    register WINDOW *cwp;

    if (n < 0)
    {
        return (backline(f, -n));
    }
    cwp = curwp;
    if ((lastflag&CFCPCN) == 0)         /* Reset goal if last   */
    {
        curgoal = getccol(FALSE);       /* not C-P or C-N       */
    }
    thisflag |= CFCPCN;
    dlp = cwp->w_dotp;
    while (n-- && dlp != curbp->b_linep)
    {
        dlp = lforw(dlp);
    }
    cwp->w_dotp = dlp;
    cwp->w_doto = getgoal(dlp);
    cwp->w_flag |= WFMOVE;
    return (TRUE);
}

/*
 * This function is like "forwline", but goes backwards. The scheme is exactly
 * the same. Check for arguments that are less than zero and call your
 * alternate. Figure out the new line and call "movedot" to perform the
 * motion. No errors are possible. Bound to "C-P".
 */
backline(f, n)
{
    register LINE *dlp;
    register WINDOW *cwp;

    if (n < 0)
    {
        return (forwline(f, -n));
    }
    cwp = curwp;
    if ((lastflag & CFCPCN) == 0)       /* Reset goal if the    */
    {
        curgoal = getccol(FALSE);       /* last isn't C-P, C-N  */
    }
    thisflag |= CFCPCN;
    dlp = cwp->w_dotp;
    while (n-- && lback(dlp)!=curbp->b_linep)
    {
        dlp = lback(dlp);
    }
    cwp->w_dotp = dlp;
    cwp->w_doto = getgoal(dlp);
    cwp->w_flag |= WFMOVE;
    return (TRUE);
}

/*
 * This routine, given a pointer to a LINE, and the current cursor goal
 * column, return the best choice for the offset. The offset is returned.
 * Used by "C-N" and "C-P".
 */
getgoal(dlp)
register LINE *dlp;
{
    register int c;
    register int col;
    register int newcol;
    register int dbo;

    col = 0;
    dbo = 0;
    while (dbo != llength(dlp))
    {
        c = lgetc(dlp, dbo);
        newcol = col;
        if (c == '\t')
        {
            newcol |= tabmask;
        }
        else if (c < 0x20 || c == 0x7F)
        {
            ++newcol;
        }
        ++newcol;
#ifdef  KANJI
/* Ken */
        if (mskanjip(c))
        {
            if (newcol + 1 > curgoal)
            {
                break;
            }
            ++newcol;
            ++dbo;
        }
        else
#endif
/* Ken */
        {
            if (newcol > curgoal)
            {
                break;
            }
        }
        col = newcol;
        ++dbo;
    }
    return (dbo);
}

/*
 * Scroll forward by a specified number of lines, or by a full page if no
 * argument. Bound to "C-V". The "2" in the arithmetic on the window size is
 * the overlap; this value is the default overlap value in ITS EMACS. Because
 * this zaps the top line in the display window, we have to do a hard update.
 */
forwpage(f, n)
register int n;
{
    register LINE *lp;
    register WINDOW *cwp;

    cwp = curwp;

    if (f == FALSE)
    {
        n = cwp->w_ntrows - 2;      /* Default scroll.      */
        if (n <= 0)
        {                           /* Forget the overlap   */
            n = 1;                  /* if tiny window.      */
        }
    }
    else if (n < 0)
    {
        return (backpage(f, -n));
    }
#if CVMVAS
    else                                /* Convert from pages   */
    {
        n *= cwp->w_ntrows;             /* to lines.            */
    }
#endif
    lp = cwp->w_linep;
    while (n-- && lp != curbp->b_linep)
    {
        lp = lforw(lp);
        cwp->w_linenum++;
    }
    cwp->w_linep = lp;
    cwp->w_dotp = lp;
    cwp->w_doto = 0;
    cwp->w_flag |= WFHARD;
    return (TRUE);
}

/*
 * This command is like "forwpage", but it goes backwards. The "2", like
 * above, is the overlap between the two windows. The value is from the ITS
 * EMACS manual. Bound to "M-V". We do a hard update for exactly the same
 * reason.
 */
backpage(f, n)
register int n;
{
    register LINE *lp;
    register WINDOW *cwp;

    cwp = curwp;

    if (f == FALSE)
    {
        n = cwp->w_ntrows - 2;          /* Default scroll.      */
        if (n <= 0)                     /* Don't blow up if the */
        {
            n = 1;                      /* window is tiny.      */
        }
    }
    else if (n < 0)
    {
            return (forwpage(f, -n));
    }
#if     CVMVAS
    else                                /* Convert from pages   */
    {
        n *= cwp->w_ntrows;             /* to lines.            */
    }
#endif
    lp = cwp->w_linep;
    if (lback(lp) == curbp->b_linep)
    {
        return(TRUE);                   /* now we are top_of_text */
    }
    
    while (n-- && lback(lp) != curbp->b_linep)
    {
        lp = lback(lp);
        cwp->w_linenum--;
    }
    cwp->w_linep = lp;
    cwp->w_dotp = lp;
    cwp->w_doto = 0;
    cwp->w_flag |= WFHARD;
    return (TRUE);
}

/*
 * Set the mark in the current window to the value of "." in the window. No
 * errors are possible. Bound to "M-.".
 */
setmark(f, n)
{
    register WINDOW *cwp;

    cwp = curwp;
    cwp->w_markp = cwp->w_dotp;
    cwp->w_marko = cwp->w_doto;
    mlwrite("[Mark set]");
    return (TRUE);
}

/*
 * Swap the values of "." and "mark" in the current window. This is pretty
 * easy, bacause all of the hard work gets done by the standard routine
 * that moves the mark about. The only possible error is "no mark". Bound to
 * "C-X C-X".
 */
swapmark(f, n)
{
    register LINE *odotp;
    register WINDOW *cwp;
    register int odoto;

    cwp = curwp;

    if (cwp->w_markp == NULL)
    {
        mlwrite("No mark in this window");
        return (FALSE);
    }
    odotp = cwp->w_dotp;
    odoto = cwp->w_doto;
    cwp->w_dotp = cwp->w_markp;
    cwp->w_doto = cwp->w_marko;
    cwp->w_markp = odotp;
    cwp->w_marko = odoto;
    cwp->w_flag |= WFMOVE;
    return (TRUE);
}

lgetcc()
{
    return(lgetc(curwp->w_dotp, curwp->w_doto));
}
