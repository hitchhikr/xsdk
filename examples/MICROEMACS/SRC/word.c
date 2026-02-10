/*
 * The routines in this file implement commands that work word or a
 * paragraph at a time.  There are all sorts of word mode commands.  If I
 * do any sentence mode commands, they are likely to be put in this file. 
 */

#include <stdio.h>
#include "estruct.h"
#include "edef.h"

/* Word wrap on n-spaces. Back-over whatever precedes the point on the current
 * line and stop on the first word-break or the beginning of the line. If we
 * reach the beginning of the line, jump back to the end of the word and start
 * a new line.  Otherwise, break the line at the word-break, eat it, and jump
 * back to the end of the word.
 * Returns TRUE on success, FALSE on errors.
 */
wrapword(n)
int n;
{
    register int cnt;   /* size of word wrapped to next line */
    register int c;     /* charector temporary */

    /* backup from the <NL> 1 char */
    if (!backchar(0, 1))
    {
        return(FALSE);
    }

    /* back up until we aren't in a word,
       make sure there is a break in the line */
    cnt = 0;
    while (((c = lgetcc()) != ' ') && (c != '\t'))
    {
        cnt++;
        if (!backchar(0, 1))
        {
            return(FALSE);
        }
        /* if we make it to the begining, start a new line */
        if (curwp->w_doto == 0)
        {
            gotoeol(FALSE, 0);
            return(newline(0, 1));
        }
    }

    /* delete the forward white space */
    if (!forwdel(0, 1))
    {
        return(FALSE);
    }

    /* put in a end of line */
    if (!newline(0, 1))
    {
        return(FALSE);
    }

    /* and past the first word */
    while (cnt-- > 0)
    {
        if (forwchar(FALSE, 1) == FALSE)
        {
            return(FALSE);
        }
    }
    return(TRUE);
}

/*
 * Move the cursor backward by "n" words. All of the details of motion are
 * performed by the "backchar" and "forwchar" routines. Error if you try to
 * move beyond the buffers.
 */
