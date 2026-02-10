/*
 * The functions in this file handle redisplay. There are two halves, the
 * ones that update the virtual display screen, and the ones that make the
 * physical display screen the same as the virtual display screen. These
 * functions use hints that are left in the windows by the commands.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <doslib.h>
#include <iocslib.h>
#include <string.h>
#include "estruct.h"
#include "edef.h"

#define SCROLPRO 1          /* define scroll enhance */

typedef struct VIDEO
{
    int v_flag;             /* Flags */
    int v_linecnt;          /* line number count */
    char v_linestr[8];      /* line number string */
    char v_text[2];         /* Screen data. */
} VIDEO;

#define VFCHG 0x0001        /* Changed flag         */
#define VFEXT 0x0002        /* extended (beyond column 80)  */
#define VFREV 0x0004        /* reverse video status     */
#define VFREQ 0x0008        /* reverse video request    */
#define VFCOL 0x0010        /* color change requested   */

VIDEO **vscreen;            /* Virtual screen. */

static int scrolflg = 0;
int leftmargin = 6;         /* number display maegin */
int tabmask = 0x07;
int ctabmask = 0x07;
int flickey = 1;            /* 0 = flickey mode 1 = non_flicker */
int flickless = 0;          /* copy of flicker      */
char tabdisplay = 0;

/*
 * Erase from the end of the software cursor to the end of the line on which
 * the software cursor is located.
 */
#define vteeol() { vscreen[vtrow]->v_text[vtcol] = 0; }

chkscroll()
{
    int d;

    d = B_CONSOL(-1, -1, -1, -1);

    if((d & 0xFFFF) != 31 - 1)
    {
        C_WINDOW(0, 31);
    }
}

toggletabdisp(f, n)
{
    if(tabdisplay)
    {
        tabdisplay = 0;
    }
    else
    {
        tabdisplay = 1;
    }
    brandnew();

    return(TRUE);
}

/*
 * Initialize the data structures used by the display code. The edge vectors
 * used to access the screens are set up. The operating system's terminal I/O
 * channel is set up. All the other things get initialized at compile time.
 * The original window has "WFCHG" set, so that it will get completely
 * redrawn on the first call to "update".
 */
vtinit()
{
    register int i, vsize;
    register VIDEO *vp;

    Term_open();
    Term_rev(FALSE);
    vscreen = (VIDEO **) malloc(term.t_nrow * sizeof(VIDEO *));

    if (vscreen == NULL)
    {
        exit(1);
    }

    vsize = sizeof(VIDEO) + term.t_ncol + 4;
    for (i = 0; i < term.t_nrow; ++i)
    {
        vp = (VIDEO *) malloc(vsize);
        if (vp == NULL)
        {
            exit(1);
        }
        vp->v_flag = 0;
        vscreen[i] = vp;
    }
}

/*
 * Clean up the virtual terminal system, in anticipation for a return to the
 * operating system. Move down to the last line and clear it out (the next
 * system prompt will be written in the line). Shut down the channel to the
 * terminal.
 */
vttidy()
{
    mlerase();
    movecursor(term.t_nrow, 0);
    Term_close();
}

/*
 * Set the virtual cursor to the specified row and column on the virtual
 * screen. There is no checking for nonsense values; this might be a good
 * idea during the early stages.
 */
vtmove(row, col)
{
    vtrow = row;
    vtcol = col;
}

/*
 * Write a character to the virtual screen. The virtual row and column are
 * updated. If the line is too long put a "$" in the last column. This routine
 * only puts printing characters into the virtual terminal buffers. Only
 * column overflow is checked.
 */
vtputc(c)
int c;
{
    register VIDEO *vp;
    register int tnc;

    vp = vscreen[vtrow];
    tnc= term.t_ncol - leftmargin;

    if (vtcol >= tnc)
    {
        vtcol = (vtcol + tabmask) & ~tabmask;
#ifdef  KANJI
        if (ismsk2(vp->v_text,tnc - 1))
        {
            vp->v_text[tnc - 2] = ' ';
        }
#endif
        vp->v_text[tnc - 1] = '$';
        vp->v_text[tnc ] = 0;
    }
    else if (c == '\t')
    {
        if(tabdisplay == 0)
        {
            do
            {
                vtputc(' ');
            } while ((vtcol&tabmask) != 0);
        }
        else
        {
            vtputc(0xfe);
            while(vtcol & tabmask)
            {
                vtputc(0xff);
            }
        }
    }
    else if (c < 0x20 || c == 0x7F)
    {
        vtputc('^');
        vtputc(c ^ 0x40);
    }
    else
    {
        vp->v_text[vtcol++] = c;
    }
}

