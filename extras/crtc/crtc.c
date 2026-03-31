// ===============================================================
// CRTC Operator written by Taka2.
// Translated & improved by hitchhikr.
// ===============================================================
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <class.h>
#include <conio.h>
#include <doslib.h>
#include <iocslib.h>

#define locate(xxx,yyy) fprintf(stderr, "\x1b[%d;%dH", (yyy)+1, (xxx)+1)
#define style(xxx) fprintf(stderr, "\x1b[%dm", (xxx))

#define VERSION "1"
#define REVISION "05"

#define O38 38.86363
#define O69 69.55199
#define N38 38.863
#define N69 69.551
#define N50 50.350
#define SEL '>'
#define NSEL ' '
#define SELECTS 10

#define OSCS 8

int oscselects = 7;

double OSC_old[OSCS] = { O38 / 8.0, O69 / 8.0, O38 / 4.0, O69 / 6.0, O69 / 4.0, O69 / 3.0, O69 / 2.0 };
int    HRL_old[OSCS] = {         0,         1,         0,         0,         1,         0,         0 };
int     HF_old[OSCS] = {         0,         1,         0,         1,         1,         1,         1 };
int     HD_old[OSCS] = {         0,         0,         1,         0,         1,         1,         2 };
char   *mess_old[OSCS]={ " 38/8  ", " 69/2/4", " 38/4  ", " 69/6  ", " 69/2/2", " 69/3  ", " 69/2  " };

double OSC_new[OSCS] = { N38 / 8.0, N69 / 8.0, N38 / 4.0, N69 / 6.0, N69 / 4.0, N69 / 3.0, N50 / 2.0, N69 / 2.0 };
int    HRL_new[OSCS] = {         0,         1,         0,         0,         1,         0,         1,         0 };
int     HF_new[OSCS] = {         0,         1,         0,         1,         1,         1,         1,         1 };
int     HD_new[OSCS] = {         0,         0,         1,         0,         1,         1,         3,         2 };
char   *mess_new[OSCS]={ " 38/8  ", " 69/2/4", " 38/4  ", " 69/6  ", " 69/2/2", " 69/3  ", " 50/3  ", " 69/2  " };

double OSC[OSCS];
int HRL[OSCS], HF[OSCS], HD[OSCS];
char *messdotclock[OSCS];

int  VD[2][3] = {      0,      1,      0,
/*     HF inter*/      1,      2,      0 };
int ypos[SELECTS] = {  2,  3,  4,  5,  6,  7,  9, 10, 11, 12 };
int xpos = 27;
char *messinter[3] = {"Non-interlaced", "Interlaced    ", "Read twice    "};

int inter = 0;      /*0:Non-interlaced, 1:Interlaced, 2:Read twice*/
int dotclock = 5;   /*0-6*/
int hdisp = 512;
int hpulse = 80;
int hback = 96;
int hfront = 48;
int vdisp = 512;
int vpulse = 6;
int vback = 35;
int vfront = 15;

int hsync, vsync;

double dhdisp, dhpulse, dhback, dhfront, dvdisp, dvpulse, dvback, dvfront;
double dotclock8, dhsync, dvsync;

unsigned int reg[8], reg20, hrl;

int o_inter, o_dotclock;
int o_hdisp, o_hpulse, o_hback, o_hfront;
int o_vdisp, o_vpulse, o_vback, o_vfront;
int o_hsync, o_vsync;
unsigned int o_reg[8], o_reg20, o_hrl;

void recalc(void)
{
    double dh, dv;
    int realline;
    int hf;

    switch(inter)
    {
        case 0: 
            realline = vdisp;
            break;
        case 1:
            realline = vdisp / 2;
            break;
        case 2:
            realline = vdisp * 2;
            break;
    }

    dh        = 1.0 / OSC[dotclock];
    dotclock8 = dh * 8.0;
    dhdisp    = (double) hdisp * dh;
    dhpulse   = (double) hpulse * dh;
    dhback    = (double) hback * dh;
    dhfront   = (double) hfront * dh;
    hsync     = hdisp + hpulse + hback + hfront;
    dhsync    = (double) (hsync) * dh;

    dv        = dhsync / 1000.0;
    dvdisp    = (double) realline * dv;
    dvpulse   = (double) vpulse * dv;
    dvback    = (double) vback * dv;
    dvfront   = (double) vfront * dv;

    vsync     = realline + vpulse + vback + vfront;
    dvsync    = (double) vsync * dv;
    if (inter == 1)
    {
        dvsync += dv / 2.0;
    }

    reg[0]    = hsync / 8 - 1;
    reg[1]    = hpulse / 8 - 1;
    reg[2]    = (hpulse + hback) / 8 - 5;
    reg[3]    = (hsync - hfront) / 8 - 5;
    reg[4]    = vsync - 1;
    reg[5]    = vpulse - 1;
    reg[6]    = vpulse + vback - 1;
    reg[7]    = vsync - vfront - 1;
    hrl       = HRL[dotclock];
    hf        = HF[dotclock];
    reg20     = hf * 16 + VD[hf][inter] * 4 + HD[dotclock];
}

