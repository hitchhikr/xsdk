/*
 *  MicroEMACS 3.7
 *          written by Dave G. Conroy.
 *          substatially modified by Daniel M. Lawrence
 *
 *  (C)opyright 1987 by Daniel M. Lawrence
 *  MicroEMACS 3.7 can be copied and distributed freely for any
 *  non-commercial purposes. MicroEMACS 3.7 can only be incorporated
 *  into commercial software with the permission of the current author.
 *
 *    ...
 *
 *  MicroEMACS PRO-68K for X68000 by iruka, 20-June-88
 *  - bound keys like ED.X
 *  - added linenumber display function
 *  - added smooooth scroll function
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* make global definitions not external */
#define maindef

#include "estruct.h" /* global structures and defines */
#include "efunc.h"   /* function declarations and name table */
#include "edef.h"    /* global definitions */
#include "ebind.h"   /* default key bindings */

#define GOOD 0

unsigned char helpfile[64];
unsigned char bindfile[64];

static char *ename;
extern char *wildcard();
extern tabmask;
extern ctabmask;

#define ENDMIP (META|'@')

/*------------------------------------------------------------------
 * MicroEmacs Start up
 *------------------------------------------------------------------
 */
main(argc, argv)
char *argv[];
{
    register int c;
    register int f;
    register int n;
    register int mflag;
    register BUFFER *bp;
    register int ffile;         /* first file flag          */
    register int carg;          /* current arg to scan      */
    register int startf;        /* startup executed flag    */
    int basec;                  /* c stripped of meta character     */
    int viewflag;               /* are we starting in view mode?    */
    int gotoflag;               /* do we need to goto a line at start?  */
    int gline;                  /* if so, what line?            */
    int searchflag;             /* Do we need to search at start?   */
    unsigned char bname[NBUFN]; /* buffer name of file to read      */
    int  argv0i;
    char hsflag = 0;

    H_INIT();
    /* Extract the startup directory to argv_0[] and rename it to .HLP and .RC */
    strcpy(helpfile, argv[0]);
    strcpy(bindfile, argv[0]);
    for(argv0i = 0; helpfile[argv0i]; argv0i++);
    strcpy(&bindfile[argv0i - 2], ".RC");
    strcpy(&helpfile[argv0i - 2], ".HLP");

    /* initialize the editor and process the command line arguments */
    strcpy(bname, "main");  /* default buffer name              */
    chkscroll();
    vtinit();           /* Displays.                    */
    edinit(bname);      /* Buffers, windows.                */
    viewflag = FALSE;   /* view mode defaults off in command line   */
    gotoflag = FALSE;   /* set to off to begin with         */
    searchflag = FALSE; /* set to off to begin with         */
    ffile = TRUE;       /* no file to edit yet              */
    startf = FALSE;     /* startup file not executed yet */
    
    /* scan through the command line and get the files to edit */
    for (carg = 1; carg < argc; ++carg)
    {
        /* if its a switch, process it */
        if (argv[carg][0] == '-')
        {
            switch (argv[carg][1])
            {
                case 'v':   /* -v for View File */
                case 'V':
                    viewflag = TRUE;
                    break;

                case 'e':   /* -e for Edit file */
                case 'E':
                    viewflag = FALSE;
                    break;

                case 's':   /* -s for initial search string */
                case 'S':
                    searchflag = TRUE;
                    strcpy(pat, &argv[carg][2]);
                    break;

                case 'g':   /* -g for initial goto */
                case 'G':   
                    gotoflag = TRUE;
                    gline = atoi(&argv[carg][2]);
                    break;

                case 'h':   /* -H4 Tab = 4 characters    -H8 8 characters */
                case 'H':
                    tabmask = (argv[carg][2] == '8' ? 7: 3);
                    ctabmask = tabmask;
                    brandnew();
                    hsflag = tabmask;
                    break;

                case 'f':   /* flickey but Fast! */
                case 'F':
                    swflicker();
                    break;

                default:    /* unknown switch */
                    /* ignore this for now */
                    break;
            }
        }
        else  /* check for a macro file */
        {
            if (argv[carg][0]== '@')
            {
                if (startup(&argv[carg][1]) == TRUE)
                    startf = TRUE;  /* don't execute emacs.rc */
            }
            else
            {    /* process a file name */
            /* if we haven't run emacs.rc, do it now */
                if (startf == FALSE)
                {
                    startup(bindfile);
                    startf = TRUE;
                }
                /* set up a buffer for this file */
                makename(bname, argv[carg]);
                /* if this is the first file, read it in */
                if (ffile)
                {
                    bp = curbp;
                    ename = wildcard(argv[carg]);
                    
                    makename(bname, ename);
                    strcpy(bp->b_bname, bname);
                    strcpy(bp->b_fname, ename);
                    if (readin(ename, (viewflag == FALSE)) == ABORT)
                    {
                        strcpy(bp->b_bname, "main");
                        strcpy(bp->b_fname, "");
                    }
                    bp->b_dotp = bp->b_linep;
                    bp->b_doto = 0;
                    ffile = FALSE;
                }
                else
                {
                    /* set this to inactive */
                    bp = bfind(bname, TRUE, 0);
                    strcpy(bp->b_fname, argv[carg]);
                    bp->b_active = FALSE;
                }
                /* set the view mode appropriatly */
                if (viewflag)
                {
                    bp->b_mode |= MDVIEW;
                }
            }
        }
    }
    /* if invoked with nothing, run the startup file here */
    if (startf == FALSE)
    {
        startup(bindfile);
        startf = TRUE;
    }

    if(hsflag)
    {
        tabmask = hsflag;
        ctabmask = hsflag;
        brandnew();
    }

    /* Deal with startup gotos and searches */
    if (gotoflag && searchflag)
    {
        update(FALSE);
        mlwrite("[Can not search and goto at the same time!]");
    }
    else if (gotoflag)
    {
        if (gotoline(NULL, gline) == FALSE)
        {
            update(FALSE);
            mlwrite("[Bogus goto argument]");
        }
    }
    else if (searchflag)
    {
        if (forscan(&pat[0], 2) == FALSE)
        {
            update(FALSE);
            mlwrite("Not found.");
        }
    }
    /* setup to process commands */
    lastflag = 0;                           /* Fake last flags.     */
    curbp->b_mode = curbp->b_mode | gmode;  /* and set default modes*/
    curwp->w_flag |= WFMODE;                /* and force an update  */
loop:
    update(FALSE);                          /* Fix up the screen    */
    c = getcmd();
    if( mpresf != FALSE )
    {
        mlerase();
        update(FALSE);
    }

    f = FALSE;
    n = 1;

    /* do META-# processing if needed */
    basec = c & ~META;      /* strip meta char off if there */
    if ((c & META) && ((basec >= '0' && basec <= '9') || basec == '-'))
    {
        f = TRUE;       /* there is a # arg */
        n = 0;          /* start with a zero default */
        mflag = 1;      /* current minus flag */
        c = basec;      /* strip the META */
        while ((c >= '0' && c <= '9') || (c == '-'))
        {
            if (c == '-')
            {
                /* already hit a minus or digit? */
                if ((mflag == -1) || (n != 0))
                {
                    break;
                }
                mflag = -1;
            }
            else
            {
                n = n * 10 + (c - '0');
            }
            if ((n == 0) && (mflag == -1))  /* lonely - */
            {
                mlwrite("Arg:");
            }
            else
            {
                mlwrite("Arg: %d", n * mflag);
            }
            c = getcmd();                   /* get the next key */

            if( c ==  (CTRL | 'M') )
            {
                c = (META | 'G');           /* Goto Line */
            }
        }
        n = n * mflag;  /* figure in the sign */
    }

    /* do ^U repeat argument processing */

    if (c == (CTRL | 'U'))
    {
        /* ^U, start argument   */
        f = TRUE;
        n = 4;                          /* with argument of 4 */
        mflag = 0;                      /* that can be discarded. */
        mlwrite("Arg: 4");
        while ( ((c = getcmd()) >= '0' && c <= '9') || c == (CTRL | 'U') || c == '-')
        {
            if (c == (CTRL | 'U'))
            {
                n = n * 4;
            }
            /*
             * If dash, and start of argument string, set arg.
             * to -1.  Otherwise, insert it.
             */
            else if (c == '-')
            {
                if (mflag)
                {
                    break;
                }
                n = 0;
                mflag = -1;
            }
            /*
             * If first digit entered, replace previous argument
             * with digit and set sign.  Otherwise, append to arg.
             */
            else
            {
                if (!mflag)
                {
                    n = 0;
                    mflag = 1;
                }
                n = 10 * n + c - '0';
            }
            mlwrite("Arg: %d", (mflag >= 0) ? n : (n ? -n : -1));
        }
        /*
         * Make arguments preceded by a minus sign negative and change
         * the special argument "^U -" to an effective "^U -1".
         */
        if (mflag == -1)
        {
            if (n == 0)
            {
                n++;
            }
            n = -n;
        }
    }

    if (kbdmip != NULL)
    {
        /* Save macro strokes.  */
        if (c != ENDMIP && kbdmip > &kbdm[NKBDM - 6])
        {
            ctrlg(FALSE, 0);
            goto loop;
        }
        if (f != FALSE)
        {
            *kbdmip++ = (CTRL | 'U');
            *kbdmip++ = n;
        }
        *kbdmip++ = c;
    }
    execute(c, f, n);                       /* Do it.               */
    goto loop;
}