vtputs(s, l)
register unsigned char *s;
{
    register char *vpt;
    register int tnc, c;

    vpt = vscreen[vtrow]->v_text;
    tnc= term.t_ncol-leftmargin;

    while(l)
    {
        c = *s++;

        if (vtcol >= tnc)
        {
            vtcol = (vtcol + tabmask) & ~tabmask;
#ifdef  KANJI
            if (ismsk2(vpt,tnc - 1))
            {
                vpt[tnc - 2] = ' ';
            }
#endif
            vpt[tnc - 1] = '$';
            vpt[tnc ] = 0;
        }
        else if (c == '\t')
        {
            if(tabdisplay == 0)
            {
                do
                {
                    if (vtcol >= tnc)
                    {
                        vtcol = (vtcol + tabmask) & ~tabmask;
                        vpt[tnc - 1] = '$';
                        vpt[tnc ] = 0;
                    }
                    else
                    {
                        vpt[vtcol++] = ' ';
                    }
                }  while ((vtcol&tabmask) != 0);
            }
            else
            {
                if (vtcol >= tnc)
                {
                    vtcol = (vtcol + tabmask) & ~tabmask;
                    vpt[tnc - 1] = '$';
                    vpt[tnc ] = 0;
                }
                else
                {
                    vpt[vtcol++] = 0xfe;
                }
                while(vtcol & tabmask)
                {
                    if (vtcol >= tnc)
                    {
                        vtcol = (vtcol + tabmask) & ~tabmask;
                        vpt[tnc - 1] = '$';
                        vpt[tnc ] = 0;
                    }
                    else
                    {
                        vpt[vtcol++] = 0xff;
                    }
                }
            }
        }
        else if (c < 0x20 || c == 0x7F)
        {
            vtputc('^');
            vtputc(c ^ 0x40);
        }
        else
        {
            vpt[vtcol++] = c;
        }
        l--;
    }
}

/*  put a character to the virtual screen in an extended line. If we are
    not yet on left edge, don't print it yet. check for overflow on
    the right margin                        */

vtpute(c)
int c;
{
    register VIDEO *vp;
    register int tnc;

    vp = vscreen[vtrow];
    tnc= term.t_ncol - leftmargin;

    if (vtcol >= tnc)
    {
        vtcol = (vtcol + tabmask) & ~tabmask;
#ifdef  KANJI
        if (ismsk2(vp->v_text,tnc - 1))
        {
            vp->v_text[tnc - 2] = 0;  /* ' '; */
        }
#endif
        vp->v_text[tnc - 1] = '$';
        vp->v_text[tnc ] = 0;
    }
    else if (c == '\t')
    {
        if(tabdisplay == 0)
        {
            do
            {
                vtpute(' ');
            } while (((vtcol + lbound) & tabmask) != 0);
        }
        else
        {
            vtpute(0xfe);
            while((vtcol + lbound) & tabmask)
            {
                vtpute(0xff);
            }
        }
    }
    else if (c < 0x20 || c == 0x7F)
    {
        vtpute('^');
        vtpute(c ^ 0x40);
    }
    else
    {
        if (vtcol >= 0)
        {
            vp->v_text[vtcol] = c;
        }
        ++vtcol;
    }
}

/* upscreen:    user routine to force a screen update
        always finishes complete update     */
upscreen(f, n)
{
    update(TRUE);
    return(TRUE);
}

/*
 * Make sure that the display is right. This is a three part process. First,
 * scan through all of the windows looking for dirty ones. Check the framing,
 * and refresh the screen. Second, make sure that "currow" and "curcol" are
 * correct for the current window. Third, make the virtual and physical
 * screens the same.
 */