void print_status(char *msg)
{
    locate(37, 26);
    style(32);
    style(1);
    fprintf(stderr, "                                         ");
    locate(37, 26);
    fprintf(stderr, msg);
    style(0);
}

void showdemand(void)
{
    if (o_inter != inter)
    {
        locate(xpos + 2, 2);
        fputs(messinter[inter], stderr);
    }
    if (o_dotclock != dotclock)
    {
        locate(xpos + 2, 3);
        fprintf(stderr, "%7.3lfMHz(%s) 8-dot clock: %7.3lfÉ s",
                OSC[dotclock], messdotclock[dotclock], dotclock8);
    }
    if (o_hdisp != hdisp || o_dotclock != dotclock)
    {
        locate(xpos + 2, 4);
        fprintf(stderr, "%3d dot (%7.3lfÉ s)",hdisp,  dhdisp);
    }
    if (o_hpulse != hpulse || o_dotclock != dotclock)
    {
        locate(xpos + 2, 5);
        fprintf(stderr, "%3d dot (%7.3lfÉ s)",hpulse,  dhpulse);
    }
    if (o_hback != hback || o_dotclock != dotclock)
    {
        locate(xpos + 2, 6);
        fprintf(stderr, "%3d dot (%7.3lfÉ s)",hback,  dhback);
    }
    if (o_hfront != hfront || o_dotclock != dotclock)
    {
        locate(xpos + 2, 7);
        fprintf(stderr, "%3d dot (%7.3lfÉ s)",hfront,  dhfront);
    }
    if (o_hsync != hsync || o_dotclock != dotclock)
    {
        locate(xpos + 2, 8);
        style(32);
        style(1);
        fprintf(stderr, "%7.3lfÉ s %7.3lfKHz ", dhsync, 1000.0 / dhsync);
        style(0);
    }
    if (o_vdisp != vdisp || o_hsync != hsync || o_dotclock != dotclock)
    {
        locate(xpos + 2, 9);
        fprintf(stderr, "%3d line (%7.3lfns)\n",vdisp,  dvdisp);
    }
    if (o_vpulse != vpulse || o_hsync != hsync || o_dotclock != dotclock)
    {
        locate(xpos + 2, 10);
        fprintf(stderr, "%3d line (%7.3lfns)\n",vpulse,  dvpulse);
    }
    if (o_vback != vback || o_hsync != hsync || o_dotclock != dotclock)
    {
        locate(xpos + 2, 11);
        fprintf(stderr, "%3d line (%7.3lfns)\n",vback,  dvback);
    }
    if (o_vfront != vfront || o_hsync != hsync || o_dotclock != dotclock)
    {
        locate(xpos + 2, 12);
        fprintf(stderr, "%3d line (%7.3lfns)\n",vfront,  dvfront);
    }
    if (o_vsync != vsync || o_inter != inter || o_hsync != hsync || o_dotclock != dotclock)
    {
        locate(xpos + 2, 13);
        style(32);
        style(1);
        fprintf(stderr, "%7.3lfms %7.3lfHz ", dvsync, 1000.0 / dvsync);
        style(0);
    }
    if (o_reg[0] != reg[0])
    {
        locate(24, 15);
        fprintf(stderr, "%5d (%4x)    ", reg[0], reg[0]);
    }
    if (o_reg[1] != reg[1])
    {
        locate(24, 16);
        fprintf(stderr, "%5d (%4x)    ", reg[1], reg[1]);
    }
    if (o_reg[2] != reg[2])
    {
        locate(24, 17);
        fprintf(stderr, "%5d (%4x)    ", reg[2], reg[2]);
    }
    if (o_reg[3] != reg[3])
    {
        locate(24, 18);
        fprintf(stderr, "%5d (%4x)    ", reg[3], reg[3]);
    }
    if (o_reg[4] != reg[4])
    {
        locate(24, 19);
        fprintf(stderr, "%5d (%4x)    ", reg[4], reg[4]);
    }
    if (o_reg[5] != reg[5])
    {
        locate(24, 20);
        fprintf(stderr, "%5d (%4x)    ", reg[5], reg[5]);
    }
    if (o_reg[6] != reg[6])
    {
        locate(24, 21);
        fprintf(stderr, "%5d (%4x)    ", reg[6], reg[6]);
    }
    if (o_reg[7] != reg[7])
    {
        locate(24, 22);
        fprintf(stderr, "%5d (%4x)    ", reg[7], reg[7]);
    }
    if (o_reg20 != reg20)
    {
        locate(24, 23);
        fprintf(stderr, "%5d (%4x)    ", reg20, reg20);
    }
    if (o_hrl != hrl)
    {
        locate(24, 24);
        fprintf(stderr, "%5d", hrl);
    }
    if((1000.0 / dhsync) >= 32 || (1000.0 / dhsync) < 15)
    {
        print_status("Caution: frequency may be out of range!");
    }
    else
    {
        print_status("");
    }

    o_inter = inter;
    o_dotclock = dotclock;
    o_hdisp = hdisp;
    o_hpulse = hpulse;
    o_hback = hback;
    o_hfront = hfront;
    o_hsync = hsync;
    o_vdisp = vdisp;
    o_vpulse = vpulse;
    o_vback = vback;
    o_vfront = vfront;
    o_vsync = vsync;
    o_reg[0] = reg[0];
    o_reg[1] = reg[1];
    o_reg[2] = reg[2];
    o_reg[3] = reg[3];
    o_reg[4] = reg[4];
    o_reg[5] = reg[5];
    o_reg[6] = reg[6];
    o_reg[7] = reg[7];
    o_reg20 = reg20;
    o_hrl = hrl;

}

