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

//-----------------------------------------------------------------------------
/// Enter Sleep Mode
/// Enter condition: WFE or WFI + SLEEPDEEP bit = 0 + LPM bit = 0
//-----------------------------------------------------------------------------
void EnterSleepMode(void)
{
    AT91C_BASE_PMC->PMC_FSMR &= ~AT91C_PMC_LPM;
    AT91C_BASE_NVIC->NVIC_SCR &= ~AT91C_NVIC_SLEEPDEEP;
    // Wake from Interrupt
    __WFI();
}

//-----------------------------------------------------------------------------
/// Interrupt handler for buttons.
//-----------------------------------------------------------------------------
void ISR_ButtonWakeUp(void)
{
    // Check if the button1 has been pressed
    
     
    if (!PIO_Get(&pinPB1)) {
    }
}

//-----------------------------------------------------------------------------
/// Configures an interrupt on the wake-up pins
//-----------------------------------------------------------------------------
void ConfigureWakeUpInterrupt(void)
{
    PIO_InitializeInterrupts(0);
    PIO_ConfigureIt(&pinPB1, (void (*)(const Pin *)) ISR_ButtonWakeUp);
    PIO_EnableIt(&pinPB1);
}

//-----------------------------------------------------------------------------
/// Disable interrupt of wake-up pins
//-----------------------------------------------------------------------------
void DisableWakeUpInterrupt(void)
{
    PIO_DisableIt(&pinPB1);
}

//------------------------------------------------------------------------------
/// Sets the wake-up inputs for fast startup mode registers.
/// \param inputs  Wake up inputs to enable.
//------------------------------------------------------------------------------
void SetWakeUpInputsForFastStartup(unsigned int inputs)
{
    AT91C_BASE_PMC->PMC_FSMR &= ~0xFFFF;
    AT91C_BASE_PMC->PMC_FSMR |= inputs;
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


}

//------------------------------------------------------------------------------
/// Test Sleep Mode
//------------------------------------------------------------------------------
void TestSleepMode(unsigned char key)
{
    unsigned int oldPll;
    unsigned int oldMck;
    unsigned int oldPeriphClk;
    unsigned int temp;



    PIO_Configure(&pinPB1, 1);
    ConfigureWakeUpInterrupt();


    EnterSleepMode();



    DisableWakeUpInterrupt();
    

}






int main(void)
{
  
  //  TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
	
     // Set default clock
    CLOCK_SetConfig(0);

    // Set flash WS
    SetFlashWaitState(3);

    // initialize the chip for the power consumption test
    InitChip();

    // Test core consumption
  PIO_Configure(&led0, 1);  
  PIO_Clear(&led0);


  PIO_Configure(&led1, 1);  
  PIO_Set(&led1);

	while(1){

  TestActiveMode();  
  
  TestSleepMode('1');


}
  	

    return 0;
}
