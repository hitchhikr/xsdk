/*  INPUT:  Various input routines for MicroEMACS 3.7
            written by Daniel Lawrence
            5/9/86                      */

#include <stdio.h>
#include <string.h>
#include <iocslib.h>
#include "estruct.h"
#include "edef.h"

#ifdef  KANJI
static short keitbl[4][13] =
{
    { 'Ñü', 'Ñ†', 'Ñ°', 'Ñ¢', 'Ñ£', 'Ñ§', 'Ñ•', 'Ñ¶', 'Ñß', 'Ñ®', 'Ñ©', 'Å^', 'Å_' },
    { 'Ñ™', 'Ñ´', 'Ñ¨', 'Ñ≠', 'ÑÆ', 'ÑØ', 'Ñ∞', 'Ñ±', 'Ñ≤', 'Ñ≥', 'Ñ¥', 'Å^', 'Å_' },
    { 'Ñü', 'Ñ†', 'Ñ°', 'Ñ¢', 'Ñ£', 'Ñ§', 'Ñ∫', 'Ñª', 'Ñº', 'ÑΩ', 'Ñæ', 'Å^', 'Å_' },
    { 'Ñ™', 'Ñ´', 'Ñ¨', 'Ñ≠', 'ÑÆ', 'ÑØ', 'Ñµ', 'Ñ∂', 'Ñ∑', 'Ñ∏', 'Ñπ', 'Å^', 'Å_' },
};