void showall(int select, FILE *fp)
{
    if (fp == stderr || fp == stdout)
    {
        fprintf(fp, "\x1b*\n\n");
    }
    fprintf(fp, "    Interlace mode         %c %s\n", (select == 0 ? SEL:NSEL), messinter[inter]);
    fprintf(fp, "    Dot clock              %c %7.3lfMHz(%s) 8-dot clock:%7.3lfÉ s\n", (select == 1 ? SEL:NSEL), OSC[dotclock], messdotclock[dotclock], dotclock8);
    fprintf(fp, "    Horizontal display     %c %3d dot (%7.3lfÉ s)\n", (select == 2 ? SEL:NSEL), hdisp,  dhdisp);
    fprintf(fp, "    Horizontal pulse       %c %3d dot (%7.3lfÉ s)\n", (select == 3 ? SEL:NSEL), hpulse, dhpulse);
    fprintf(fp, "    Horizontal back porch  %c %3d dot (%7.3lfÉ s)\n", (select == 4 ? SEL:NSEL), hback,  dhback);
    fprintf(fp, "    Horizontal front porch %c %3d dot (%7.3lfÉ s)\n", (select == 5 ? SEL:NSEL), hfront, dhfront);
    style(32);
    style(1);
    fprintf(fp, "    Horizontal sync          %7.3lfÉ s %7.3lfKHz\n", dhsync, 1000.0 / dhsync);
    style(0);
    fprintf(fp, "    Vertical display       %c %3d line (%7.3lfns)\n", (select == 6 ? SEL:NSEL), vdisp,  dvdisp);
    fprintf(fp, "    Vertical pulse         %c %3d line (%7.3lfns)\n", (select == 7 ? SEL:NSEL), vpulse, dvpulse);
    fprintf(fp, "    Vertical back porch    %c %3d line (%7.3lfns)\n", (select == 8 ? SEL:NSEL), vback,  dvback);
    fprintf(fp, "    Vertical front porch   %c %3d line (%7.3lfns)\n", (select == 9 ? SEL:NSEL), vfront, dvfront);
    style(32);
    style(1);
    fprintf(fp, "    Vertical sync            %7.3lfms %7.3lfHz\n\n",  dvsync, 1000.0 / dvsync);
    style(0);
    fprintf(fp, "    Reg 0  (0xE80000) = %5d (%4x)    \n", reg[0], reg[0]);
    fprintf(fp, "    Reg 1  (0xE80002) = %5d (%4x)    \n", reg[1], reg[1]);
    fprintf(fp, "    Reg 2  (0xE80004) = %5d (%4x)    \n", reg[2], reg[2]);
    fprintf(fp, "    Reg 3  (0xE80006) = %5d (%4x)    \n", reg[3], reg[3]);
    fprintf(fp, "    Reg 4  (0xE80008) = %5d (%4x)    \n", reg[4], reg[4]);
    fprintf(fp, "    Reg 5  (0xE8000A) = %5d (%4x)    \n", reg[5], reg[5]);
    fprintf(fp, "    Reg 6  (0xE8000C) = %5d (%4x)    \n", reg[6], reg[6]);
    fprintf(fp, "    Reg 7  (0xE8000E) = %5d (%4x)    \n", reg[7], reg[7]);
    fprintf(fp, "    Reg 20 (0xE80028) = %5d (%4x)    \n", reg20, reg20);
    fprintf(fp, "    HRL               = %5d\n\n", hrl);
    style(31);
    fprintf(fp, "    CRTC Operator Ver. " VERSION "." REVISION "\n");
    style(0);

    o_inter = inter;
    o_dotclock = dotclock;
    o_hdisp = hdisp;
    o_hpulse = hpulse;
    o_hback = hback;
    o_hfront = hfront;
    o_hsync = hsync;
    o_vdisp = vdisp;
    o_vpulse = vpulse;
    o_vback = vback;
    o_vfront = vfront;
    o_vsync = vsync;
    o_reg[0] = reg[0];
    o_reg[1] = reg[1];
    o_reg[2] = reg[2];
    o_reg[3] = reg[3];
    o_reg[4] = reg[4];
    o_reg[5] = reg[5];
    o_reg[6] = reg[6];
    o_reg[7] = reg[7];
    o_reg20 = reg20;
    o_hrl = hrl;
    if (fp == stderr ||  fp == stdout)
    {
        int i;
        B_PUTMES(3, 0, 0, 127, (unsigned char *)
                    "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#"
                    "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#");
        for (i = 0; i < 28; ++i)
        {
            char str[8];
            sprintf(str, "%02d", i + 1);
            B_PUTMES(3, 0, i, 1, (unsigned char *)str);
        }
        for (; i < 62; ++i)
        {
            char str[8];
            sprintf(str, "%02d", i + 1);
            B_PUTMES(3, 0, i, 127, (unsigned char *)
                    "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#"
                    "0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz#");
            B_PUTMES(3, 0, i, 1, (unsigned char*) str);
        }
    }
}