update(force)
int force;  /* force update past type ahead? */
{
    register _WINDOW *wp;

    if(scrolflg)
    {
        scrolflg = 0;
        force = 1;
    }

#if TYPEAH
    if (force == FALSE && typahead())
    {
        return(TRUE);
    }
#endif

    /* update any windows that need refreshing */
    wp = wheadp;
    while (wp != NULL)
    {
        if (wp->w_flag)
        {
            /* if the window has changed, service it */
            reframe(wp);        /* check the framing */

            flickless = 1;      /* non flickerd */

            if(scrolflg)
            {
                force = 1;
                flickless = flickey;
            }

            if ((wp->w_flag & ~WFMODE) == WFEDIT)
            {
                updone(wp);     /* update EDITed line */
            }
            else if (wp->w_flag & ~WFMOVE)
            {
                flickless = flickey;
                updall(wp);
            } /* update all lines */

            if (wp->w_flag & WFMODE)
            {
                modeline(wp);   /* update modeline */
            }
            wp->w_flag = 0;
            wp->w_force = 0;
        }
        /* on to the next window */
        wp = wp->w_wndp;
    }

    updpos();   /* recalc the current hardware cursor location */
    upddex();   /* check for lines to de-extend */
    if (sgarbf != FALSE)
    {
        updgar();   /* if screen is garbage, re-plot it */
    }
    updupd(force);  /* update the virtual screen to the physical screen */
                    /* update the cursor and flush the buffers */
    movecursor(currow, curcol - lbound + leftmargin);
    return(TRUE);
}

movecurrent()
{
    movecursor(currow, curcol - lbound + leftmargin);
}

keydrops()
{
    while(kbhit())
    {
        K_KEYINP();
    }
}

keydrop()
{
    if(kbhit())
    {
        K_KEYINP();
    }
}

/* -------------------------------------------------------------------- */
/*  reframe:    check to see if the cursor is on in the window          */
/*              and re-frame it if needed or wanted                     */
/*  Redisplaying the Frame Virtual Screen                               */
/* -------------------------------------------------------------------- */
reframe(wp)
register _WINDOW *wp;
{
    register _LINE *lp;
    register int i;
    /* If there is no request for redisplay, check if it is necessary */
    /* if not a requested reframe, check for a needed one */
    if ((wp->w_flag & WFFORCE) == 0)
    {
        lp = wp->w_linep;   /* Top line of the window */

#if SCROLPRO
        if (( wp->w_dotp == lback(lp)) && 
            ((wp->w_flag & WFEDIT) == 0) &&
            ( wp->w_dotp != (wp->w_bufp->b_linep)) &&
            ( wp->w_dotp != lback(wp->w_bufp->b_linep)) &&
            ((wp->w_flag & WFHARD) == 0)
           )
        {
            /*keydrops();*/
            scroll_down(wp);
            wp->w_linep = wp->w_dotp;
            wp->w_flag |= WFEDIT;
            wp->w_flag &= ~(WFFORCE | WFHARD | WFMOVE);
            scrolflg = 1;
            return(TRUE);
        }
#endif

        for (i = 0; i < wp->w_ntrows; i++)
        {

            if (lp == wp->w_dotp)           /* The current line was in the window */
            {
                return(TRUE);
            }
            if (lp == wp->w_bufp->b_linep)  /* Reached the last line */
            {
                break;
            }
            lp = lforw(lp);                 /* The next line */
        }

#if SCROLPRO
        if (( wp->w_dotp == lp ) &&
            ((wp->w_flag & WFEDIT) == 0) &&
            ((wp->w_flag & WFHARD) == 0)
           )
        {
            /*keydrops();*/
            scroll_up(wp);
            wp->w_linep = lforw(wp->w_linep);
            wp->w_flag |= WFEDIT;
            wp->w_flag &= ~(WFFORCE | WFHARD | WFMOVE);
            scrolflg=1;
            return(TRUE);
        }
#endif
    }

    /* reaching here, we need a window refresh */
    i = wp->w_force;

    /* how far back to reframe? */
    if (i > 0)
    {
        /* only one screen worth of lines max */
        if (--i >= wp->w_ntrows)
        {
            i = wp->w_ntrows - 1;
        }
    }
    else if (i < 0)
    { 
        /* negative update???? */
        i += wp->w_ntrows;
        if (i < 0)
        {
            i = 0;
        }
    }
    else
    {
        i = wp->w_ntrows / 2;
    }
    /* backup to new line at top of window */
    lp = wp->w_dotp;
    while (i != 0 && lback(lp) != wp->w_bufp->b_linep)
    {
        --i;
        lp = lback(lp);
    }

    /* and reset the current line at top of window */
    wp->w_linep = lp;
    wp->w_flag |= WFHARD;
    wp->w_linenum=0;
    wp->w_flag &= ~WFFORCE;
    return(TRUE);
}

