/*
 *          MicroEMACS PRO-68K
 *          Wildcard file search and file selection.
 *              wbABN
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#include <doslib.h>
#include <iocslib.h>
#include "estruct.h"
#include "edef.h"

#define NAMELEN 23
#define ffblk FILBUF
#define ff_name name
#define findfirst(s, blk, t) FILES(blk, s, t)
#define findnext(blk) NFILES(blk)
#define ff_fdate date
#define ff_ftime time

#define MAXARGS 512         /* max number of arguments    */
#define MAXLEN 128          /* max length of an argument  */

#define X_TOP 8
#define Y_TOP 2
#define X_MAX 4
#define Y_MAX 16
#define MAXI 64             /* X_MAX * Y_MAX */
#define X_SIZ 16

static int widef = 0;
static int x_max = 4;
static int y_max = 16;
static int maxi = 64;
static int argbufc;
static int argbufb;

static char wildarg[64];
static char pathname[MAXLEN];
extern char turbobuf[];

static struct ffblk dirblk;

typedef struct
{
    char name[23];
    char atr;
    unsigned short time;
    unsigned short date;
    unsigned int filelen;
} SLOT;

SLOT *slot;

static int chkyen(char *s)
{
    for(; *s != '\0';)
    {
        if(iskanji(*s) && iskanji2(*(s + 1)))
        {
            s+= 2;
            continue;
        }
        if(*s == '\\' && *(s + 1) == '\0')
        {
            return(0);
        }
        s++;
    }
    return(-1);
}

static cmp(t, s)
SLOT *t, *s;
{
    return(stricmp( t->name, s->name));
}
/*
 *  Wildcard Entry
 */

char *wildcard(s)
char *s;
{
    int i;
    int primef;
    struct NAMECKBUF ckbuf;

    slot=(SLOT *) turbobuf;

    primef = 1;

    if((s == NULL) || (*s == 0))
    {
        strcpy(wildarg, "*.*");
    }
    else
    {
        strcpy(wildarg, s);
    }

_dir_:
    mkfulpath(pathname, wildarg);
    strcpy(wildarg, pathname);

    if(iswild(wildarg) == 0)
    {
        return(s);   /* not wildcard string */
    }
    wildexp(wildarg);
    if(primef)
    {
        if(argbufc == 0)
        {
            return(s);      /* new file ???? err */
        }
        if(argbufc == 1)
        {
            /* There is one file */
            strcat(pathname, slot[0].name);
            return(pathname);
        }
    }

    qsort(&slot[0], argbufc, sizeof slot[0], cmp);

    sgarbf = TRUE;

    waku();

    refreshen();
    i = wildsel();
    if(i == -1)
    {
        return(NULL);
    }

    if(slot[i + argbufb].atr & 0x10)
    {
        if(slot[i + argbufb].name[0] == '.')
        {
            cd_paren(wildarg);      /* cut \*.* */
            cd_paren(wildarg);      /* cut current dir */
            strcpy(pathname,wildarg);
            strcat(wildarg, "\\*.*");
        }
        else
        {
            if(NAMECK(pathname, &ckbuf) >= 0)
            {
                strcpy(pathname, ckbuf.drive);
            }
            strcat(pathname, slot[i + argbufb].name);
            strcpy(wildarg, pathname);
            strcat(wildarg, "\\*.*");
        }
        primef = 0;
        goto _dir_;
    }
    strcat(pathname, slot[i + argbufb].name);
    return(pathname);
}

/*
 *  Change to parent directory
 */
cd_paren(s)
char *s;
{
    char *p,*y;

    p = s;
    y = NULL;

    if(*s == '\0')
    {
        return;
    }
    if(chkyen(s) == 0)
    {
        s[strlen(s) - 1] = '\0';
    }
    for(; *p != '\0';)
    {
        if(iskanji(*p) && iskanji2(*(p + 1)))
        {
            p += 2;
            continue;
        }
        if(*p == '\\')
        {
            y = p;
        }
        p++;
    }

    if(y != NULL)
    {
        *y = '\0';
    }
}

/*
 *  Checks whether a string contains a wildcard.
 */
iswild(s)
char *s;
{
    while(*s)
    {
        if( (*s == '?') || (*s == '*') )
        {
            return(1);
        }
        s++;
    }
    return(0);
}

/*
 *  Redisplaying the file selector
 */
refreshen()
{
    register int i,c;

    for(i = 0; i < x_max * y_max; i++)
    {
        c = 3;
        if(slot[i + argbufb].atr & 0x10)
        {
            c = 1;
        }
        cprnname(i, c);
    }
    B_COLOR(3);
}

/*
 *  File Select
 */
