#include	<iocslib.h>
#include	"sound.h"
#include	"display.h"
#include	"txput.h"
#include	"dsp.h"
#define		BESTER_ENERGY_MAX	30 * 8 * 128

volatile int	VDI_ENE_FLG = 0;
extern int bester_energy;
extern volatile int graphicflag;		/* Between RASTER2 and VDISP, it is 1 */
extern int	ledflag;


void __attribute__((interrupt)) vdisp_int(void)
{
	graphicflag = 0;
	if (ledflag) led_control();
	if (VDI_ENE_FLG) {
		if (VDI_ENE_FLG++ > 40) {
			if (VDI_ENE_FLG == 42) sd_on(SD_PUP);
			bester_energy += 16 * 16;
			if (bester_energy > BESTER_ENERGY_MAX) {
				bester_energy = BESTER_ENERGY_MAX;
				sd_on(SD_NULL);
				VDI_ENE_FLG = -1000;
			}
			energy_disp1(bester_energy);
		}
		if (VDI_ENE_FLG == -980) {
			tx_puts(13, 13, ">START", 14);
			sd_on(SD_GO);
		}
		if (VDI_ENE_FLG == -940) {
			VDI_ENE_FLG = 0;
			tx_puts(10, 13, "            ", 14);
		}
	}
}

void	vdisp_intinit()
{
	if (VDISPST(vdisp_int, 0, 1)) {
		dsperror("V-DISP interrupt already in use.\n");
	}
}
