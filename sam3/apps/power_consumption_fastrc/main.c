/*
 * Based on Atmel's test-power-consumption-sam3-project
 * Lucas Wanner - Dec 4, 2009
 */

#include <board.h>
#include <pio/pio.h>
#include <pio/pio_it.h>
#include <irq/irq.h>
#include <pmc/pmc.h>
//#include <dbgu/dbgu.h>
//#include <utility/trace.h>
#include <utility/led.h>
#include <utility/clock.h>
#include <board_lowlevel.h>
//#include <stdio.h>

// Button pin definition for wakeup
#define BUTTON_WAKEUP          (0x1 << 7)

// Definition of Main On-Chip RC Oscillator Frequency Selection
#define FAST_RC_OSC_4MHZ    ((0x0 << 4) & AT91C_CKGR_MOSCRCF)
#define FAST_RC_OSC_8MHZ    ((0x1 << 4) & AT91C_CKGR_MOSCRCF)
#define FAST_RC_OSC_12MHZ   ((0x2 << 4) & AT91C_CKGR_MOSCRCF)

// Pushbutton \#1 pin instance.
const Pin pinPB1 = PIN_PUSHBUTTON_1;
const Pin pinPB2 = PIN_PUSHBUTTON_2;

const Pin led0 = PIN_LED_0;
const Pin led1 = PIN_LED_1;

extern void DHRY_testloop(int);

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
    


    //TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);

}


//------------------------------------------------------------------------------
/// Configure clock by using 32KHz RC Oscillator.
/// \param pres Programmable Clock Prescaler
//------------------------------------------------------------------------------
void ConfigureClockUsing32KRcOsc(unsigned int pres)
{
    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_CSS)
        | AT91C_PMC_CSS_SLOW_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_PRES)
        | pres;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    // Stop PLL A
    // MULA: PLL A Multiplier 0 = The PLL A is deactivated.
    // STMODE must be set at 2 when the PLLA is Off
    AT91C_BASE_PMC->PMC_PLLAR = 0x2 << 14;

    // Stop Main Oscillator
    AT91C_BASE_PMC->PMC_MOR = (0x37 << 16) | (0x3F << 8);
}

//------------------------------------------------------------------------------
/// Configure clock by using Main On-Chip RC Oscillator.
/// \param moscrcf Main On-Chip RC Oscillator Frequency Selection
/// \param pres    Processor Clock Prescaler
//------------------------------------------------------------------------------
void ConfigureClockUsingFastRcOsc(unsigned int moscrcf, unsigned int pres)
{

    // Enable Fast RC oscillator but DO NOT switch to RC now . Keep MOSCSEL to 1
    AT91C_BASE_PMC->PMC_MOR = AT91C_CKGR_MOSCSEL | (0x37 << 16)
        | AT91C_CKGR_MOSCXTEN | AT91C_CKGR_MOSCRCEN;
    // Wait the Fast RC to stabilize
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCRCS));

    // Switch from Main Xtal osc to Fast RC
    AT91C_BASE_PMC->PMC_MOR = (0x37 << 16) | AT91C_CKGR_MOSCRCEN | AT91C_CKGR_MOSCXTEN ;
    // Wait for Main Oscillator Selection Status bit MOSCSELS
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCSELS));


    // Disable Main XTAL Oscillator
    AT91C_BASE_PMC->PMC_MOR = (0x37 << 16) | AT91C_CKGR_MOSCRCEN;


    // Change frequency of Fast RC oscillator
    AT91C_BASE_PMC->PMC_MOR = (0x37 << 16) | AT91C_BASE_PMC->PMC_MOR | moscrcf;
    // Wait the Fast RC to stabilize
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCRCS));

    // Switch to main clock
    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_CSS)
        | AT91C_PMC_CSS_MAIN_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_PRES)
        | pres;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    // Stop PLL A  PIO_Configure(&led1, 1);  

    // MULA: PLL A Multiplier 0 = The PLL A is deactivated.
    // STMODE must be set at 2 when the PLLA is Off
    AT91C_BASE_PMC->PMC_PLLAR = 0x2 << 14;
}

//------------------------------------------------------------------------------
/// Test Active Mode
//------------------------------------------------------------------------------
void TestActiveMode(void)
{

  PIO_Configure(&led0, 1);  
  PIO_Clear(&led0);
  
  PIO_Configure(&pinPB2, 1);   
  while(PIO_Get(&pinPB2)) {
    DHRY_testloop(1);
  }

	
  PIO_Set(&led0);
	
  PIO_Configure(&led1, 1);  
  PIO_Clear(&led1);
  PIO_Configure(&pinPB1, 1);   
  while(PIO_Get(&pinPB1)) {  }
  PIO_Set(&led1);  


}



//-----------------------------------------------------------------------------
/// test Core consumption
//-----------------------------------------------------------------------------
void TestCore(void)
{
	// 125KHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_32);
	TestActiveMode();     
	// 250KHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_16);
	TestActiveMode();     
	// 500KHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_8);
	TestActiveMode();     
	// 1MHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_4);
	TestActiveMode();     
	// 2MHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_2);
	TestActiveMode();     
	// 4MHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK);
	TestActiveMode();     
	// 8MHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_8MHZ, AT91C_PMC_PRES_CLK);
	TestActiveMode();     
	// 12MHz
	ConfigureClockUsingFastRcOsc(FAST_RC_OSC_12MHZ, AT91C_PMC_PRES_CLK);
	TestActiveMode();     
	
}

//-----------------------------------------------------------------------------
/// main function
//-----------------------------------------------------------------------------
int main(void)
{

	//  TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);

	//ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK);
	
	InitChip();
	
	// Set default clock
	//CLOCK_SetConfig(0);

	PIO_Configure(&led1, 1);  
	PIO_Clear(&led1);

	PIO_Configure(&led0, 1);  
	PIO_Clear(&led0);		

	// Set flash WS
	SetFlashWaitState(3);




	// initialize the chip for the power consumption test


	// Test core consumption
	TestCore();


	return 0;
}
