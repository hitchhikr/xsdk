#include	"sub.h"
#include	"ship.h"
#include	"enemy.h"
#include	"enemove.h"
#include	"sound.h"

#define	XMAX		255
#define	XMIN		0
#define	YCNT		128

#define	RATE		DOT_RATE
#define	RATE2		( DOT_RATE * 2 )
#define	RATE3		( DOT_RATE * 3 )
#define	RATE8		( DOT_RATE * 8 )
#define	RATE16		( DOT_RATE * 16 )
#define	HSCRL		( -DOT_RATE / 2 )

#define	SITA		(28*8)
#define	UE		(5*8+8)

ENEINFO	eneinfo[] = {
	/*ene,str,point, spsize,    rx,    ry,       func,    sdno */
	{   1,  1,  100, CHR1x1, RATE8, RATE8, dummy_move, SD_ZOUT } ,	/* 0 */
	{   1,  1,  100, CHR1x1, RATE8, RATE8, ene1_move , SD_ZOUT } ,
	{   1,  1,  200, CHR1x1, RATE8, RATE8, ene2_move , SD_ZOUT } ,
	{   1,  1,  100, CHR1x1, RATE8, RATE8, ene3_move , SD_ZOUT } ,
	{   1,  1,  300, CHR1x1, RATE8, RATE8, ene4_move , SD_DOUT } ,
	{  10,  1, 1000, CHR2x1,RATE16, RATE8, ene5_move , SD_HOUT } ,	/* 5*/
	{   1,  1,  300, CHR1x1, RATE8, RATE8, ene6_move , SD_ZOUT } ,
	{   1,  1,  300, CHR1x1, RATE8, RATE8, ene7_move , SD_ZOUT } ,
	{   1,  1,  100, CHR1x1, RATE8, RATE8, ene8_move , SD_ZOUT } ,
	{   1,  1,  100, CHR1x1, RATE8, RATE8, ene9_move , SD_DOUT } ,
	{  16,  1, 4000, CHR3x3,RATE16,RATE16, ene10_move, SD_HOUT } 
} ;