/*-------------------------------------------------------------------------
 * edinit -- Initialize all of the buffers and windows.
 * The buffer name is passed down as an argument, because the main routine
 * may have been told to read in a file by default, and we want the buffer
 * name to be right.
 *-------------------------------------------------------------------------
 */
edinit(bname)
unsigned char *bname;
{
    register BUFFER *bp;
    register WINDOW *wp;

    bp = bfind(bname, TRUE, 0);             /* First buffer         */
    blistp = bfind("[List]", TRUE, BFINVS); /* Buffer list buffer   */
    wp = (WINDOW *) malloc(sizeof(WINDOW)); /* First window         */
    if (bp == NULL || wp == NULL || blistp == NULL)
    {
        exit(1);
    }
    curbp  = bp;                            /* Make this current    */
    wheadp = wp;
    curwp  = wp;
    wp->w_wndp  = NULL;                     /* Initialize window    */
    wp->w_bufp  = bp;
    bp->b_nwnd  = 1;                        /* Displayed.           */
    wp->w_linep = bp->b_linep;
    wp->w_dotp  = bp->b_linep;
    wp->w_doto  = 0;
    wp->w_markp = NULL;
    wp->w_marko = 0;
    wp->w_toprow = 0;
    wp->w_linenum = 0;
    wp->w_ntrows = term.t_nrow - 1;         /* "-1" for mode line.  */
    wp->w_force = 0;
    wp->w_flag  = WFMODE | WFHARD;          /* Full.                */
}

