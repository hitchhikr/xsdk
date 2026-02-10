/*  EFUNC.H:    MicroEMACS function declarations and names

        This file list all the C code functions used by MicroEMACS
        and the names to use to bind keys to them. To add functions,
        declare it here in both the extern function list and the name
        binding table.

*/

/* External function declarations */

extern  int ctrlg();                /* Abort out of things          */
extern  int quit();                 /* Quit                         */
extern  int ctlxrlp();              /* Begin macro                  */
extern  int ctlxlp();               /* Begin macro                  */
extern  int ctlxrp();               /* End macro                    */
extern  int ctlxe();                /* Execute macro                */
extern  int cmodetab();             /* Set CMODE tab width 4/8      */
extern  int fileread();             /* Get a file, read only        */
extern  int filefind();             /* Get a file, read write       */
extern  int filewrite();            /* Write a file                 */
extern  int filesave();             /* Save current file            */
extern  int filename();             /* Adjust file name             */
extern  int getccol();              /* Get current column           */
extern  int gotobol();              /* Move to start of line        */
extern  int forwchar();             /* Move forward by characters   */
extern  int gotoeol();              /* Move to end of line          */
extern  int backchar();             /* Move backward by characters  */
extern  int forwline();             /* Move forward by lines        */
extern  int backline();             /* Move backward by lines       */
extern  int forwpage();             /* Move forward by pages        */
extern  int backpage();             /* Move backward by pages       */
extern  int gotobob();              /* Move to start of buffer      */
extern  int gotoeob();              /* Move to end of buffer        */
extern  int setfillcol();           /* Set fill column.             */
extern  int setmark();              /* Set mark                     */
extern  int swapmark();             /* Swap "." and mark            */
extern  int tagjump();              /* Swap "." and mark            */
extern  int forwsearch();           /* Search forward               */
extern  int backsearch();           /* Search backwards             */
extern  int qforwsearch();          /* Search forward               */
extern  int qbacksearch();          /* Search backwards             */
extern  int sreplace();             /* search and replace           */
extern  int qreplace();             /* search and replace w/query   */
extern  int showcpos();             /* Show the cursor position     */
extern  int nextwind();             /* Move to the next window      */
extern  int prevwind();             /* Move to the previous window  */
extern  int onlywind();             /* Make current window only one */
extern  int splitwind();            /* Split current window         */
extern  int mvdnwind();             /* Move window down             */
extern  int mvupwind();             /* Move window up               */
extern  int enlargewind();          /* Enlarge display window.      */
extern  int shrinkwind();           /* Shrink window.               */
extern  int listbuffers();          /* Display list of buffers      */
extern  int usebuffer();            /* Switch a window to a buffer  */
extern  int killbuffer();           /* Make a buffer go away.       */
extern  int reposition();           /* Reposition window            */
extern  int refresh();              /* Refresh the screen           */
extern  int twiddle();              /* Twiddle characters           */
extern  int tab();                  /* Insert tab                   */
extern  int newline();              /* Insert CR-LF                 */
extern  int indent();               /* Insert CR-LF, then indent    */
extern  int openline();             /* Open up a blank line         */
extern  int deblank();              /* Delete blank lines           */
extern  int quote();                /* Insert literal               */
extern  int backword();             /* Backup by words              */
extern  int forwword();             /* Advance by words             */
extern  int forwdel();              /* Forward delete               */
extern  int backdel();              /* Backward delete              */
extern  int killtext();             /* Kill forward                 */
extern  int killline();             /* Kill current line            */
extern  int yank();                 /* Yank back from killbuffer.   */
extern  int upperword();            /* Upper case word.             */
extern  int lowerword();            /* Lower case word.             */
extern  int upperregion();          /* Upper case region.           */
extern  int lowerregion();          /* Lower case region.           */
extern  int capword();              /* Initial capitalize word.     */
extern  int delfword();             /* Delete forward word.         */
extern  int delbword();             /* Delete backward word.        */
extern  int killregion();           /* Kill region.                 */
extern  int copyregion();           /* Copy region to kill buffer.  */
extern  int spawncli();             /* Run CLI in a subjob.         */
extern  int spawn();                /* Run a command in a subjob.   */
#if BSD
extern  int bktoshell();            /* suspend emacs to parent shell*/
extern  int rtfrmshell();           /* return from a suspended state*/
#endif
extern  int quickexit();            /* low keystroke style exit.    */
extern  int _setmode();             /* set an editor mode           */
extern  int delmode();              /* delete a mode                */
extern  int gotoline();             /* go to a numbered line        */
extern  int namebuffer();           /* rename the current buffer    */
extern  int swlinenum();
extern  int toggleover();
extern  int switchtab();
extern  int toggletabdisp();
extern  int help();                 /* get the help file here       */
extern  int deskey();               /* describe a key's binding     */
extern  int viewfile();             /* find a file in view mode     */
extern  int insfile();              /* insert a file                */
extern  int scrnextup();            /* scroll next window back      */
extern  int scrnextdw();            /* scroll next window down      */
extern  int bindtokey();            /* bind a function to a key     */
extern  int unbindkey();            /* unbind a key's function      */
extern  int namedcmd();             /* execute named command        */
extern  int desbind();              /* describe bindings            */
extern  int execcmd();              /* execute a command line       */
extern  int execbuf();              /* exec commands from a buffer  */
extern  int execfile();             /* exec commands from a file    */
extern  int nextbuffer();           /* switch to the next buffer    */
#if CFENCE
extern  int getfence();             /* kill the current paragraph   */
#endif
extern  int setgmode();             /* set a global mode            */
extern  int delgmode();             /* delete a global mode         */
extern  int insspace();             /* insert a space forword       */
extern  int forwhunt();             /* hunt forward for next match  */
extern  int backhunt();             /* hunt backwards for next match*/
extern  int pipe();                 /* pipe command into buffer     */
extern  int filter();               /* filter buffer through dos    */
extern  int delwind();              /* delete the current window    */
extern  int cbuf1();                /* execute numbered comd buffer */
extern  int cbuf2();
extern  int cbuf3();
extern  int cbuf4();
extern  int cbuf5();
extern  int cbuf6();
extern  int cbuf7();
extern  int cbuf8();
extern  int cbuf9();
extern  int cbuf10();
extern  int cbuf11();
extern  int cbuf12();
extern  int cbuf13();
extern  int cbuf14();
extern  int cbuf15();
extern  int cbuf16();
extern  int storemac();             /* store text for macro         */
extern  int resize();               /* resize current window        */
extern  int clrmes();               /* clear the message line       */
extern  int meta();                 /* meta prefix dummy function   */
extern  int cex();                  /* ^X prefix dummy function     */
extern  int istring();              /* insert string in text        */
extern  int unmark();               /* unmark current buffer        */
#if ISRCH
extern  int fisearch();             /* forward incremental search   */
extern  int risearch();             /* reverse incremental search   */
#endif
extern  int savewnd();              /* save current window          */
extern  int restwnd();              /* restore current window       */
extern  int upscreen();             /* force screen update          */
extern  int writemsg();             /* write text on message line   */