void updown(int select, int dir)
{
    switch(select)
    {
        case 0:
            if (inter+dir >= 0 && inter+dir <= 2)
            {
                inter += dir;
            }
            break;
        case 1:
            if (dotclock+dir >= 0 && dotclock+dir < oscselects)
            {
                dotclock += dir;
            }
            break;
        case 2:
            if (hdisp+dir*8 >= 0 && hdisp+dir*8 <= 1536)
            {
                hdisp += dir*8;
            }
            break;
        case 3:
            if (hpulse+dir*8 >= 0 && hpulse+dir*8 <= 1536)
            {
                hpulse += dir*8;
            }
            break;
        case 4:
            if (hback+dir*8 >= 0 && hback+dir*8 <= 1536)
            {
                hback += dir*8;
            }
            break;
        case 5:
            if (hfront+dir*8 >= 0 && hfront+dir*8 <= 1536)
            {
                hfront += dir*8;
            }
            break;
        case 6:
            if (vdisp+dir >= 0 && vdisp+dir <= 1536)
            {
                vdisp += dir;
            }
            break;
        case 7:
            if (vpulse+dir >= 0 && vpulse+dir <= 1536)
            {
                vpulse += dir;
            }
            break;
        case 8:
            if (vback+dir >= 0 && vback+dir <= 1536)
            {
                vback += dir;
            }
            break;
        case 9:
            if (vfront+dir >= 0 && vfront+dir <= 1536)
            {
                vfront += dir;
            }
            break;
    }
}