#ifdef KANJI
    /*
     * see "edef.h"
     */
    char ipcsts = 0;
#endif
/*
 * This is the general command execution routine. It handles the fake binding
 * of all the keys to "self-insert". It also clears out the "thisflag" word,
 * and arranges to move it to the "lastflag", so that the next command can
 * look at it. Return the status of command.
 */
execute(c, f, n)
{
    register KEYTAB *ktp;
    register int status;

    ktp = &keytab[0];                       /* Look in key table.   */
    while (ktp->k_fp != NULL)
    {
        if (ktp->k_code == c)
        {
            thisflag = 0;
            status   = (*ktp->k_fp)(f, n);
            lastflag = thisflag;
            return (status);
        }
        ++ktp;
    }

    /*
     * If a space was typed, fill column is defined, the argument is non-
     * negative, wrap mode is enabled, and we are now past fill column,
     * and we are not read-only, perform word wrap.
     */
    if (c == ' ' && (curwp->w_bufp->b_mode & MDWRAP) && fillcol > 0 &&
        n >= 0 && getccol(FALSE) > fillcol && (curwp->w_bufp->b_mode & MDVIEW) == FALSE
        )
    {
        wrapword();
    }

    if ((c >= 0x20 && c <= 0x7E)                /* Self inserting.      */
        || (c >= 0xA0 && c <= 0xDF)
#ifdef KANJI
        || (mskanjip(c >> 8))
#endif
       )
    {
        if (n <= 0)
        {                               /* Fenceposts.          */
            lastflag = 0;
            return (n < 0 ? FALSE : TRUE);
        }
        thisflag = 0;                   /* For the future.      */

        /* if we are in overwrite mode, not at eol,
           and next char is not a tab or we are at a tab stop,
           delete a char forword            */
        if (curwp->w_bufp->b_mode & MDOVER &&
            curwp->w_doto < curwp->w_dotp->l_used &&
            (lgetc(curwp->w_dotp, curwp->w_doto) != '\t' ||
             (curwp->w_doto) % (tabmask + 1) == tabmask))
        {
#ifdef KANJI
            ldelete(1L, FALSE, TRUE);
#else
            ldelete(1L, FALSE);
#endif
        }
        /* do the appropriate insertion */
        if (c == '}' && (curbp->b_mode & MDCMOD) != 0)
        {
            status = insbrace(n, c);
        }
        else if (c == '#' && (curbp->b_mode & MDCMOD) != 0)
        {
            status = inspound();
        }
        else
        {
#ifdef KANJI
            if (mskanjip(c >> 8))
            {
                status = linsert(n, c);
                ipcsts++;
            }
            else
#endif
            {
                status = linsert(n, c);
            }
        }
#if CFENCE
        /* check for CMODE fence matching */
        if ((c == '}' || c == ')') && (curbp->b_mode & MDCMOD) != 0)
        {
            fmatch(c);
        }
#endif

        lastflag = thisflag;
        return (status);
    }

    Term_beep();
    mlwrite("[No key assigned]");           /* complain     */
    lastflag = 0;                           /* Fake last flags.     */
    return (FALSE);
}