#if SCROLPRO
scroll_up(wp)
register _WINDOW *wp;
{
    int y, i;
    y = wp->w_toprow + 1;

    asm("move.w #18,-(%sp)\n"
        "dc.w   0xFF23\n"
        "addq.w #2,%sp\n");
    TXRASCPY((((y + 1) << 8) + y) << 2, (wp->w_ntrows -1) << 2, 0x0003);

    wp->w_linenum++;
    for(i = 0; i < wp->w_ntrows - 1; i++)
    {
        move_vs(vscreen[y], vscreen[y + 1]);
        y++;
    }
}

scroll_down(wp)
register _WINDOW *wp;
{
    register int y, i;

    y = wp->w_toprow + wp->w_ntrows;

    asm("move.w #18,-(%sp)\n"
        "dc.w   0xFF23\n"
        "addq.w #2,%sp\n"
       );
    TXRASCPY(((((y - 1) << 8) + y) << 2) + 0x0303, (wp->w_ntrows - 1) << 2, 0x8003);

    wp->w_linenum--;
    for(i = 0; i < wp->w_ntrows - 1; i++)
    {
        move_vs(vscreen[y], vscreen[y - 1]);
        y--;
    }
}

move_vs(dst, src)
register char *dst, *src;
{
    register int i;

    i = (sizeof(VIDEO) + term.t_ncol);
    LDIRL(src, dst, i);
}
#endif

/*  updone: update the current line to the virtual screen       */

updone(wp)
register _WINDOW *wp;   /* window to update current line in */
{
    register _LINE *lp; /* line to update */
    register int sline; /* physical screen line to update */
    register int i;

    /* search down the line we want */
    lp = wp->w_linep;
    sline = wp->w_toprow + 1;   /* '+1' K.y:Iruka changes */
    i=0;
    while (lp != wp->w_dotp)
    {
        ++sline;
        ++i;
        lp = lforw(lp);
    }

    /* and update the virtual line */
    if (leftmargin)
    {
        if (i != 0)
        {
            vscreen[sline]->v_linecnt = vscreen[sline - 1]->v_linecnt + 1;
        }
        else
        {
            vscreen[sline]->v_linecnt = vscreen[sline + 1]->v_linecnt - 1;
        }
            sprintf(vscreen[sline]->v_linestr, "%5d\x7f", vscreen[sline]->v_linecnt);
    }
    
    vscreen[sline]->v_flag |= VFCHG;
    vscreen[sline]->v_flag &= ~VFREQ;
    vtmove(sline, 0);
    vtputs(lp->l_text, llength(lp));
    vteeol();
}

/*  updall: update all the lines in a window on the virtual screen */
updall(wp)
register _WINDOW *wp;   /* window to update lines in */
{
    register _LINE *lp; /* line to update */
    register int sline; /* physical screen line to update */
    register int i, linenc;

    /* search down the lines, updating them */
    lp = wp->w_linep;
    sline = wp->w_toprow + 1;

    if(leftmargin)
    {
        linenc = getlinenum(wp);
    }
    while (sline < wp->w_toprow + wp->w_ntrows + 1)
    {
        /* and update the virtual line */
        if(leftmargin)
        {
            sprintf(vscreen[sline]->v_linestr, "%5d\x7f", linenc);
            vscreen[sline]->v_linecnt = linenc;
            linenc++;
        }
        
        vscreen[sline]->v_flag |= VFCHG;
        vscreen[sline]->v_flag &= ~VFREQ;
        vtmove(sline, 0);

        if (lp != wp->w_bufp->b_linep)
        {
            /* if we are not at the end */
            vtputs(lp->l_text, llength(lp));
            lp = lforw(lp);
            vteeol();
        }
        else
        {
            /* on to the next one */
            /* vteeol(); */
            vscreen[vtrow]->v_text[vtcol] = 0x01;
        }
        ++sline;
    }

}

/*  updpos: update the position of the hardware cursor and handle extended
        lines. This is the only update for simple moves.    */
updpos()
{
    register _LINE *lp;
    register int c;
    register int i;

    /* find the current row */
    lp = curwp->w_linep;
    currow = curwp->w_toprow + 1;
    while (lp != curwp->w_dotp)
    {
        ++currow;
        lp = lforw(lp);
    }

    /* find the current column */
    curcol = 0;
    i = 0;
    while (i < curwp->w_doto)
    {
        c = lgetc(lp, i++);
        if (c == '\t')
        {
            curcol |= tabmask;
        }
        else if (c < 0x20 || c == 0x7f)
        {
            ++curcol;
        }
        ++curcol;

    }

    /* if extended, flag so and update the virtual line image */
    if (curcol >=  term.t_ncol-leftmargin - 1)
    {
        vscreen[currow]->v_flag |= (VFEXT | VFCHG);
        updext();
    }
    else
    {
        lbound = 0;
    }
}