void setregister()
{
    volatile static unsigned short *crtc = (unsigned short *) 0xe80000;
    volatile static unsigned short *scon = (unsigned short *) 0xeb080a;
    volatile static unsigned char  *sysp = (unsigned char  *) 0xe8e007;

    unsigned int stack;
    unsigned int i;
    unsigned short r20;
    stack = SUPER(0);
    r20 = crtc[20] & 0x13;
    if (reg20 >= r20)
    {
        for (i = 0; i < 8; i++)
        {
            crtc[i] = reg[i];
        }
        crtc[20] = (crtc[20] & 0xff00) | reg20;
    }
    else
    {
        crtc[20] = (crtc[20] & 0xff00) | reg20;
        for (i = 1; i < 8; i++)
        {
            crtc[i] = reg[i];
        }
        crtc[0] = reg[0];
    }
    if (hrl)
    {
        *sysp |= 0x02;
    }
    else
    {
        *sysp &= ~0x02;
    }
    scon[1] = reg[2] + 4;
    for (i = 0; i < 0x200; i++)
        ;
    if ((reg20 & 0x1f) == 0)
        *scon = reg[0];
    else
        *scon = 0xff;
    scon[2] = reg[6];
    scon[3] = reg20 & 0xff;
    SUPER(stack);
}

void getregister()
{
    volatile static unsigned short *crtc = (unsigned short *) 0xe80000;
    volatile static unsigned char  *sysp = (unsigned char  *) 0xe8e007;

    unsigned int stack;
    unsigned int i;
    int r20, hf, hd, vd;

    stack = SUPER(0);

    hpulse = 8 * crtc[1] +  8;
    hback  = 8 * crtc[2] + 40 - hpulse;
    hdisp  = 8 * crtc[3] + 40 - hpulse - hback;
    hfront = 8 * crtc[0] +  8 - hpulse - hback - hfront;

    vpulse = crtc[5] +  1;
    vback  = crtc[6] +  1 - vpulse;
    vdisp  = crtc[7] +  1 - vpulse - vback;
    vfront = crtc[4] +  1 - vpulse - vback - vfront;

    getch();
    r20 = reg[20];
    hrl = (*sysp >> 1) & 1;
    hf = (r20 >> 4) & 1;
    hd = r20 & 3;
    vd = (r20 >> 2) & 3;

    if (hf == 1)
    {
        switch(vd)
        {
            case 0:
                inter = 2;
                break;
            case 1:
                inter = 0;
                break;
            case 2:
                inter = 1;
                break;
        }
    }
    else
    {
        if (vd == 1)
        {
            inter = 1;
        }
        else
        {
            inter = 0;
        }
    }
    for (i = 0; i < oscselects; ++i)
    {
        if (
            (hrl == HRL[i] && hf == HF[i] && hd == HD[i]) ||
            (HF[i] == 0 && hf == 0 && HD[i] == 0 && hd == 2) ||
            (HF[i] == 1 && hf == 1 && HD[i] == 2 && hd == 2)
           )
        {
            dotclock = i;
            break;
        }
    }
    SUPER(stack);
}