/*
 * Fancy quit command, as implemented by Norm. If the any buffer has
 * changed do a write on that buffer and exit emacs, otherwise simply exit.
 */
quickexit(f, n)
{
    register BUFFER *bp;    /* scanning pointer to buffers */
    short fl = 1;

    bp = bheadp;
    for(;;)
    {
        if(bp->b_bufp == NULL)
        {
            break;
        }
        fl++;
        bp = bp->b_bufp;
    }

    bp = bheadp;
    while (1)
    {
        /* Changed.             */
        if ((bp->b_flag & BFCHG) != 0 && (bp->b_flag & BFINVS) == 0)
        {  
            /* Real.                */
            curbp = bp;     /* make that buffer cur */
            mlwrite("[Saving %s]",bp->b_fname);
            filesave(f, n);
        }
        if(!--fl)
        {
            break;
        }
        bp = bp->b_bufp;        /* on to the next buffer */
    }
    quit(f, n);                 /* conditionally quit   */
}

/*
 * Quit command. If an argument, always quit. Otherwise confirm if a buffer
 * has been changed and not written out. Normally bound to "C-X C-C".
 */
quit(f, n)
{
    register int s;

    if (f != FALSE                              /* Argument forces it.  */
        || anycb() == FALSE                     /* All buffers clean.   */
                                                /* User says it's OK.   */
        || (s = mlyesno("Are you sure you want to exit?")) == TRUE)
    {
        vttidy();
        exit(GOOD);
    }
    mlwrite("");
    return (s);
}

/*
 * Begin a keyboard macro.
 * Error if not at the top level in keyboard processing. Set up variables and
 * return.
 */
ctlxlp(f, n)
{
    if (kbdmip != NULL || kbdmop != NULL)
    {
        mlwrite("Not now");
        return (FALSE);
    }
    mlwrite("[Start macro]");
    kbdmip = &kbdm[0];
    return (TRUE);
}

/*
 * End keyboard macro. Check for the same limit conditions as the above
 * routine. Set up the variables and return to the caller.
 */
ctlxrp(f, n)
{
    if (kbdmip == NULL)
    {
        mlwrite("Not now");
        return (FALSE);
    }
    mlwrite("[End macro]");
    kbdmip = NULL;
    return (TRUE);
}

ctlxrlp(f, n)
{
    if(kbdmip == NULL)
    {
        return ctlxlp(f, n);
    }
    else
    {
        return ctlxrp(f, n);
    }
}
/*
 * Execute a macro.
 * The command argument is the number of times to loop. Quit as soon as a
 * command gets an error. Return TRUE if all ok, else FALSE.
 */
ctlxe(f, n)
{
    register int c;
    register int af;
    register int an;
    register int  s;

    if (kbdmip != NULL || kbdmop != NULL)
    {
        mlwrite("Not now");
        return (FALSE);
    }
    if (n <= 0)
    {
        return (TRUE);
    }
    do
    {
        kbdmop = &kbdm[0];
        do
        {
            af = FALSE;
            an = 1;
            if ((c = *kbdmop++) == (CTRL | 'U'))
            {
                af = TRUE;
                an = *kbdmop++;
                c  = *kbdmop++;
            }
            s = TRUE;
        } while (c != (ENDMIP) && (s = execute(c, af, an)) == TRUE);
        kbdmop = NULL;
    } while (s == TRUE && --n);
    return (s);
}

/*-------------------------------------------------------------------------
 * ctrlg --  Abort.
 * Beep the beeper. Kill off any keyboard macro, etc., that is in progress.
 * Sometimes called as a routine, to do general aborting of stuff.
 *-------------------------------------------------------------------------
 */
ctrlg(f, n)
{
    Term_beep();
    
    if( kbdmip != NULL )
    {
        kbdm[0] = ENDMIP;
        kbdmip  = NULL;
    }
    mlwrite("[Aborted]");
    return (ABORT);
}

/*---------------------------------------------------------------------
 * rdonly -- tell the user that this command is illegal while we are in
 *  VIEW (read-only) mode
 *---------------------------------------------------------------------
 */
rdonly()
{
    Term_beep();
    mlwrite("[Do not edit in VIEW mode]");
    return(FALSE);
}

/*----------------------------------------------------------------*/
meta()  /* dummy function for binding to meta prefix */
{
}

/*----------------------------------------------------------------*/
cex()   /* dummy function for binding to control-x prefix */
{
}