/*  upddex: de-extend any line that deserves it        */
upddex()
{
    register _WINDOW *wp;
    register _LINE *lp;
    register int i,j;

    wp = wheadp;

    while (wp != NULL)
    {
        lp = wp->w_linep;
        i = wp->w_toprow + 1;

        while (i < wp->w_toprow + wp->w_ntrows + 1)
        {
            if (vscreen[i]->v_flag & VFEXT)
            {
                if ((wp != curwp) || (lp != wp->w_dotp) ||
                   (curcol < term.t_ncol-leftmargin - 1))
                {
                    vtmove(i, 0);
                    vtputs(lp->l_text,llength(lp));
                    vteeol();

                    /* this line no longer is extended */
                    vscreen[i]->v_flag &= ~VFEXT;
                    vscreen[i]->v_flag |= VFCHG;
                }
            }
            lp = lforw(lp);
            ++i;
        }
        /* and onward to the next window */
        wp = wp->w_wndp;
    }
}

/*  updgar: if the screen is garbage, clear the physical screen and
        the virtual screen and force a full update      */
updgar()
{
    register char *txt;
    register int i,j;

    for (i = 0; i < term.t_nrow; ++i)
    {
        vscreen[i]->v_flag |= VFCHG;
#if REVSTA
        vscreen[i]->v_flag &= ~VFREV;
#endif
    }

    movecursor(0, 0);        /* Erase the screen. */
    Term_eeop();
    sgarbf = FALSE;          /* Erase-page clears */
    mpresf = FALSE;          /* the message area. */
}

/*  updupd: update the physical screen from the virtual screen  */
updupd(force)
int force;      /* forced update flag */
{
    register VIDEO *vp1;
    register int i;

    /*keydrops();*/
    for (i = 0; i < term.t_nrow; ++i)
    {
        vp1 = vscreen[i];

        /* for each line that needs to be updated*/
        if ((vp1->v_flag & VFCHG) != 0)
        {
            updateline(i, vp1 );
        }
    }
    return(TRUE);
}

/*  updext: update the extended line which the cursor is currently
        on at a column greater than the terminal width. The line
        will be scrolled right or left to let the user see where the cursor is */
updext()
{
    register int rcursor;   /* real cursor location */
    register _LINE *lp;     /* pointer to current line */
    register int j;         /* index into line */

    /* calculate what column the real cursor will end up in */
    rcursor = ((curcol - (term.t_ncol - leftmargin) ) % term.t_scrsiz) + term.t_margin;
    lbound = curcol - rcursor + 1;

    /* scan through the line outputing characters to the virtual screen */
    /* once we reach the left edge                  */
    lp = curwp->w_dotp;     /* line to output */
#ifdef  KANJI
    if (chkhmsk(lp,lbound))
    {
        lbound--;
    }
#endif
    vtmove(currow, -lbound);            /* start scanning offscreen */
    for (j = 0; j < llength(lp); ++j)   /* until the end-of-line */
    {
        vtpute(lgetc(lp, j));
    }
    /* truncate the virtual line */
    vteeol();

    /* and put a '$' in column 1 */
    vscreen[currow]->v_text[0] = '$';
}

/*
 * Update a single line. This does not know how to use insert or delete
 * character sequences; we are using VT52 functionality. Update the physical
 * row and column variables. It does try an exploit erase to end of line. The
 * RAINBOW version of this routine uses fast video.
 */
updateline(row, vp1)
int row;                    /* row of screen to update */
register struct VIDEO *vp1; /* virtual screen image */
{
    /*  UPDATE_LINE code for all other versions     */
    int rev;        /* reverse video flag */
    int req;        /* reverse video request flag */

    movecurs2(row, 0);

    rev = (vp1->v_flag & VFREV) == VFREV;
    req = (vp1->v_flag & VFREQ) == VFREQ;

    if (req)
    {
        B_COLOR(13);
        B_PRINT(&vp1->v_text[0]);
        if(strlen(&vp1->v_text[0]) < (term.t_ncol - leftmargin) )
        {
            B_ERA_ED();
        }
    }
    else
    {
        if(vp1->v_text[0] == 0x01)
        {
            B_COLOR(1);
            H_ERA();        /*B_ERA_ED();*/
        }
        else
        {
            if (leftmargin)
            {
                H_PRINT(&vp1->v_linestr[0], &vp1->v_text[0], flickless);
            }
            else
            {
                H_PRINT("", &vp1->v_text[0], flickless);
            }
            B_COLOR(3);
        }
    }
    B_COLOR(1);
    vp1->v_flag &= ~VFCHG;      /* flag this line is changed */
    return(TRUE);
}

