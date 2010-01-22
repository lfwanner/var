/*
 * Active Power Consumption @ Different Clock Speeds
 * Lucas Wanner - Dec 28, 2009
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


void InitChip(void) {

	// Disable all the peripheral clocks
	PMC_DisableAllPeripherals();

	// Stop UTMI
	AT91C_BASE_CKGR->CKGR_UCKR &= ~AT91C_CKGR_UPLLEN;

	// Configure all PIO as input
	// Set PIO as Input
	AT91C_BASE_PIOA->PIO_ODR = 0xFFFFFFFF;
	AT91C_BASE_PIOB->PIO_ODR = 0xFFFFFFFF;
	AT91C_BASE_PIOC->PIO_ODR = 0xFFFFFFFF;
	// Enable PIO
	AT91C_BASE_PIOA->PIO_PER = 0xFFFFFFFF;
	AT91C_BASE_PIOB->PIO_PER = 0xFFFFFFFF;
	AT91C_BASE_PIOC->PIO_PER = 0xFFFFFFFF;

	TRACE_CONFIGURE(DBGU_STANDARD, 57600, get_clock());
	
}




//------------------------------------------------------------------------------
/// Test Active Mode
//------------------------------------------------------------------------------
void TestActiveMode(int m, int d, int iterations)
{

	CLOCK_SetMultiplier(m,d);
	TRACE_CONFIGURE(DBGU_STANDARD, 57600, get_clock());

	printf("Start: %d",get_clock());

	DHRY_testloop(iterations);
	printf("Stop:  ");
	print_temperature();

	// This is just to add some delay
	DHRY_testloop(iterations/10);

}

int main(void) {

	TRACE_CONFIGURE(DBGU_STANDARD, 57600, get_clock());
	CLOCK_SetMultiplier(1,4);

	int i;
	for (i = 1; i <= MULTIPLIERS; i++) {
		TestActiveMode(i,1,ITERATIONS*i);     
	}

	return 0;
}
