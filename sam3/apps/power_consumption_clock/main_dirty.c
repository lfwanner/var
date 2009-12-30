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

#include <lcd/color.h>
#include <lcd/lcdd.h>
#include <lcd/draw.h>

#define ITERATIONS 200000

#define MULTIPLIERS 10

const Pin pinPB1 = PIN_PUSHBUTTON_1;
const Pin pinPB2 = PIN_PUSHBUTTON_2;
const Pin led0 = PIN_LED_0;
const Pin led1 = PIN_LED_1;

extern void DHRY_testloop(int);

    void *pLcdBase = (void *)BOARD_LCD_BASE;


/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}


void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
} 



volatile unsigned int clock;

unsigned int get_clock() {




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


/*
void configureClock(unsigned short multiplier) {


	// Check if MCK source is RC or XT
	if((AT91C_BASE_PMC->PMC_MOR & AT91C_CKGR_MOSCSEL) == 0) {
		// it is RC, turn on XT
		AT91C_BASE_PMC->PMC_MOR = (0X37 << 16) | AT91C_CKGR_MOSCXTST | AT91C_CKGR_MOSCRCEN | AT91C_CKGR_MOSCXTEN;
		while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCXTS));
	}

	AT91C_BASE_PMC->PMC_MOR = (0X37 << 16) | AT91C_CKGR_MOSCXTST | AT91C_CKGR_MOSCRCEN | AT91C_CKGR_MOSCXTEN | AT91C_CKGR_MOSCSEL;	
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCXTS));

	AT91C_BASE_PMC->PMC_MCKR = AT91C_BASE_PMC->PMC_MCKR | AT91C_PMC_CSS_MAIN_CLK;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

	AT91C_BASE_PMC->PMC_PLLAR = (1 << 29) | ((multiplier - 1) << 16) | (0x3F << 8) | (1 << 0) | AT91C_CKGR_STMODE_0;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA));

	AT91C_BASE_PMC->PMC_MCKR |= AT91C_PMC_CSS_MAIN_CLK;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

	AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_PRES) | AT91C_PMC_PRES_CLK | AT91C_PMC_CSS_PLLA_CLK;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

}

*/



//------------------------------------------------------------------------------
/// Test Active Mode
//------------------------------------------------------------------------------
void TestActiveMode(int m, int d, int iterations)
{


    CLOCK_SetMultiplier(m,d);
    TRACE_CONFIGURE(DBGU_STANDARD, 57600, get_clock());

//   int i = 100 - 20*d + 20*m;


   printf("Start: %d",clock);

/*
    volatile char buffer[32];
    itoa(clock, buffer);
    LCDD_Initialize();
    LCDD_Start();
    //LCDD_Fill((void *)BOARD_LCD_BASE, COLOR_WHITE);
    LCDD_DrawString(pLcdBase, 1, i, buffer, COLOR_BLACK);

*/

        DHRY_testloop(iterations);


   printf("Stop:  %d\0");



        DHRY_testloop(100);

  // Wait for user to change clock, press button 
/*
  PIO_Configure(&led0, 1);  
  PIO_Clear(&led0);
  PIO_Configure(&pinPB2, 1);
  while(PIO_Get(&pinPB2)) {

  }
  PIO_Set(&led0);  
  PIO_Set(&pinPB2);  

*/







}



//-----------------------------------------------------------------------------
/// main function
//-----------------------------------------------------------------------------
int main(void)
{

//LCDD_Stop(); 

    // Initialize LCD
    //LCDD_Initialize();
    //LCDD_Start();
    //LCDD_Fill((void *)BOARD_LCD_BASE, COLOR_WHITE);


    TRACE_CONFIGURE(DBGU_STANDARD, 57600, get_clock());

     // Set default clock
    CLOCK_SetMultiplier(1,4);

    // initialize the chip for the power consumption test
    InitChip();



  int i;


  for (i = 1; i <= MULTIPLIERS; i++) {
 	 TestActiveMode(i,1,ITERATIONS*i);     
    }

    return 0;
}