wildsel()
{
    register int x,y,c,i;

    x = y = 0;
    while(1)
    {
        i = x + y * x_max;
        c = wildgetc(i);
        
        switch(c)
        {
            case 'H' - '@':
            case 'S' - '@':
                if(x > 0)
                {
                    x--;
                    break;
                }
                else
                {
                    x = x_max - 1;
                }

            case 'E' - '@':
                if(y > 0)
                {
                    y--;
                }
                else
                {
                    if(argbufb > 0)
                    {
                        argbufb -= x_max;
                        refreshen();
                    }
                }
                break;

            case 'R' - '@':
                if(argbufb > 0)
                {
                    argbufb -= x_max * 15;
                    if(argbufb < 0)
                    {
                        argbufb = 0;
                    }
                    refreshen();
                }
                break;

            case ' ':
            case 'D' - '@':
                if(x < x_max - 1)
                {
                    x++;
                    break;
                }
                else
                {
                    x=0;
                }

            case 'X' - '@':
                if(y < y_max - 1)
                {
                    y++;
                }
                else
                {
                    if(argbufb < MAXARGS - x_max * y_max)
                    {
                        argbufb += x_max;
                        refreshen();
                    }
                }
                break;

            case 'C' - '@':
                if(argbufb < MAXARGS - x_max * y_max)
                {
                    argbufb += x_max * 15;
                    refreshen();
                }
                break;

            case 'V':
            case 'v':
                widef ^= 1;
                if(widef)
                {
                    x_max = 1;
                    waku();
                    clw();
                }
                else
                {
                    x_max = X_MAX;
                }
                x = i % x_max;
                y = i / x_max;
                if(i >= x_max * y_max)
                {
                    argbufb += i;
                    i = x = y = 0;
                }
                refreshen();
                break;
        }
        if( (c == 0x0d) )
        {
            i = x + y * x_max;
            if(i < argbufc)
            {
                return(i);
            }
        }
        if( (c == 0x1b) )
        {
            return(-1);
        }
    }
}

/*
 *  Key input when selecting a file
 */
wildgetc(i)
int i;
{
    int c;
    int atrc,revc;

    atrc = 3;
    revc = 10;          /* Regular files */
    if( (i + argbufb) < argbufc )
    {
        if( slot[i + argbufb].atr & 0x10)
        {
            atrc = 1;
            revc = 9;
        }    /* Directory */
    }
    cprnname(i, revc);
    C_CUROFF();
    while(K_KEYSNS() == 0);
    c = K_KEYINP();
    cprnname(i, atrc);
    return(c & 255);
}

/*
 *  Display only one file name (with color settings)
 */
cprnname(i,c)
int i;
{
    B_COLOR(c);
    if ((i + argbufb) < argbufc)
    {
        prnname1(i, slot[i + argbufb].name);
    }
    else
    {
        prnname1(i, "");
    }
    B_COLOR(3);
}

/*
 *  Frame display
 */
waku()
{
    char wakusp[96];
    register int i;

    for(i = 0; i <(X_MAX * X_SIZ + 2); i++)
    {
        wakusp[i] = ' ';
    }
    wakusp[X_MAX * X_SIZ + 2] = 0;
    B_LOCATE(X_TOP, Y_TOP);
    B_COLOR(9);
    B_PRINT(wakusp);
    for(i = 0; i < y_max; i++)
    {
        B_LOCATE(X_TOP, Y_TOP + i + 1);
        B_PRINT(" ");
        B_LOCATE(X_TOP + X_MAX * X_SIZ + 1, Y_TOP + i + 1);
        B_PRINT(" ");
    }
    B_LOCATE(X_TOP, Y_TOP + y_max + 1);
    B_COLOR(9);
    B_PRINT(wakusp);

    B_COLOR(9 + 4);     /* Bold */
    B_LOCATE(X_TOP + 4, Y_TOP);
    B_PRINT(pathname);
}

clw()
{
    char wakusp[96];
    register int i;

    for(i = 0; i < (X_MAX * X_SIZ); i++)
    {
        wakusp[i] = ' ';
    }
    wakusp[X_MAX * X_SIZ] = 0;
    B_COLOR(3);

    for(i = 0; i < y_max; i++)
    {
        B_LOCATE(X_TOP + 1, Y_TOP + i + 1);
        B_PRINT(wakusp);
    }
}

/*
 *  Display only one file name
 */