void datawrite(void)
{
    int i;
    char str[128];
    char str_asm[128];
    FILE *fp;
    for (i = 0; i < 1000; ++i)
    {
        sprintf(str, "crtc.%03d", i);
        sprintf(str_asm, "%03d.s", i);
        if ((fp = fopen(str, "r")) != NULL)
        {
            fclose(fp);
        }
        else
        {
            break;
        }
    }
    if (i < 1000)
    {
        fp = fopen(str, "w");
        if (fp != NULL)
        {
            fprintf(fp, "%d %d %d %d %d %d %d %d %d %d\n", inter, dotclock, hdisp, hpulse, hback, hfront, vdisp, vpulse, vback, vfront);
            fprintf(fp, "%d %d %d %d %d %d %d %d %d %d\n", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5], reg[6], reg[7], reg20, hrl);
            fclose(fp);
        }
        _dos_delete(str_asm);
        fp = fopen(str_asm, "w");
        if (fp != NULL)
        {
            fprintf(fp, "           xdef    _setup_screen\n");
            fprintf(fp, "_setup_screen:\n");
            fprintf(fp, "           movem.l d0-d2/a0-a5,-(a7)\n");
            fprintf(fp, ";           move.l  (9*4)+4(a7),a0\n");
            fprintf(fp, "           lea     .screen_data(pc),a0\n");
            fprintf(fp, "           move.w  $E82600,-(a7)\n");
            fprintf(fp, "           move.w  $EB0808,-(a7)\n");
            fprintf(fp, "           move.w  #%%1000000000000000,$E82600\n");
            fprintf(fp, "           clr.w   $EB0808\n");
            fprintf(fp, "           lea     $E80000,a1\n");
            fprintf(fp, "           lea     $EB080A,a4\n");
            fprintf(fp, "           lea     $E8E007,a5\n");
            fprintf(fp, "           move.w  (8*2)(a0),d2\n");
            fprintf(fp, "           move.w  (20*2)(a1),d0\n");
            fprintf(fp, "           and.w   #%%0000000010011,d0\n");
            fprintf(fp, "           cmp.w   d2,d0\n");
            fprintf(fp, "           blt.b   .upper\n");
            fprintf(fp, "           move.w  (20*2)(a1),d0\n");
            fprintf(fp, "           and.w   #$ff00,d0\n");
            fprintf(fp, "           or.w    d2,d0\n");
            fprintf(fp, "           move.w  d0,(20*2)(a1)\n");
            fprintf(fp, "           lea     (1*2)(a0),a2\n");
            fprintf(fp, "           lea     (1*2)(a1),a3\n");
            fprintf(fp, "           moveq   #7-1,d0\n");
            fprintf(fp, ".copy_screen_lower_sync_data:\n");
            fprintf(fp, "           move.w  (a2)+,(a3)+\n");
            fprintf(fp, "           dbf     d0,.copy_screen_lower_sync_data\n");
            fprintf(fp, "           move.w  (a0),(a1)\n");
            fprintf(fp, "           bra.b   .done_crtc\n");
            fprintf(fp, ".upper:\n");
            fprintf(fp, "           lea     (a0),a2\n");
            fprintf(fp, "           lea     (a1),a3\n");
            fprintf(fp, "           moveq   #8-1,d0\n");
            fprintf(fp, ".copy_screen_upper_sync_data:\n");
            fprintf(fp, "           move.w  (a2)+,(a3)+\n");
            fprintf(fp, "           dbf     d0,.copy_screen_upper_sync_data\n");
            fprintf(fp, "           move.w  (20*2)(a1),d0\n");
            fprintf(fp, "           and.w   #$ff00,d0\n");
            fprintf(fp, "           or.w    d2,d0\n");
            fprintf(fp, "           move.w  d0,(20*2)(a1)\n");
            fprintf(fp, ".done_crtc:\n");
            fprintf(fp, "           move.b  (a5),d1\n");
            fprintf(fp, "           move.w  (9*2)(a0),d0\n");
            fprintf(fp, "           beq.b   .hi_res\n");
            fprintf(fp, "           or.b    #2,d1\n");
            fprintf(fp, "           bra.b   .done_res\n");
            fprintf(fp, ".hi_res:\n");
            fprintf(fp, "           and.b   #~2,d1\n");
            fprintf(fp, ".done_res:\n");
            fprintf(fp, "           move.b  d1,(a5)\n");
            fprintf(fp, "           move.w  (2*2)(a0),d0\n");
            fprintf(fp, "           addq.w  #4,d0\n");
            fprintf(fp, "           move.w  d1,(1*2)(a4)\n");
            fprintf(fp, "           moveq   #10-1,d0\n");
            fprintf(fp, ".wait_screen:\n");
            fprintf(fp, "           btst    #4,($E88001)\n");
            fprintf(fp, "           beq.b   .wait_screen\n");
            fprintf(fp, ".wait:\n");
            fprintf(fp, "           btst    #4,($E88001)\n");
            fprintf(fp, "           bne.b   .wait\n");
            fprintf(fp, "           dbf     d0,.wait_screen\n");
            fprintf(fp, "           move.w  d2,d0\n");
            fprintf(fp, "           and.w   #%%0000000011111,d0\n");
            fprintf(fp, "           bne.b   .tot_horz\n");
            fprintf(fp, "           move.w  (a0),(a4)\n");
            fprintf(fp, "           bra.b   .done_tot_horz\n");
            fprintf(fp, ".tot_horz:\n");
            fprintf(fp, "           move.w  #$ff,(a4)\n");
            fprintf(fp, ".done_tot_horz:\n");
            fprintf(fp, "           move.w  (6*2)(a0),(2*2)(a4)\n");
            fprintf(fp, "           and.w   #%%11111111,d2\n");
            fprintf(fp, "           move.w  d2,(3*2)(a4)\n");
            fprintf(fp, "           move.w  (a7)+,$EB0808\n");
            fprintf(fp, "           move.w  (a7)+,$E82600\n");
            fprintf(fp, "           movem.l (a7)+,d0-d2/a0-a5\n");
            fprintf(fp, "           rts\n");
            fprintf(fp, ".screen_data:\n");
            fprintf(fp, "           dc.w   %d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", reg[0], reg[1], reg[2], reg[3], reg[4],
                                                                             reg[5], reg[6], reg[7], reg20, hrl);
            fclose(fp);
        }
        print_status("Files saved.");
    }
}

