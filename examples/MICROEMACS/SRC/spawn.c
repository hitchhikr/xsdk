/*  Spawn:  various DOS access commands
            for MicroEMACS
*/

#include <stdio.h>
#include <string.h>
#include <doslib.h>
#include "estruct.h"
#include "edef.h"
#include <process.h>

/*
 * Create a subjob with a copy of the command intrepreter in it. When the
 * command interpreter exits, mark the screen as garbage so that you do a full
 * repaint. Bound to "^X C". The message at the start in VMS puts out a newline.
 * Under some (unknown) condition, you don't get one free when DCL starts up.
 */
spawncli(f, n)
{
    movecursor(term.t_nrow,0);      /* Seek to last line    */
    Term_close();
    system("command");              /* Run CLI      */
    Term_open();
    sgarbf = TRUE;
    return(TRUE);
}

/*
 * Run a one-liner in a subjob. When the command returns, wait for a single
 * character to be typed, then mark the screen as garbage so a full repaint is
 * done. Bound to "C-X !".
 */
spawn(f, n)
{
    register int s;
    char line[262];
    int c;

    movecursor(term.t_nrow, 0);
    mlputs("Shell: Hit [ESC] to Return EMACS.\r\n");
    Term_close();

    while(1)
    {
        prompt();
        line[0] = 255;
        /* ESC ?? */
        while( (c = K_KEYSNS()) == 0 );
        c &= 0xff;
        if( c == 0x1b )
        {
            keydrops();
            break;
        }
        /* A:path>    */
        GETSS((struct dos_inpptr *) line);
        printf("\n");
        if (line[1] == 0)
        {
            continue;
        }
        if(stricmp(line + 2, "exit") == 0)
        {
            break;
        }
        line[ (line[1] & 0xff) + 2] = 0;
        /* Shell Call */
        system(line + 2);
    }

    Term_open();
    sgarbf = TRUE;
    return (TRUE);
}

prompt()
{
    int  drv;
    char path[80];

    strcpy(path,"A:\\");

    drv=CURDRV();
    CURDIR(drv + 1, path + 3);

    path[0]= drv + 'A';

    C_COLOR(1);
    mlputs(path);
    mlputs(">");
    C_COLOR(3);
}

/*
 * Pipe a one line command into a window
 * Bound to ^X @
 */
pipe(f, n)
{
    register int s;         /* return status from CLI */
    register WINDOW *wp;    /* pointer to new window */
    register BUFFER *bp;    /* pointer to buffer to zot */
    char line[NLINE];       /* command line send to shell */
    static char bname[] = "__FLTOUT.$$$";
    static char filnam[] = "__FLTOUT.$$$";

    /* get the command to pipe in */
    if ((s = mlreply("Pipe$ ", line, NLINE)) != TRUE)
    {
        return(s);
    }

    /* get rid of the command output buffer if it exists */
    if ((bp = bfind(bname, FALSE, 0)) != FALSE)
    {
        /* try to make sure we are off screen */
        wp = wheadp;
        while (wp != NULL)
        {
            if (wp->w_bufp == bp)
            {
                onlywind(FALSE, 1);
                break;
            }
            wp = wp->w_wndp;
        }
        if (zotbuf(bp) != TRUE)
        {
            return(FALSE);
        }
    }

    strcat(line," >");
    strcat(line,filnam);
    movecursor(term.t_nrow , 0);
    Term_close();
    system(line);
    Term_open();
    sgarbf = TRUE;
    s = TRUE;

    if (s != TRUE)
    {
        return(s);
    }
    /* split the current window to make room for the command output */
    if (splitwind(FALSE, 1) == FALSE)
    {
            return(FALSE);
    }
    /* and read the stuff in */
    if (getfile(filnam, FALSE) == FALSE)
    {
        return(FALSE);
    }
    /* make this window in VIEW mode, update all mode lines */
    curwp->w_bufp->b_mode |= MDVIEW;
    wp = wheadp;
    while (wp != NULL)
    {
        wp->w_flag |= WFMODE;
        wp = wp->w_wndp;
    }

    /* and get rid of the temporary file */
    unlink(filnam);
    return(TRUE);
}

/*
 * filter a buffer through an external DOS program
 * Bound to ^X #
 */
filter(f, n)
{
    register int s;         /* return status from CLI */
    register BUFFER *bp;    /* pointer to buffer to zot */
    char line[NLINE];       /* command line send to shell */
    char tmpnam[NFILEN];    /* place to store real file name */

    static char bname1[] = "__FLTINP.$$$";
    static char filnam1[] = "__FLTINP.$$$";
    static char filnam2[] = "__FLTOUT.$$$";

    /* get the filter name and its args */
    if ((s = mlreply("Filter# ", line, NLINE)) != TRUE)
    {
        return(s);
    }
    /* setup the proper file names */
    bp = curbp;
    strcpy(tmpnam, bp->b_fname);    /* save the original name */
    strcpy(bp->b_fname, bname1);    /* set it to our new one */

    /* write it out, checking for errors */
    if (writeout(filnam1) != TRUE)
    {
        mlwrite("[Cannot write filter file]");
        strcpy(bp->b_fname, tmpnam);
        return(FALSE);
    }

    strcat(line," <__FLTINP.$$$ >__FLTOUT.$$$");
    movecursor(term.t_nrow , 0);
    Term_close();
    system(line);
    Term_open();
    sgarbf = TRUE;
    s = TRUE;

    /* on failure, escape gracefully */
    if (s != TRUE || (readin(filnam2, FALSE) == FALSE))
    {
        mlwrite("[Execution failed]");
        strcpy(bp->b_fname, tmpnam);
        unlink(filnam1);
        unlink(filnam2);
        return(s);
    }

    /* reset file name */
    strcpy(bp->b_fname, tmpnam);    /* restore name */
    bp->b_flag |= BFCHG;            /* flag it as changed */

    /* and get rid of the temporary file */
    unlink(filnam1);
    unlink(filnam2);
    return(TRUE);
}
