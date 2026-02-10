/*----------------------------------------------------------------------------
 * FILEIO.C -- ffropen,ffwopen,ffclose,ffputline,ffgetline
 * The routines in this file read and write ASCII files from the disk. All of
 * the knowledge about files are here. A better message writing scheme should
 * be used.
 *----------------------------------------------------------------------------
 */
#include <stdio.h>
#include "estruct.h"
#include "edef.h"

#define TURBOSIZE 0x4000
char turbobuf[ TURBOSIZE ];
FILE *ffp;                           /* File pointer, all functions. */

/*---------------------------------------------------------
 * ffropen -- Open a file for reading.
 *--------------------------------------------------------- */
ffropen(fn)
register unsigned char *fn;
{
    if ((ffp = fopen(fn, "rb")) == NULL )
    {
        return (FIOFNF);    /* No exist */
    }
    setvbuf(ffp, turbobuf, 0, TURBOSIZE );
    return (FIOSUC);        /* OK       */
}

/*---------------------------------------------------------------------
 * ffwopen -- Open a file for writing.
 * Return TRUE if all is well, and FALSE on error (cannot create).
 *--------------------------------------------------------------------- */
ffwopen(fn)
register unsigned char *fn;
{
    if ((ffp = fopen(fn, "wb")) == NULL)
    {
        mlwrite("Cannot open file for writing");
        return (FIOERR);/* file write error     */
    }
    setvbuf(ffp, turbobuf, 0, TURBOSIZE );
    return (FIOSUC);    /* OK           */
}

/*--------------------------------------------------------------------
 * ffclose -- Close a file. Should look at the status in all systems.
 *-------------------------------------------------------------------- */
ffclose()
{
    putc( 0x1A, ffp );          /* EOF character put */
    if( fclose(ffp) != FALSE )
    {
        mlwrite("Error closing file");
        return(FIOERR);         /* File Close Error     */
    }
    return(FIOSUC);             /* OK           */
}

/*------------------------------------------------------------------------
 * ffputline -- Write a line to the already opened file.
 * The "buf" points to the buffer,and the "nbuf" is its length,
 * less the free newline. Return the status. Check only at the newline.
 *------------------------------------------------------------------------ */
ffputline(buf,nbuf)
register unsigned char buf[];   /* write buffer     */
register int nbuf;              /* buffer length    */
{
    register int i;
    register FILE *gfp;

    gfp = ffp;

    for (i = 0; i < nbuf; ++i)
    {
        putc(*buf++, gfp);
    }
    putc('\r', gfp);
    putc('\n', gfp);

    if( ferror(gfp) )
    {
        /* check write file status */
        mlwrite("Write I/O error");
        return (FIOERR);
    }
    return (FIOSUC);
}

/*-------------------------------------------------------------------------
 * ffgetline -- Read a line from a file, and store the bytes
 * in the supplied buffer. The "nbuf" is the length of the buffer.
 * Complain about long lines and lines  at the end of the file 
 * that don't have a newline present. Check for I/O errors too.
 * Return status.
 *------------------------------------------------------------------------- */
ffgetline(buf, nbuf)
register unsigned char *buf;    /* buffer pointer   */
register int nbuf;              /* buffer length    */
{
    register FILE *gfp;
    register unsigned char *bufend; /* buffer pointer   */
    register int c;

    gfp = ffp;
    bufend = buf + nbuf - 2;

    while( (c = getc(gfp)) != '\n')
    {
        if(c == EOF)
        {
            break;
        }
        if( buf >= bufend )
        {
            /* read character over buffer   */ 
            bufend[0] = c;  /* store last char read */
            bufend[1] = 0;  /* and terminate it */
            mlwrite("There are lines that do not fit on one line (they have been split)");
            return (FIOLNG);
        }
        *buf++ = c;     /* normal store in buffer   */
    }

    if( c == EOF )
    {
        /* EOF read */
        if( ferror(gfp) )
        {
            mlwrite("Loading errors");
            return (FIOERR);
        }
        return (FIOEOF);    /* end of EOF */
    }
    *buf = 0;       /* end of buffer mark */

    if(buf[-1]=='\r')
    {
        buf[-1] = 0;    /* delete CR */
    }
    return (FIOSUC);    /* OK       */
}