/*
 * Redisplay the mode line for the window pointed to by the "wp". This is the
 * only routine that has any idea of how the modeline is formatted. You can
 * change the modeline format by hacking at this routine. Called by "update"
 * any time there is a dirty window.
 */
modeline(wp)
register _WINDOW *wp;
{
    register char *cp;
    register int c;
    register int n;         /* cursor position count */
    register BUFFER *bp;
    register i;             /* loop index */
    register lchar;         /* character to draw line in buffer with */
    register firstm;        /* is this the first mode? */
    char tline[NLINE];      /* buffer for part of mode line */

    n = wp->w_toprow;       /* Location. */
    vscreen[n]->v_flag |= VFCHG | VFREQ | VFCOL;/* Redraw next time. */
    vtmove(n, 0);                       /* Seek to right line. */
    if (wp == curwp)                    /* mark the current buffer */
    {
        lchar = '=';
    }
    else
    {
#if REVSTA
        if (revexist)
        {
            lchar = ' ';
        }
        else
#endif
        {
            lchar = '-';
        }
    }

    vtputc(' ');
    bp = wp->w_bufp;

    if ((bp->b_flag&BFCHG) != 0)                /* "*" if changed. */
    {
        vtputc('*');
    }
    else
    {
        vtputc(' ');
    }
    n  = 2;
    strcpy(tline, " MicroEMACS PRO 68K ("); /* Buffer name. */

    /* display the modes */

    firstm = TRUE;
    for (i = 0; i < NUMMODES; i++)  /* add in the mode flags */
    {
        if (wp->w_bufp->b_mode & (1 << i))
        {
            if (firstm != TRUE)
            {
                strcat(tline, " ");
            }
            firstm = FALSE;
            strcat(tline, modename[i]);
        }
    }
    strcat(tline,") ");

    cp = &tline[0];
    while ((c = *cp++) != 0)
    {
        vtputc(c);
        ++n;
    }

    vtputc(lchar);
    vtputc(lchar);
    vtputc(' ');
    n += 3;
    cp = &bp->b_bname[0];

    while ((c = *cp++) != 0)
    {
#ifdef KANJI
        vtputc(c & 0xff);
#else
        vtputc(c);
#endif
        ++n;
    }

    vtputc(' ');
    vtputc(lchar);
    vtputc(lchar);
    n += 3;

    if (bp->b_fname[0] != 0)            /* File name. */
    {
        vtputc(' ');
        ++n;
        cp = &bp->b_fname[0];

        while ((c = *cp++) != 0)
        {
#ifdef  KANJI
            vtputc(c & 0xff);
#else
            vtputc(c);
#endif
            ++n;
        }

        vtputc(' ');
        ++n;
    }

    while (n < term.t_ncol)             /* Pad to full width. */
    {
        vscreen[vtrow]->v_text[vtcol++] = lchar;
        ++n;
    }
    vscreen[vtrow]->v_text[vtcol] = 0;
}

upmode()    /* update all the mode lines */
{
    register _WINDOW *wp;

    wp = wheadp;
    while (wp != NULL)
    {
        wp->w_flag |= WFMODE;
        wp = wp->w_wndp;
    }
}

/*
 * Send a command to the terminal to move the hardware cursor to row "row"
 * and column "col". The row and column arguments are origin 0. Optimize out
 * random calls. Update "ttrow" and "ttcol".
 */
movecursor(row, col)
{
    ttrow = row;
    ttcol = col;
    B_LOCATE(col, row);
}

movecurs2(row, col)
{
    ttrow = row;
    ttcol = col;
    B_LOCATE(col, row);
}

/*
 * Erase the message line. This is a special routine because the message line
 * is not considered to be part of the virtual screen. It always works
 * immediately; the terminal buffer is flushed via a call to the flusher.
 */