void dataread(char *filename)
{
    FILE *fp;
    char str[128];

    fp = fopen(filename, "r");
    if (fp != NULL && fgets(str, 128, fp) != NULL)
    {
        sscanf(str, "%d %d %d %d %d %d %d %d %d %d",
                    &inter, &dotclock,
                    &hdisp, &hpulse, &hback, &hfront,
                    &vdisp, &vpulse, &vback, &vfront);
        fclose(fp);
    }
}

void mainloop(void)
{
    int select = 0, c, recalcflag = 0, loopflag = 1;
    int crtflag = 0;
    char backup[712];
    FNCKEYGT(0, (UBYTE*) backup);
    FNCKEYST(25, (UBYTE*) "\x05");      /*Up*/
    FNCKEYST(26, (UBYTE*) "\x13");      /*Left*/
    FNCKEYST(27, (UBYTE*) "\x04");      /*Right*/
    FNCKEYST(28, (UBYTE*) "\x18");      /*Down*/
    FNCKEYST(29, (UBYTE*) "\x0c");      /*CLR*/
    FNCKEYST(31, (UBYTE*) "\x08");      /*HOME*/
    FNCKEYST(32, (UBYTE*) "\x09");      /*UNDO*/
    C_CUROFF();
    B_CONSOL(0, 0, 1024 / 8 - 1, 1024 / 16 - 1);
    recalc();
    showall(select,stderr);
    while (loopflag)
    {
        if (recalcflag && !kbhit())
        {
            recalc();
            showdemand();
        }
        recalcflag = 0;
        switch(c = getch())
        {
            case '8' :
            case 'E'-'@' :
                if (select > 0)
                {
                    locate(xpos, ypos[select]);
                    fputc(NSEL, stderr);
                    select--;
                    locate(xpos, ypos[select]);
                    fputc(SEL, stderr);
                }
                else
                {
                    locate(xpos, ypos[select]);
                    fputc(NSEL, stderr);
                    select = SELECTS - 1;
                    locate(xpos, ypos[select]);
                    fputc(SEL, stderr);
                }
                break;
            case '2' :
            case 'X'-'@' :
                if (select < SELECTS - 1)
                {
                    locate(xpos, ypos[select]);
                    fputc(NSEL, stderr);
                    select++;
                    locate(xpos, ypos[select]);
                    fputc(SEL, stderr);
                }
                else
                {
                    locate(xpos, ypos[select]);
                    fputc(NSEL, stderr);
                    select = 0;
                    locate(xpos, ypos[select]);
                    fputc(SEL, stderr);
                }
                break;
            case '6' :
            case 'D'-'@' :
                if (B_SFTSNS() & 0x03)
                {
                    updown(select, 10);
                }
                else
                {
                    updown(select, 1);
                }
                recalcflag = 1;
                break;
            case '4' :
            case 'S'-'@' :
                if (B_SFTSNS() & 0x03)
                {
                    updown(select, -10);
                }
                else
                {
                    updown(select, -1);
                }
                recalcflag = 1;
                break;
            case '\x0d' :
                datawrite();
                break;
            case 'r' :
            case '\x0c' :
                B_CLR_AL();
                break;
            case '\x08' :
                showall(select, stderr);
                break;
            case ' ' :
                crtflag = 1;
                setregister();
                break;
            case '\x09' :
                crtflag = 0;
                CRTMOD(16 + 256);
                break;
            case '\x1b' :
                fprintf(stderr, "\x1b*");
                loopflag = 0;
                break;
        }
    }
    FNCKEYST(0, (UBYTE *) backup);
    C_FNKMOD(3);
    C_CURON();
    if (crtflag)
    {
        B_CONSOL(0, 0, hdisp / 8 - 1, vdisp / 16 - 1);
        C_CLS_AL();
    }
    else
    {
        CRTMOD(16);
    }
}

