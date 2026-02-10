/*========================================================================
 * PATH:This file contains certain info needed to locate the
 *	MicroEMACS files on a system dependant basis.
 *======================================================================== */

/*----------------------------------------------------------
 *	possible names and paths of help files under OSs	
 *---------------------------------------------------------- */
char *pathname[] =
{
    "emacs.rc",
    "emacs.hlp",
    "",			/* root 	*/
    "",			/* bin		*/
    ""			/* emacs	*/
};

#define	NPNAMES	(sizeof(pathname) / sizeof(char *))