mlerase()
{
    int i;

    movecurs2(term.t_nrow, 0);

    B_ERA_ED();
    mpresf = FALSE;
}

/*
 * Write a message into the message line. Keep track of the physical cursor
 * position. A small class of printf like format items is handled. Assumes the
 * stack grows down; this assumption is made by the "++" in the argument scan
 * loop. Set the "message line" flag TRUE.
 */
mlwrite(fmt, arg)
char *fmt;
{
    register int c;
    register char *ap;

    movecursor(term.t_nrow, 0);
    ap = (char *) &arg;
    while ((c = *fmt++) != 0)
    {
        if (c != '%')
        {
            Term_putc(c);
            ++ttcol;
        }
        else
        {
            c = *fmt++;
            switch (c)
            {
                case 'd':
                    mlputi(*(int *) ap, 10);
                    ap += sizeof(int);
                    break;

                case 'o':
                    mlputi(*(int *) ap,  8);
                    ap += sizeof(int);
                    break;

                case 'x':
                    mlputi(*(int *) ap, 16);
                    ap += sizeof(int);
                    break;

                case 'D':
                    mlputli(*(long *) ap, 10);
                    ap += sizeof(long);
                    break;

                case 's':
                    mlputs(*(char **) ap);
                    ap += sizeof(char *);
                    break;

                case 'f':
                    mlputf(*(int *)ap);
                    ap += sizeof(int);
                    break;

                default:
                    Term_putc(c);
                    ++ttcol;
            }
        }
    }
    B_ERA_ED();
    
    mpresf = TRUE;
}

/*
 * Write out a string. Update the physical cursor position. This assumes that
 * the characters in the string all have width "1"; if this is not the case
 * things will get screwed up a little.
 */
mlputs(s)
register char *s;
{
    register int c;

    while ((c = *s++) != 0)
    {
        Term_putc(c);
        ++ttcol;
    }
}

/*
 * Write out an integer, in the specified radix. Update the physical cursor
 * position.
 */
mlputi(i, r)
{
    register int q;
    static char hexdigits[] = "0123456789ABCDEF";

    if (i < 0)
    {
        i = -i;
        Term_putc('-');
    }

    q = i / r;

    if (q != 0)
    {
        mlputi(q, r);
    }
    Term_putc(hexdigits[i % r]);
    ++ttcol;
}

/*
 * do the same except as a long integer.
 */
mlputli(l, r)
long l;
{
    register long q;

    if (l < 0)
    {
        l = -l;
        Term_putc('-');
    }

    q = l / r;

    if (q != 0)
    {
        mlputli(q, r);
    }
    Term_putc((int)(l % r) + '0');
    ++ttcol;
}

/*
 *  write out a scaled integer with two decimal places
 */
mlputf(s)
int s;  /* scaled integer to output */
{
    int i;  /* integer portion of number */
    int f;  /* fractional portion of number */

    /* break it up */
    i = s / 100;
    f = s % 100;

    /* send out the integer portion */
    mlputi(i, 10);
    Term_putc('.');
    Term_putc((f / 10) + '0');
    Term_putc((f % 10) + '0');
    ttcol += 3;
}

getlinenum(wp)
register _WINDOW *wp;
{
    register _LINE *lp;
    register _LINE *cp;
    register int linenum;

    if(wp->w_linenum)
    {
        return(wp->w_linenum);
    }

    lp = wp->w_bufp->b_linep;
    cp = wp->w_linep;
    linenum = 0;
    while(lp != cp)
    {
        linenum++;
        lp = lforw(lp);
    }
    wp->w_linenum = linenum;
    return(linenum);
}

cmodetab(f, n)
{
    ctabmask ^= 0x04;

    if(curbp->b_mode & MDCMOD)
    {
        tabmask = ctabmask;
        brandnew();
    }
    return(TRUE);
}

switchtab()
{
    tabmask ^= 0x04;
    brandnew();
    return(TRUE);
}

swflicker()
{
    flickey ^= 1;
    return(TRUE);
}

swlinenum()
{
    if(leftmargin)
    {
        leftmargin = 0;
    }
    else
    {
        leftmargin = 6;
    }
    brandnew();
    return(TRUE);
}

brandnew()  /* update all the mode lines */
{
    register _WINDOW *wp;

    wp = wheadp;
    while (wp != NULL)
    {
        wp->w_flag |= WFFORCE;
        wp = wp->w_wndp;
    }
}