void usage(void)
{
    fputs("CRTC Operator Ver. " VERSION "." REVISION " Copyright (C) Taka2 1994\n"
          "Improved in 2026 by Franck 'hitchhikr' Charlet.\n\n"
          "Usage: crtc [options] [crtc.xxx]\n\n"
          "Options:        -q        Read from [crtc.xxx], set the CRTC and end the process.\n"
          "                -n        Support newer models such as the CZ-500\n"
          "                          (50MHz has been added, but the frequency is slightly different).\n"
          "                -o <num>  Change the oscillator frequency, which should be 69MHz.\n"
          "\n"
          "How to operate: 'Å™' 'Å´'  Select the value to change.\n"
          "                'Å©' 'Å®'  Increase or decrease the value.\n"
          "                           (holding down Ctrl will increase or decrease it by 10).\n"
          "                '[SPACE]'  Send the values to CRTC.\n"
          "                '[UNDO]'   Switch the screen mode back to 31kHz/768x512.\n"
          "                '[CLR]'    Clear the screen.\n"
          "                '[HOME]'   Redraw the screen.\n"
          "                '[RETURN]' Output the current values to a file (crtc.xxx and xxx.s).\n"
          "                '[ESC]'    End.\n"
          , stderr);
    exit(1);
}

void optioncheck(int argc, char *argv[])
{
    int i;
    int quitflag = 0;
    int readflag = 0;
    int machinetype = 0;
    double d;
    char str[128];

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-' || argv[i][0] == '/')
        {
            switch(tolower(argv[i][1]))
            {
                case 'q':
                    quitflag = 1;
                    break;
                case 'n':
                    oscselects = 8;
                    machinetype = 1;
                    break;
                case 'o':
                    if (argv[i][2])
                    {
                        sscanf(argv[i] + 2, "%lf", &d);
                    }
                    else if (i < argc - 1)
                    {
                        sscanf(argv[++i], "%lf", &d);
                    }
                    else
                    {
                        usage();
                    }
                    if (d < 0.0 || d > 999.0)
                    {
                        usage();
                    }
                    OSC_old[1] = d / 8.0;
                    OSC_old[3] = d / 6.0;
                    OSC_old[4] = d / 4.0;
                    OSC_old[5] = d / 3.0;
                    OSC_old[6] = d / 2.0;
                    sprintf(str, "%03d", d);
                    memcpy(mess_old[1], str, 3);
                    memcpy(mess_old[3], str, 3);
                    memcpy(mess_old[4], str, 3);
                    memcpy(mess_old[5], str, 3);
                    memcpy(mess_old[6], str, 3);
                    OSC_new[1] = d / 8.0;
                    OSC_new[3] = d / 6.0;
                    OSC_new[4] = d / 4.0;
                    OSC_new[5] = d / 3.0;
                    OSC_new[7] = d / 2.0;
                    memcpy(mess_new[1], str, 3);
                    memcpy(mess_new[3], str, 3);
                    memcpy(mess_new[4], str, 3);
                    memcpy(mess_new[5], str, 3);
                    memcpy(mess_new[7], str, 3);
                    break;
                case '?':
                case 'h':
                default:
                    usage();
                    break;
            }
        }
        else
        {
            dataread(argv[i]);
            readflag = 1;
        }
    }

    if (machinetype == 0)
    {
        for (i = 0; i < oscselects; ++i)
        {
            OSC[i] = OSC_old[i];
            HRL[i] = HRL_old[i];
            HF[i] = HF_old[i];
            HD[i] = HD_old[i];
            messdotclock[i] = mess_old[i];
        }
    }
    else
    {
        for (i = 0; i < oscselects; ++i)
        {
            OSC[i] = OSC_new[i];
            HRL[i] = HRL_new[i];
            HF[i] = HF_new[i];
            HD[i] = HD_new[i];
            messdotclock[i] = mess_new[i];
        }
    }
    if (quitflag)
    {
        if (readflag)
        {
            recalc();
            setregister();
            exit(0);
        }
        else
        {
            usage();
        }
    }
}

int main(int argc, char *argv[])
{
    optioncheck(argc, argv);
    mainloop();
    return 0;
}
