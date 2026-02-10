#include "estruct.h"

#ifdef  KANJI
#include "edef.h"

/*
 * mskanjip()
 *  Check if this character is 1st byte of Shift-JIS code
 */
mskanjip(c)
register int c;
{
    c &= 0xff;

    return((c > 0x80 && c < 0xa0) || (c > 0xdf && c < 0xfd));
}

/*
 * ismsk1()
 *  Check this dot is higher part of Shift-JIS Code character or not.
 */
ismsk1(s,n)
register char *s;
register short n;
{
    register short i = n;

    while ((i >= 0) && mskanjip(s[i]))
    {
        i--;
    }
    return(((n - i) % 2) && mskanjip(s[n]));
}

/*
 * ismsk2()
 *  Check this dot is lower part of Shift-JIS Code character or not.
 */
ismsk2(s,n)
register char *s;
register short n;
{
    return(ismsk1(s, n - 1));
}

/*
 * chkhmsk()
 *  Check this dot is higher part of Shift-JIS Code character or not.
 */
chkhmsk(p, dot)
register LINE *p;
register short dot;
{
    register short i = dot;

    while ((i >= 0) && mskanjip((int) lgetc(p, i)))
    {
        i--;
    }
    return(((dot - i) % 2) && mskanjip((int) lgetc(p, dot)) && (dot >= 0));
}

/*
 * chklmsk()
 *  Check this dot is lower part of Shift-JIS Code character or not.
 */
chklmsk(p,dot)
register LINE *p;
register short dot;
{
    return(chkhmsk(p,dot - 1));
}

/*
 * called from ldelete().
 */
long bedeln(n)
long n;
{
    register short doto = curwp->w_doto;
    register short used = curwp->w_dotp->l_used;
    register long i;

    /* see "edef.h" and updateline() */
    ipcsts = (mskanjip((int) lgetc(curwp->w_dotp,doto))) ? 1 : 0;

    for (i = n; i > 0 && doto < used; doto++, i--)
    {
        if (mskanjip((int) lgetc(curwp->w_dotp, doto)))
        {
            doto++;
            n++;
        }
    }
    return(n);
}

/*
 * msklen()
 *  number of character (not byte of string)
 *  called from replaces()
 */
msklen(p)
register char *p;
{
    register i;

    for (i = 0; *p != '\0'; *p++, i++)
    {
        if (mskanjip(*p))
        {
            p++;
        }
    }
    return(i);
}

/*
 * startreset()
 *  find reset position on current screen.
 *  called from updateline().
 */
char *startreset(top, target)
register char *top;
char *target;
{
    register char *start = top;

    while (top < target)
    {
        start = top;
        if (mskanjip(*top++))
        {
            top++;
        }
    }
    return (start);
}
#endif