/*  Name to function binding table

        This table gives the names of all the bindable functions
        end their C function address. These are used for the bind-to-key
        function.
*/
NBIND names[] =
{
    { "add-mode",                   _setmode },
    { "add-global-mode",            setgmode },
    { "backward-character",         backchar },
    { "begin-macro",                ctlxlp },
    { "begining-of-file",           gotobob },
    { "begining-of-line",           gotobol },
    { "bind-to-key",                bindtokey },
    { "buffer-position",            showcpos },
    { "case-region-lower",          lowerregion },
    { "case-region-upper",          upperregion },
    { "case-word-capitalize",       capword },
    { "case-word-lower",            lowerword },
    { "case-word-upper",            upperword },
    { "change-file-name",           filename },
    { "clear-and-redraw",           refresh },
    { "clear-message-line",         clrmes },
    { "copy-region",                copyregion },
    { "ctlx-prefix",                cex },
    { "delete-blank-lines",         deblank },
    { "delete-buffer",              killbuffer },
    { "delete-mode",                delmode },
    { "delete-global-mode",         delgmode },
    { "delete-next-character",      forwdel },
    { "delete-next-word",           delfword },
    { "delete-other-windows",       onlywind },
    { "delete-previous-character",   backdel },
    { "delete-previous-word",       delbword },
    { "delete-window",              delwind },
    { "describe-bindings",          desbind },
    { "describe-key",               deskey },
    { "end-macro",                  ctlxrp },
    { "end-of-file",                gotoeob },
    { "end-of-line",                gotoeol },
    { "exchange-point-and-mark",    swapmark },
    { "execute-buffer",             execbuf },
    { "execute-command-line",       execcmd },
    { "execute-file",               execfile },
    { "execute-macro",              ctlxe },
    { "execute-macro-1",            cbuf1 },
    { "execute-macro-2",            cbuf2 },
    { "execute-macro-3",            cbuf3 },
    { "execute-macro-4",            cbuf4 },
    { "execute-macro-5",            cbuf5 },
    { "execute-macro-6",            cbuf6 },
    { "execute-macro-7",            cbuf7 },
    { "execute-macro-8",            cbuf8 },
    { "execute-macro-9",            cbuf9 },
    { "execute-macro-10",           cbuf10 },
    { "execute-macro-11",           cbuf11 },
    { "execute-macro-12",           cbuf12 },
    { "execute-macro-13",           cbuf13 },
    { "execute-macro-14",           cbuf14 },
    { "execute-macro-15",           cbuf15 },
    { "execute-macro-16",           cbuf16 },
    { "execute-named-command",      namedcmd },
    { "exit-emacs",                 quit },
    { "filter-buffer",              filter },
    { "find-file",                  filefind },
    { "forward-character",          forwchar },
    { "get-fence",                  getfence },
    { "goto-line",                  gotoline },
    { "grow-window",                enlargewind },
    { "handle-tab",                 tab },
    { "hunt-forward",               forwhunt },
    { "hunt-backward",              backhunt },
    { "help",                       help },
    { "i-shell",                    spawncli },
#if ISRCH
    { "incremental-search",         fisearch },
#endif
    { "insert-file",                insfile },
    { "insert-space",               insspace },
    { "insert-string",              istring },
    { "kill-region",                killregion },
    { "kill-to-end-of-line",        killtext },
    { "kill-line",                  killline },
    { "list-buffers",               listbuffers },
    { "meta-prefix",                meta },
    { "move-window-down",           mvdnwind },
    { "move-window-up",             mvupwind },
    { "name-buffer",                namebuffer },
    { "newline",                    newline },
    { "newline-and-indent",         indent },
    { "next-buffer",                nextbuffer },
    { "next-line",                  forwline },
    { "next-page",                  forwpage },
    { "next-window",                nextwind },
    { "next-word",                  forwword },
    { "open-line",                  openline },
    { "pipe-command",               pipe },
    { "previous-line",              backline },
    { "previous-page",              backpage },
    { "previous-window",            prevwind },
    { "previous-word",              backword },
    { "query-replace-string",       qreplace },
    { "quick-exit",                 quickexit },
    { "quote-character",            quote },
    { "read-file",                  fileread },
    { "redraw-display",             reposition },
    { "resize-window",              resize },
    { "restore-window",             restwnd },
    { "replace-string",             sreplace },
#if ISRCH
    { "reverse-incremental-search", risearch },
#endif
    { "save-file",                  filesave },
    { "save-window",                savewnd },
    { "scroll-next-up",             scrnextup },
    { "scroll-next-down",           scrnextdw },
    { "search-forward",             forwsearch },
    { "search-forward-q",           qforwsearch },
    { "search-reverse",             backsearch },
    { "search-reverse-q",           qbacksearch },
    { "select-buffer",              usebuffer },
    { "set-fill-column",            setfillcol },
    { "set-mark",                   setmark },
    { "shell-command",              spawn },
    { "shrink-window",              shrinkwind },
    { "split-current-window",       splitwind },
    { "store-macro",                storemac },
#if BSD
    { "suspend-emacs",              bktoshell },
#endif
    { "tag-jump",                   tagjump },
    { "toggle-cmode-tab",           cmodetab },
    { "toggle-linenum",             swlinenum },
    { "toggle-over",                toggleover },
    { "toggle-tab",                 switchtab },
    { "toggle-tab-disp",            toggletabdisp },
    { "transpose-characters",       twiddle },
    { "unbind-key",                 unbindkey },
    { "unmark-buffer",              unmark },
    { "update-screen",              upscreen },
    { "view-file",                  viewfile },
    { "write-file",                 filewrite },
    { "write-message",              writemsg },
    { "yank",                       yank },

    { "",                           NULL }
};
