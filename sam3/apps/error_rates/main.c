/*
 * Error Rates
 * Lucas Wanner - Jan 29, 2010
 */

#include <board.h>
#include <pio/pio.h>
#include <pio/pio_it.h>
#include <irq/irq.h>
#include <pmc/pmc.h>
#include <dbgu/dbgu.h>
#include <utility/trace.h>
#include <utility/led.h>
#include <utility/clock.h>
#include <board_lowlevel.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "itoa.h"

#include <twi/twi.h>
#include <utility/math.h>
#include <utility/assert.h>
#include <drivers/async/async.h>
#include <drivers/twi/twid.h>


#include "temperature.h"

#define ITERATIONS 200000
#define MULTIPLIERS 10


extern void DHRY_testloop(int);




unsigned int get_clock() {

	unsigned int clock;

	while(!(AT91C_BASE_PMC->PMC_MCFR & AT91C_CKGR_MAINRDY));

	clock =  32768/16;
	clock = clock * (AT91C_BASE_PMC->PMC_MCFR & AT91C_CKGR_MAINF) ;
	clock = clock * (((AT91C_BASE_PMC->PMC_PLLAR & AT91C_CKGR_MULA)>>16)+1);


	if ((AT91C_BASE_PMC->PMC_PLLAR & AT91C_CKGR_DIVA) > 0)
		clock = clock / (2*(AT91C_BASE_PMC->PMC_PLLAR & AT91C_CKGR_DIVA));

	return clock;

}

double x;
double xx;
double old_x;


int main(void) {

	TRACE_CONFIGURE(DBGU_STANDARD, 115200, get_clock());
	CLOCK_SetMultiplier(10,1);
	TRACE_CONFIGURE(DBGU_STANDARD, 115200, get_clock());	

	
	while(1) {

		double acc_err = -23;
		old_x = 1;
		int i;
		for (i = 2; i <= 32768; i++) {
			x = (double)i;
			xx = x*(x-0.3f);
			double diff = xx/(x-0.5f);
			//printf("%d,%d,%d\n",i,(long)(xx*2),(long)(1000*(diff-(double)i)));		
			//xx = xx/x;
			//acc_err += (long)(diff-(double)i);

			acc_err += ((x*x)/(old_x*old_x)-1);
			old_x = x;
		}
		acc_err = ((((1000000000*acc_err-593719777)*1000000000)-672498703)*10000000000);
		printf("%d\n",(long)acc_err);		

	}	


	return 0;
}