prnname1(n,s)
int n;
char *s;
{
    char sbuf[128];
    int i,x,y;
    int time,date,filelen;
    int yy,mm,dd;
    int hur,min,sec;

    strcpy(sbuf, s);
    for(i = strlen(sbuf); i < X_SIZ; i++)
    {
        sbuf[i] = ' ';
    }
    sbuf[X_SIZ] = 0;

    x = n % x_max;
    y = n / x_max;
    B_LOCATE(X_TOP + x * X_SIZ + 1, Y_TOP + y + 1);
    B_PRINT(sbuf);

    if(widef)
    {
        if(s[0] == 0)
        {
            B_PRINT("                           ");
            return;
        }

        filelen = slot[n + argbufb].filelen;
        if(slot[n + argbufb].atr & 0x10)
        {
            B_PRINT("   <dir> ");
        }
        else
        {
            sprintf(sbuf," %8d", filelen);
            B_PRINT(sbuf);
        }
        time = slot[n + argbufb].time;
        date = slot[n + argbufb].date;
        
        yy = 80 + ((date >> 9) & 0x7f);
        mm = (date >> 5) & 0x0f;
        dd = (date) & 0x1f;
        
        hur = (time >> 11) & 0x1f;
        min = (time >> 5) & 0x3f;
        sec = (time << 1) & 0x3e;
        
        sprintf(sbuf, " %02d-%02d-%02d %02d:%02d:%02d", yy, mm, dd, hur, min, sec);
        B_PRINT(sbuf);
    }
}

/***************************************************************
        File-Related Subroutines
 ***************************************************************/
/* Replace with Kanji-compatible strrchr */

#define strrchr(s,c) _strrchr(s,c)

unsigned char *_strrchr(unsigned char *str, int ch)
{
    unsigned char *ptr = NULL;

    for(; *str != '\0';)
    {
        if( iskanji(*str) && iskanji2(*(str + 1)) )
        {
            if( ((*str) *256 + *(str + 1)) == ch )
            {
                ptr = str;
            }
            str += 2;
            continue;
        }
        else
        {
            if(*str == ch)
            {
                ptr = str;
            }
        }
        str++;
    }

    return(ptr);
}

/*
 *  File Search (1) FindFirst
 */
struct ffblk *ffirst(s, atr)
char *s;
{
    if( findfirst(s, &dirblk, atr)== 0)   /* archive | directory */
    {
        return( &dirblk);
    }
    else
    {
        return(NULL);
    }
}

/*
 *  File Search (2) findnext
 */
struct ffblk *fnext()
{
    if (findnext(&dirblk) == 0)
    {
        return(  &dirblk);
    }
    else
    {
        return(NULL);
    }
}

/*
 *  Wildcard Expansion
 */
wildexp(str)
char *str;
{
     char *ap;
     char **argv;
     char quote, withwild;          /* flags */
     char dirname[64];
     struct ffblk *cp;
     register int i;

     argbufc = argbufb = 0;
     withwild = quote = 0;

      ap = pathname;
      do
      {
           if( !quote && ( *str == '*' || *str == '?' ) )
           {
                withwild = 1;
           }
           if(*str == quote)
           {
                /* end of quoted string */
                quote = 0;
                /* Skip quote  */
                str++;
           }
           else if(!quote && (*str == '\'' || *str == '"'))
           {
                quote = *str++;
           }
           else
           {
                *ap++ = *str++;
           }

      }while( *str && (quote || !isspace(*str)));

      *ap = 0;

      strcpy(dirname,pathname);
      cd_paren(dirname);
      strcat(dirname, "\\*.*");

      if( withwild && ((cp = ffirst(pathname, 0x20)) != 0) )
      {
           if( ((ap = strrchr(pathname, '\\')) != NULL) ||
               ((ap = strrchr(pathname, '/' )) != NULL) ||
               ((ap = strrchr(pathname, ':' )) != NULL)
             )
           {
                ap[1] = 0;
           }
           else
           {
                pathname[0] = 0;
           }
           do
           {
                putarg(cp);
           } while((cp = fnext()) != 0);
      }

      if( withwild && ((cp = ffirst(dirname, 0x10)) != 0) )
      {
           do
           {
                putarg(cp);
           } while((cp = fnext()) != 0);
      }
}

/*
 *  Register the extracted file in the structure.
 */
putarg(dirblk)
struct ffblk *dirblk;
{
    int i;
    unsigned char *p;

    if(argbufc > MAXARGS)
    {
        return;
    }
    p = dirblk->name;
    if((p[0] == '.') && (p[1] == 0))
    {
        return;   /* Current ('.') is not registered */
    }

    for(i = 0; i < 23; i++)
    {
        slot[argbufc].name[i] = p[i];
    }

    slot[argbufc].atr = dirblk->atr;
    slot[argbufc].time = dirblk->time;
    slot[argbufc].date = dirblk->date;
    slot[argbufc].filelen = dirblk->filelen;
    argbufc++;
}