enepat	enepattern[ ROUNDMAX ][ ENEMAX ] = {
	{
						/* round 1 */

		/* cnt ,typ,   x,    y,    vx,    vy, dir, flg */

		{   16,  1, XMAX,   96, -RATE,     0,  0, 0 } ,	/* Formation 1 */
		{   17,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   18,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   19,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   20,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   21,  1, XMAX,   96, -RATE,     0,  0, 0 } ,

		{   28,  1, XMAX,  160, -RATE,     0,  0, 0 } ,	/* Formation 2 */
		{   29,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{   30,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{   31,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{   32,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{   33,  1, XMAX,  160, -RATE,     0,  0, 0 } ,


		{   48,  2, XMAX,   64, -RATE,     0,  8, 0 } ,	/* Formation 3 */
		{   50,  2, XMAX,  128, -RATE,     0,  8, 0 } ,
		{   52,  2, XMAX,  160, -RATE,     0,  8, 0 } ,

		{   60,  2, XMAX,   64, -RATE,     0,  8, 0 } ,	/* Formation 4 */
		{   62,  2, XMAX,   96, -RATE,     0,  8, 0 } ,
		{   64,  2, XMAX,  128, -RATE,     0,  8, 0 } ,

		{   72,  2, XMAX,  192, -RATE,     0,  8, 0 } ,	/* Formation 4 */
		{   74,  2, XMAX,   64, -RATE,     0,  8, 0 } ,
		{   76,  2, XMAX,   96, -RATE,     0,  8, 0 } ,


		{   90,  1, XMAX,   96, -RATE,     0,  0, 0 } ,	/* Formation 5 */
		{   91,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   92,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   93,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   94,  1, XMAX,   96, -RATE,     0,  0, 0 } ,
		{   95,  1, XMAX,   96, -RATE,     0,  0, 0 } ,

		{  100,  1, XMAX,  160, -RATE,     0,  0, 0 } ,	/* Formation 6 */
		{  101,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{  102,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{  103,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{  104,  1, XMAX,  160, -RATE,     0,  0, 0 } ,
		{  105,  1, XMAX,  160, -RATE,     0,  0, 0 } ,

		{  114,  3, XMAX,   48,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  114,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  114,  3, XMAX,   80,-RATE2,     0,  0, 0 } ,

		{  120,  3, XMAX,  176,-RATE2,     0,  0, 0 } ,
		{  120,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  120,  3, XMAX,  208,-RATE2,     0,  0, 0 } ,

		{  127,  2, XMAX,   64, -RATE,     0,  8, 0 } ,	/* Formation (5 directions) */
		{  127,  2, XMAX,   96, -RATE,     0,  8, 0 } ,
		{  127,  2, XMAX,  128, -RATE,     0,  8, 0 } ,
		{  127,  2, XMAX,  144, -RATE,     0,  8, 0 } ,
		{  127,  2, XMAX,  160, -RATE,     0,  8, 0 } ,

		{  137,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  137,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,

		{  140,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,	/* Fixed turret */
		{  142,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  150,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  152,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,

		{  160,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  160,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  160,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  160,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  162,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  165,  5, XMAX, SITA, HSCRL,     0,  0, 0 } ,

		{  169,  9, XMIN, SITA,     0,     0,  0, 0 } ,	/* Temp */

		{  175,  3, XMAX,   48,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  175,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  175,  3, XMAX,   80,-RATE2,     0,  0, 0 } ,

		{  190,  3, XMAX,  176,-RATE2,     0,  0, 0 } ,
		{  190,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  190,  3, XMAX,  208,-RATE2,     0,  0, 0 } ,

		{  196,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  200,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  202,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  210,  4, XMAX,  144, HSCRL,     0,  0, 0 } ,

		{  215,  3, XMAX,   48,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  215,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  215,  3, XMAX,   80,-RATE2,     0,  0, 0 } ,

		{  217,  3, XMAX,   48,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  217,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  217,  3, XMAX,   80,-RATE2,     0,  0, 0 } ,

		{  224,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  225,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  227,  5, XMAX,   UE, HSCRL,     0,  0, 1 } ,

		{  234,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  234,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  234,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  240,  7, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  244,  7, XMAX,  116, HSCRL,     0,  0, 1 } ,
		{  246,  7, XMAX, SITA, HSCRL,     0,  0, 0 } ,

		{  251,  3, XMAX,  176,-RATE2,     0,  0, 0 } ,/* Sin Curve */
		{  251,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  251,  3, XMAX,  208,-RATE2,     0,  0, 0 } ,

		{  256,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  258,  4, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  258,  4, XMAX, SITA, HSCRL,     0,  0, 0 } ,
		{  261,  5, XMAX,   UE, HSCRL,     0,  0, 1 } ,
		{  261,  5, XMAX, SITA, HSCRL,     0,  0, 0 } ,

		{  273,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  273,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,

		{  300, 10, XMAX,  128, HSCRL,     0,  0, 0 } ,	/* Boss */
		{32767,  1, XMAX,  160, -RATE,     0,  0, 0 }	/* EOP(end of pattern) */
	},

	/*			STAGE 2			*/

	{
		/* cnt ,typ,    x,    y,    vx,    vy, dir, flg */

		{   16,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,	/* Formation 1 */
		{   17,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   18,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   19,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   20,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   21,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,

		{   25,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,	/* Formation 2 */
		{   26,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   27,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   28,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   29,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   30,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,

		{   36,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,	/* Formation 1 */
		{   37,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   38,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   39,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   40,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,
		{   41,  1, XMAX,   96,-RATE2,     0,  0, 0 } ,

		{   48,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,	/* Formation 2 */
		{   49,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   50,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   51,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   52,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,
		{   53,  1, XMAX,  160,-RATE2,     0,  0, 0 } ,

		{   65,  8, XMAX,   64, -RATE,     0,  0, 0 } ,	/* Sin formation */
		{   66,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   67,  8, XMAX,   64, -RATE,     0,  0, 0 } ,
		{   68,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   69,  8, XMAX,   64, -RATE,     0,  0, 0 } ,
		{   70,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   71,  8, XMAX,   64, -RATE,     0,  0, 0 } ,
		{   72,  8, XMAX,   64, -RATE,     0,  0, 1 } ,

		{   86,  8, XMAX,  192, -RATE,     0,  0, 0 } ,	/* Sin formation */
		{   87,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   88,  8, XMAX,  192, -RATE,     0,  0, 0 } ,
		{   89,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   90,  8, XMAX,  192, -RATE,     0,  0, 0 } ,
		{   91,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   92,  8, XMAX,  192, -RATE,     0,  0, 0 } ,
		{   93,  8, XMAX,  192, -RATE,     0,  0, 1 } ,

		{  114,  3, XMAX,   48,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  114,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,
		{  114,  3, XMAX,   80,-RATE2,     0,  0, 0 } ,

		{  120,  3, XMAX,  176,-RATE2,     0,  0, 0 } ,
		{  120,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  120,  3, XMAX,  208,-RATE2,     0,  0, 0 } ,

		{  162,  4, XMAX,   40, HSCRL,     0,  0, 1 } , /* Stonehenge 1 */
		{  163,  4, XMAX,   88, HSCRL,     0,  0, 0 } ,
		{  164,  4, XMAX,   40, HSCRL,     0,  0, 1 } ,
		{  164,  4, XMAX,  232, HSCRL,     0,  0, 0 } ,
		{  165,  4, XMAX,   88, HSCRL,     0,  0, 0 } ,
		{  166,  4, XMAX,  232, HSCRL,     0,  0, 0 } ,
		{  168,  4, XMAX,   48, HSCRL,     0,  0, 1 } ,
		{  168,  4, XMAX,  232, HSCRL,     0,  0, 0 } ,
		{  169,  5, XMAX,  136, HSCRL,     0,  0, 1 } ,
		{  170,  4, XMAX,   48, HSCRL,     0,  0, 1 } ,

		{  178,  4, XMAX,  232, HSCRL,     0,  0, 0 } , /* Stone henge 2 */
		{  181,  4, XMAX,  128, HSCRL,     0,  0, 0 } ,
		{  183,  4, XMAX,  128, HSCRL,     0,  0, 0 } ,
		{  186,  5, XMAX,  128, HSCRL,     0,  0, 0 } ,
		{  188,  4, XMAX,  240, HSCRL,     0,  0, 0 } ,
		{  188,  4, XMAX,  168, HSCRL,     0,  0, 1 } ,
		{  189,  4, XMAX,  208, HSCRL,     0,  0, 0 } ,
		{  191,  5, XMAX,  168, HSCRL,     0,  0, 1 } ,
		{  192,  5, XMAX,  128, HSCRL,     0,  0, 0 } ,
		{  194,  4, XMAX,  168, HSCRL,     0,  0, 1 } ,

		{  214,  4, XMAX,  128, HSCRL,     0,  0, 1 } , /* Stone henge 3 */
		{  216,  4, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  219,  5, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  222,  4, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  224,  4, XMAX,  128, HSCRL,     0,  0, 1 } ,

		{  227,  4, XMAX,   88, HSCRL,     0,  0, 0 } ,
		{  228,  5, XMAX,   40, HSCRL,     0,  0, 1 } ,
		{  229,  4, XMAX,   88, HSCRL,     0,  0, 0 } ,
		{  231,  4, XMAX,   40, HSCRL,     0,  0, 1 } ,
		{  232,  5, XMAX,   88, HSCRL,     0,  0, 0 } ,
		{  232,  4, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  233,  4, XMAX,   40, HSCRL,     0,  0, 1 } ,
		{  234,  4, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  237,  5, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  239,  5, XMAX,   32, HSCRL,     0,  0, 1 } ,
		{  239,  4, XMAX,  104, HSCRL,     0,  0, 0 } ,
		{  240,  5, XMAX,  128, HSCRL,     0,  0, 1 } ,
		{  241,  4, XMAX,  104, HSCRL,     0,  0, 0 } ,

		{  305, 10, XMAX,  128, HSCRL,     0,  0, 0 } ,	/* Boss */
		{32767,  1, XMAX,  160, -RATE,     0,  0, 0 }	/* EOP(end of pattern) */
	},

	/*			STAGE 3			*/

	{

		/* cnt,typ,    x,    y,    vx,    vy, dir, flg */
		{   15,  8, XMAX,   64, -RATE,     0,  0, 0 } ,	/* Sin formation */
		{   15,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   16,  8, XMAX,   64, -RATE,     0,  0, 0 } ,
		{   16,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   17,  8, XMAX,   64, -RATE,     0,  0, 0 } ,
		{   17,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   18,  8, XMAX,   64, -RATE,     0,  0, 0 } ,
		{   18,  8, XMAX,   64, -RATE,     0,  0, 1 } ,
		{   25,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{   25,  3, XMAX,  176,-RATE2,     0,  0, 0 } ,
		{   25,  3, XMAX,  208,-RATE2,     0,  0, 0 } ,

		{   35,  8, XMAX,  192, -RATE,     0,  0, 0 } ,	/* Sin formation */
		{   35,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   36,  8, XMAX,  192, -RATE,     0,  0, 0 } ,
		{   36,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   37,  8, XMAX,  192, -RATE,     0,  0, 0 } ,
		{   37,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   38,  8, XMAX,  192, -RATE,     0,  0, 0 } ,
		{   38,  8, XMAX,  192, -RATE,     0,  0, 1 } ,
		{   45,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{   45,  3, XMAX,   48,-RATE2,     0,  0, 0 } ,
		{   45,  3, XMAX,   80,-RATE2,     0,  0, 0 } ,

		{   60,  3, XMAX,   64,-RATE3,     0,  0, 0 } ,	/* Sin curve */
		{   63,  3, XMAX,  190,-RATE3,     0,  0, 0 } ,
		{   66,  3, XMAX,  104,-RATE3,     0,  0, 0 } ,	/* Sin curve */
		{   68,  3, XMAX,  158,-RATE3,     0,  0, 0 } ,

		{   70,  2, XMAX,   64, -RATE,     0,  8, 0 } , /* The one who chases */
		{   70,  2, XMAX,   96, -RATE,     0,  8, 0 } ,
		{   70,  2, XMAX,  128, -RATE,     0,  8, 0 } ,
		{   70,  2, XMAX,  160, -RATE,     0,  8, 0 } ,
		{   70,  2, XMAX,  192, -RATE,     0,  8, 0 } ,

		{   95,  4, XMAX,   64, HSCRL,     0,  0, 1 } ,
		{   95,  4, XMAX,  208, HSCRL,     0,  0, 0 } ,

		{  101,  3, XMAX,   88,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  101,  3, XMAX,  104,-RATE2,     0,  0, 0 } ,
		{  101,  3, XMAX,  120,-RATE2,     0,  0, 0 } ,

		{  106,  9, XMAX,   64, HSCRL,     0,  0, 1 } , /* Walking guy */
		{  106,  9, XMAX,  208, HSCRL,     0,  0, 0 } , /* Walking guy */

		{  115,  3, XMAX,  152,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  115,  3, XMAX,  168,-RATE2,     0,  0, 0 } ,
		{  115,  3, XMAX,  184,-RATE2,     0,  0, 0 } ,

		{  116,  4, XMAX,   96, HSCRL,     0,  0, 1 } ,
		{  116,  4, XMAX,  208, HSCRL,     0,  0, 0 } ,

		{  123,  4, XMAX,   96, HSCRL,     0,  0, 1 } , /* Fixed artillery turrets */
		{  123,  4, XMAX,  136, HSCRL,     0,  0, 0 } ,
		{  124,  9, XMIN,   64, HSCRL,     0,  0, 1 } , /* Walking guy */
		{  124,  9, XMIN,  208, HSCRL,     0,  0, 0 } , /* Walking guy */
		{  125,  4, XMAX,   96, HSCRL,     0,  0, 1 } ,
		{  125,  4, XMAX,  136, HSCRL,     0,  0, 0 } ,
		{  127,  4, XMAX,   96, HSCRL,     0,  0, 1 } ,
		{  127,  4, XMAX,  136, HSCRL,     0,  0, 0 } ,
		{  129,  4, XMAX,   96, HSCRL,     0,  0, 1 } ,
		{  129,  4, XMAX,  136, HSCRL,     0,  0, 0 } ,
		{  131,  4, XMAX,   96, HSCRL,     0,  0, 1 } ,
		{  131,  4, XMAX,  136, HSCRL,     0,  0, 0 } ,
		{  142,  3, XMAX,  112,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  144,  3, XMAX,  112,-RATE2,     0,  0, 0 } ,

		{  146,  4, XMAX,   48, HSCRL,     0,  0, 1 } ,
		{  148,  4, XMAX,  224, HSCRL,     0,  0, 0 } ,

		{  157,  2, XMAX,   64, -RATE,     0,  8, 0 } , /* The one who chases */
		{  157,  2, XMAX,   96, -RATE,     0,  8, 0 } ,
		{  157,  2, XMAX,  128, -RATE,     0,  8, 0 } ,
		{  160,  4, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  162,  4, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  165,  5, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  169,  3, XMAX,   64,-RATE2,     0,  0, 0 } ,	/* Sin curve */
		{  169,  3, XMAX,  192,-RATE2,     0,  0, 0 } ,
		{  174,  4, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  176,  2, XMAX,   64, -RATE,     0,  8, 0 } , /* The one who chases */
		{  176,  2, XMAX,   96, -RATE,     0,  8, 0 } ,
		{  176,  2, XMAX,  128, -RATE,     0,  8, 0 } ,
		{  177,  5, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  193,  5, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  202,  4, XMAX,   64, HSCRL,     0,  8, 0 } ,
		{  204,  4, XMAX,   64, HSCRL,     0,  8, 0 } ,
		{  213,  3, XMAX,   56,-RATE2,     0,  8, 0 } ,
		{  229,  3, XMAX,   56,-RATE2,     0,  8, 0 } ,

		{  234,  9, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  234,  9, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  234,  4, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  234,  5, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  236,  4, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  238,  4, XMAX,   48, HSCRL,     0,  8, 1 } ,
		{  239,  9, XMIN,   48, HSCRL,     0,  8, 1 } ,
		{  239,  9, XMIN,  224, HSCRL,     0,  8, 0 } ,
		{  241,  4, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  243,  4, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  245,  4, XMAX,  224, HSCRL,     0,  8, 0 } ,
		{  245,  2, XMIN,   64, -RATE,     0,  8, 0 } , /* The one who chases */
		{  245,  2, XMIN,   96, -RATE,     0,  8, 0 } ,
		{  245,  2, XMIN,  128, -RATE,     0,  8, 0 } ,

		{  246,  9, XMIN,   48, HSCRL,     0,  8, 1 } ,
		{  246,  9, XMIN,  224, HSCRL,     0,  8, 0 } ,
		{  256,  5, XMIN,  224, HSCRL,     0,  8, 0 } ,
		{  259,  2, XMAX,   64, -RATE,     0,  8, 0 } , /* The one who chases */
		{  259,  2, XMAX,   96, -RATE,     0,  8, 0 } ,
		{  259,  2, XMAX,  128, -RATE,     0,  8, 0 } ,

		{  300, 10, XMAX,  128, HSCRL,     0,  0, 0 } ,	/* Boss */
		{32767,  1, XMAX,  160, -RATE,     0,  0, 0 }	/* EOP(end of pattern) */

	}
} ;