backword(f, n)
{
    if (n < 0)
    {
        return (forwword(f, -n));
    }
    if (backchar(FALSE, 1) == FALSE)
    {
        return (FALSE);
    }
    while (n--)
    {
        while (inword() == FALSE)
        {
            if (backchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
        while (inword() != FALSE)
        {
            if (backchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
    }
    return (forwchar(FALSE, 1));
}

/*
 * Move the cursor forward by the specified number of words. All of the motion
 * is done by "forwchar". Error if you try and move beyond the buffer's end.
 */
forwword(f, n)
{
    if (n < 0)
    {
        return (backword(f, -n));
    }
    while (n--)
    {

#if NFWORD
        while (inword() != FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
#endif

        while (inword() == FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
    }
    return(TRUE);
}

/*
 * Move the cursor forward by the specified number of words. As you move,
 * convert any characters to upper case. Error if you try and move beyond the
 * end of the buffer. Bound to "M-U".
 */
upperword(f, n)
{
    register int c;
    register WINDOW *cwp;

    cwp = curwp;
    if (curbp->b_mode & MDVIEW)     /* don't allow this command if  */
    {
        return(rdonly());           /* we are in read only mode */
    }
    if (n < 0)
    {
        return (FALSE);
    }
    while (n--)
    {
        while (inword() == FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
        while (inword() != FALSE)
        {
            c = lgetcc();
            if (c >= 'a' && c <= 'z')
            {
                c -= 'a' - 'A';
                lputc(cwp->w_dotp, cwp->w_doto, c);
                lchange(WFHARD);
            }
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
    }
    return (TRUE);
}

/*
 * Move the cursor forward by the specified number of words. As you move
 * convert characters to lower case. Error if you try and move over the end of
 * the buffer. Bound to "M-L".
 */
lowerword(f, n)
{
    register int c;
    register WINDOW *cwp;

    cwp = curwp;

    if (curbp->b_mode & MDVIEW)     /* don't allow this command if  */
    {
        return(rdonly());           /* we are in read only mode */
    }
    if (n < 0)
    {
        return (FALSE);
    }
    while (n--)
    {
        while (inword() == FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
        while (inword() != FALSE)
        {
            c = lgetcc();
            if (c >= 'A' && c <= 'Z')
            {
                c += 'a' - 'A';
                lputc(cwp->w_dotp, cwp->w_doto, c);
                lchange(WFHARD);
            }
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
    }
    return (TRUE);
}

/*
 * Move the cursor forward by the specified number of words. As you move
 * convert the first character of the word to upper case, and subsequent
 * characters to lower case. Error if you try and move past the end of the
 * buffer. Bound to "M-C".
 */
capword(f, n)
{
    register int c;
    register WINDOW *cwp;

    cwp = curwp;

    if (curbp->b_mode & MDVIEW)     /* don't allow this command if  */
    {
        return(rdonly());           /* we are in read only mode */
    }
    if (n < 0)
    {
        return (FALSE);
    }
    while (n--)
    {
        while (inword() == FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
        }
        if (inword() != FALSE)
        {
            c = lgetcc();
            if (c >= 'a' && c <= 'z')
            {
                c -= 'a' - 'A';
                lputc(cwp->w_dotp, cwp->w_doto, c);
                lchange(WFHARD);
            }
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
            while (inword() != FALSE)
            {
                c = lgetcc();
                if (c >= 'A' && c <= 'Z')
                {
                        c += 'a' - 'A';
                        lputc(cwp->w_dotp, cwp->w_doto, c);
                        lchange(WFHARD);
                }
                if (forwchar(FALSE, 1) == FALSE)
                {
                    return (FALSE);
                }
            }
        }
    }
    return (TRUE);
}

/*
 * Kill forward by "n" words. Remember the location of dot. Move forward by
 * the right number of words. Put dot back where it was and issue the kill
 * command for the right number of characters. Bound to "M-D".
 */
delfword(f, n)
{
    register LINE *dotp;
    register int doto;
    long size;

    if (curbp->b_mode & MDVIEW)     /* don't allow this command if  */
    {
        return(rdonly());           /* we are in read only mode */
    }
    if (n < 0)
    {
        return (FALSE);
    }
    if ((lastflag & CFKILL) == 0)   /* Clear kill buffer if */
    {
        kdelete();                  /* last wasn't a kill.  */
    }
    thisflag |= CFKILL;
    dotp = curwp->w_dotp;
    doto = curwp->w_doto;
    size = 0;
    while (n--)
    {
#if NFWORD
        if (curwp->w_doto == llength(curwp->w_dotp))
        {
            if (forwchar(FALSE,1) == FALSE)
            {
                return(FALSE);
            }
            ++size;
        }

        while (inword() != FALSE)
        {
            if (forwchar(FALSE,1) == FALSE)
            {
                return(FALSE);
            }
            ++size;
        }

        while ((inword() == FALSE) && (curwp->w_doto != llength(curwp->w_dotp)))
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
            ++size;
        }
#else
        while (inword() == FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
            ++size;
        }

        while (inword() != FALSE)
        {
            if (forwchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
            ++size;
        }
#endif
    }
    curwp->w_dotp = dotp;
    curwp->w_doto = doto;

#ifdef KANJI
    return (ldelete(size, TRUE, TRUE));
#else
    return (ldelete(size, TRUE));
#endif
}

/*
 * Kill backwards by "n" words. Move backwards by the desired number of words,
 * counting the characters. When dot is finally moved to its resting place,
 * fire off the kill command. Bound to "M-Rubout" and to "M-Backspace".
 */
delbword(f, n)
{
    long size;

    if (curbp->b_mode & MDVIEW)     /* don't allow this command if  */
    {
        return(rdonly());           /* we are in read only mode */
    }
    if (n < 0)
    {
        return (FALSE);
    }
    if ((lastflag & CFKILL) == 0)   /* Clear kill buffer if */
    {
        kdelete();                  /* last wasn't a kill.  */
    }
    thisflag |= CFKILL;
    if (backchar(FALSE, 1) == FALSE)
    {
        return (FALSE);
    }
    size = 0;
    while (n--)
    {
        while (inword() == FALSE)
        {
            if (backchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
            ++size;
        }
        while (inword() != FALSE)
        {
            if (backchar(FALSE, 1) == FALSE)
            {
                return (FALSE);
            }
            ++size;
        }
    }
    if (forwchar(FALSE, 1) == FALSE)
    {
        return (FALSE);
    }

#ifdef KANJI
    return (ldelete(size, TRUE, TRUE));
#else
    return (ldelete(size, TRUE));
#endif
}

/*
 * Return TRUE if the character at dot is a character that is considered to be
 * part of a word. The word character list is hard coded. Should be setable.
 */
inword()
{
    register int c;
    register WINDOW *cwp;

    cwp = curwp;

    if (cwp->w_doto == llength(cwp->w_dotp))
    {
            return (FALSE);
    }
    c = lgetcc();
    if (c >= 'a' && c <= 'z')
    {
        return (TRUE);
    }
    if (c >= 'A' && c <= 'Z')
    {
        return (TRUE);
    }
    if (c >= '0' && c <= '9')
    {
        return (TRUE);
    }
    if (c == '$' || c == '_')                   /* For identifiers      */
    {
        return (TRUE);
    }

#ifdef KANJI
    if (mskanjip(c))
    {
        return (TRUE);
    }
#endif

    return (FALSE);
}