static char keinum[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 00
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 20
    0x00, 0x00, 0x02, 0x0B, 0x00, 0x01, 0x00, 0x0C,
    0x00, 0x06, 0x0A, 0x05, 0x07, 0x0B, 0x09, 0x03,    // 30
    0x08, 0x04, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 40
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 50
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 60
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,    // 70
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

/*
 * Ask a yes or no question in the message line. Return either TRUE, FALSE, or
 * ABORT. The ABORT status is returned if the user bumps out of the question
 * with a ^G. Used any time a confirmation is required.
 */
mlyesno(prompt)
char *prompt;
{
    register char c;    /* input character */
    char buf[NPAT];     /* prompt to user */

    for (;;)
    {
        /* build and prompt the user */
        strcpy(buf, prompt);
        strcat(buf, " [y/n]? ");
        mlwrite(buf);

        /* get the responce */
        c = ttgetc();

        if (c == BELL)      /* Bail out! */
        {
            return(ABORT);
        }
        if (c == 'y' || c == 'Y')
        {
            return(TRUE);
        }
        if (c == 'n' || c == 'N')
        {
            return(FALSE);
        }
    }
}

ttputl(s)
char *s;
{
    while(*s)
    {
        ttputc(*s++);
    }
}

/*
 * Write a prompt into the message line, then read back a response. Keep
 * track of the physical position of the cursor. If we are in a keyboard
 * macro throw the prompt away, and return the remembered response. This
 * lets macros run at full speed. The reply is always terminated by a carriage
 * return. Handle erase, kill, and abort keys.
 */
mlreply(prompt, buf, nbuf)
char *prompt;
char *buf;
{
    return(mlreplyt(prompt, buf, nbuf, '\n'));
}

/*  A more generalized prompt/reply function allowing the caller
    to specify the proper terminator. If the terminator is not
    a return ('\n') it will echo as "<NL>" */
mlreplyt(prompt, buf, nbuf, eolchar)
char *prompt;
char *buf;
char eolchar;
{
    register int cpos;      /* current character position in string */
    register int i;
    register int c;
    register int quotef;    /* are we quoting the next char? */
    register int status;    /* status return value */

    cpos = 0;
    quotef = FALSE;

    if (kbdmop != NULL)
    {
        while ((c = *kbdmop++) != '\0')
        {
            buf[cpos++] = c;
        }
        buf[cpos] = 0;

        if (buf[0] == 0)
        {
            return(FALSE);
        }
        return(TRUE);
    }

    /* check to see if we are executing a command line */
    if (clexec)
    {
        status = nxtarg(buf);
        buf[nbuf - 1] = 0;      /* make sure we null terminate it */
        return(status);
    }

    mlwrite(prompt);

    for (;;)
    {
        /* get a character from the user. if it is a <ret>, change it to a <NL> */
        c = ttgetc();
        if (c == 0x0d)
        {
            c = '\n';
        }
        if (c == eolchar && quotef == FALSE)
        {
            buf[cpos++] = 0;

            if (kbdmip != NULL)
            {
                if (kbdmip + cpos > &kbdm[NKBDM - 3])
                {
                    ctrlg(FALSE, 0);
                    return(ABORT);
                }

                for (i = 0; i < cpos; ++i)
                {
                    *kbdmip++ = buf[i];
                }
            }

            Term_move(term.t_nrow, 0);
            ttcol = 0;

            if (buf[0] == 0)
            {
                return(FALSE);
            }
            return(TRUE);
        }
        else if (c == 0x07 && quotef == FALSE)
        {
            /* Bell, abort */
            ttputl("^G");
            ttcol += 2;
            ctrlg(FALSE, 0);
            return(ABORT);
        }
        else if ((c == 0x7F || c == 0x08) && quotef == FALSE)
        {
            /* rubout/erase */
            if (cpos != 0)
            {
                ttputl("\b \b");
                --ttcol;
#ifdef KANJI
                if(ismsk2(buf, cpos - 1))
                {
                    ttputl("\b \b");
                    --ttcol;
                    cpos -= 2;
                }
                else
                {
#endif                  
                    if (buf[--cpos] < 0x20)
                    {
                        ttputl("\b \b");
                        --ttcol;
                    }

                    if (buf[cpos] == '\n')
                    {
                        ttputl("\b\b  \b\b");
                        --ttcol;
                        --ttcol;
                    }
#ifdef  KANJI
                }
#endif                  
                    /* (*term.t_flush)(); */
            }

        }
        else if (c == 0x15 && quotef == FALSE)
        {
            /* C-U, kill */
            while (cpos != 0)
            {
                ttputl("\b \b");
                --ttcol;

#ifdef  KANJI
                if ((buf[--cpos] & 0xff) < 0x20)
                {
#else                       
                if (buf[--cpos] < 0x20)
                {
#endif                      
                    ttputl("\b \b");
                    --ttcol;
                }
            }

        }
        else if (c == quotec && quotef == FALSE)
        {
            quotef = TRUE;
        }
        else
        {
            quotef = FALSE;
            if (cpos < nbuf - 1)
            {
                buf[cpos++] = c;

                if ((c < ' ') && (c != '\n'))
                {
                    ttputc('^');
                    ++ttcol;
                    c ^= 0x40;
                }

                if (c != '\n')
                {
                    ttputc(c);
                }
                else
                {
                    /* put out <NL> for <ret> */
                    ttputl("<NL>");
                    ttcol += 3;
                }
                ++ttcol;
            }
        }
    }
}

/* get a command name from the command line. Command completion means
   that pressing a <SPACE> will attempt to complete an unfinished command
   name if it is unique. */
int (*getname())()
{
    register int cpos;      /* current column on screen output */
    register int c;
    register char *sp;      /* pointer to string for output */
    register NBIND *ffp;    /* first ptr to entry in name binding table */
    register NBIND *cffp;   /* current ptr to entry in name binding table */
    register NBIND *lffp;   /* last ptr to entry in name binding table */
    char buf[NSTRING];      /* buffer to hold tentative command name */
    int (*fncmatch())();

    /* starting at the begining of the string buffer */
    cpos = 0;

    /* if we are executing a keyboard macro, fill our buffer from there,
       and attempt a straight match */
    if (kbdmop != NULL)
    {
        while ((c = *kbdmop++) != '\0')
        {
            buf[cpos++] = c;
        }
        buf[cpos] = 0;

        /* return the result of a match */
        return(fncmatch(&buf[0]));
    }

    /* if we are executing a command line get the next arg and match it */
    if (clexec)
    {
        if (nxtarg(buf) != TRUE)
        {
            return(FALSE);
        }
        return(fncmatch(&buf[0]));
    }

    /* build a name string from the keyboard */
    while (TRUE)
    {
        c = ttgetc();

        /* if we are at the end, just match it */
        if (c == 0x0d)
        {
            buf[cpos] = 0;

            /* save keyboard macro string if needed */
            if (kbdtext(&buf[0]) == ABORT)
            {
                return( (int (*)()) NULL);
            }
            /* and match it off */
            return(fncmatch(&buf[0]));
        }
        else if (c == 0x07)
        { 
            /* Bell, abort */
            ttputl("^G");
            ttcol += 2;
            ctrlg(FALSE, 0);
            return( (int (*)()) NULL);
        }
        else if (c == 0x7F || c == 0x08)
        {
            /* rubout/erase */
            if (cpos != 0)
            {
                ttputl("\b \b");
                --ttcol;
                --cpos;
            }
        }
        else if (c == 0x15)
        {
            /* C-U, kill */
            while (cpos != 0)
            {
                ttputl("\b \b");
                --cpos;
                --ttcol;
            }
        }
        else if (c == ' ')
        {
            /* attempt a completion */
            buf[cpos] = 0;      /* terminate it for us */
            ffp = &names[0];    /* scan for matches */
            while (ffp->n_func != NULL)
            {
                if (strncmp(buf, ffp->n_name, strlen(buf)) == 0)
                {
                    /* a possible match! More than one? */
                    if ((ffp + 1)->n_func == NULL ||
                       (strncmp(buf, (ffp+1)->n_name, strlen(buf)) != 0))
                    {
                        /* no...we match, print it */
                        sp = ffp->n_name + cpos;
                        while (*sp)
                        {
                            ttputc(*sp++);
                        }
                        return(ffp->n_func);
                    }
                    else
                    {
                        /* try for a partial match against the list */
                        /* first scan down until we no longer match the current input */
                        lffp = (ffp + 1);
                        while ((lffp + 1)->n_func != NULL)
                        {
                            if (strncmp(buf, (lffp + 1)->n_name, strlen(buf)) != 0)
                            {
                                break;
                            }
                            ++lffp;
                        }

                        /* and now, attempt to partial complete the string, char at a time */
                        while (TRUE)
                        {
                            /* add the next char in */
                            buf[cpos] = ffp->n_name[cpos];

                            /* scan through the candidates */
                            cffp = ffp + 1;
                            while (cffp <= lffp)
                            {
                                if (cffp->n_name[cpos] != buf[cpos])
                                {
                                    goto onward;
                                }
                                ++cffp;
                            }

                            /* add the character */
                            ttputc(buf[cpos++]);
                        }
                    }
                }
                ++ffp;
            }

            /* no match.....beep and onward */
            (*term.t_beep)();
onward:;
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
        }
        else
        {
            if (cpos < NSTRING - 1 && c > ' ')
            {
                buf[cpos++] = c;
                ttputc(c);
            }
            ++ttcol;
        }
    }
}

kbdtext(buf)    /* add this text string to the current keyboard macro definition */
char *buf;      /* text to add to keyboard macro */
{
    /* if we are defining a keyboard macro, save it */
    if (kbdmip != NULL)
    {
        if (kbdmip + strlen(buf) > &kbdm[NKBDM - 4])
        {
            ctrlg(FALSE, 0);
            return(ABORT);
        }

        /* copy string in and null terminate it */
        while (*buf)
        {
            *kbdmip++ = *buf++;
        }
        *kbdmip++ = 0;
    }
    return(TRUE);
}

/*  GET1KEY:    Get one keystroke. The only prefixs legal here
                are the SPEC and CTRL prefixes. */
get1key()
{
    register int c;

    /* get a keystroke */
    c = ttgetc();

    if (c == 0x1F)
    {
        /* Apply SPEC prefix for PC9801 */
        c = ttgetc();
        if ( c >=0x00 && c <= 0x1F )
        {
            /* control key? */
                c = CTRL | (c+'@');
        }
        return(SPEC | c);
    }

    if (c >= 0x00 && c <= 0x1F)
    {
        /* C0 control -> C- */
        c = CTRL | (c + '@');
    }
    return (c);
}

/*  GETCMD: Get a command from the keyboard. Process all applicable
            prefix keys */
getcmd()
{
    register int c;     /* fetched keystroke */

    /* get initial character */
    c = get1key();

#ifdef  KANJI
    /* Shift-JIS Code */
    if (mskanjip(c))
    {
        return(((c & 0xff) << 8) | (get1key() & 0xff));
    }
    if(c <= 0x7F && keinum[c])
    {
        switch(BITSNS(0x0A) & 0xE0)
        {
            case 0x20:
                return(keitbl[0][keinum[c] - 1]);

            case 0x40:
                return(keitbl[1][keinum[c] - 1]);

            case 0x80:
                return(keitbl[2][keinum[c] - 1]);
        }
    }

    if(BITSNS(0x0B) & 0x01 && c <= 0x7F && keinum[c])
    {
        return(keitbl[3][keinum[c] - 1]);
    }
#endif

    /* process META prefix */
    if (c == metac)
    {
        mlwrite("[ ESC ]");
        movecurrent();
        c = get1key();
        if (c >= 'a' && c <= 'z')
        {
            /* Force to upper */
            c -= 0x20;
        }
        if (c >= 0x00 && c <= 0x1F)
        {
            /* control key */
            c = CTRL | (c + '@');
        }
        return(META | c);
    }

    /* process CTLX prefix */
    if (c == ctlxc)
    {
        c = get1key();
        if (c >= 'a' && c <= 'z')
        {
            /* Force to upper */
            c -= 0x20;
        }
        if (c >= 0x00 && c <= 0x1F)
        {
            /* control key */
            c = CTRL | (c + '@');
        }
        return(CTLX | c);
    }

    /* otherwise, just return it */
    return(c);
}
