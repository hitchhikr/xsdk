#include <stdio.h>
#include <string.h>
#include <doslib.h>
#include "estruct.h"

#define TRUE 1
#define FALSE 0

void system(char *cmd)
{
    struct COMLINE comline;
    static char path[256] = "command";
    static char first_exec = TRUE;

    if(first_exec == TRUE)
    {
        strcat(path, " ");
        strcat(path, cmd);
        if(PATHCHK(path, &comline, NULL) >= 0)
        {
            first_exec = FALSE;
        }
    }
    else
    {
        strcpy(comline.buffer, cmd);
        comline.len = strlen(cmd);
    }

    if(LOADEXEC(path, &comline, NULL) < 0)
    {
        first_exec = TRUE;
    }
}
