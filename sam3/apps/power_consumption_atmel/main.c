/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

//------------------------------------------------------------------------------
/// \dir "Test-power-consumption-sam3-project"
///
/// !!!Purpose
///
/// The Test power consumption SAM3 project allows to measure the consumption
/// of the core in different modes (active mode, sleep mode, wait mode,
/// backup mode)of AT91SAM3 series and it allows also the measure of the
/// peripheral consumption.
///
/// !!!Requirements
///
/// This package can be used with all Atmel AT91SAM3 evaluation kits.
///
/// !!!Description
///
/// At startup, the program configures all the PIOs as input to avoid
/// parasite consumption (except the DBGU PIOs). Then a menu is displayed.
/// It is divided into 2 parts. First part allows user to change the configuration
/// (MCK/PCK frequency configuration),
/// The second part allows user to enter in a mode To measure consumption.
/// An amperemeter has to be plugged on the board instead of the VDDCORE jumper,
/// VDDIN jumper or VDDBU jumper.
///
/// !!!Usage
///
/// -# Build the program and download it inside the evaluation board. Please
///    refer to the <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6224.pdf">SAM-BA User Guide</a>,
///    the <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">GNU-Based Software Development</a>
///    application note or to the <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">IAR EWARM User Guide</a>,
///    depending on your chosen solution.
/// -# On the computer, open and configure a terminal application
///    (e.g. HyperTerminal on Microsoft Windows) with these settings:
///   - 115200 bauds
///   - 8 bits of data
///   - No parity
///   - 1 stop bit
///   - No flow control
/// -# Start the application.
/// -# Upon startup, the application will output the following lines on the DBGU.
///    \code
///    -- Test SAM3 Power Consumption Project 1.* --
///    -- AT91******-**
///    -- Compiled: *** ** **** **:**:** --
///    \endcode
///
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \unit
///
/// !Purpose
///
/// This file contains all the specific code for the Test-power-consumption-sam3-project.
/// It allows core consumption measure in different modes and in different configurations.
///
/// !Contents
///
/// !See also
///
/// Please refer to the list of functions in the #Overview# tab of this unit
/// for more detailed information.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------
//         Local definitions
//------------------------------------------------------------------------------
/// Button pin definition for wakeup
#define BUTTON_WAKEUP          (0x1 << 7)

/// Definition of Main On-Chip RC Oscillator Frequency Selection
#define FAST_RC_OSC_4MHZ    ((0x0 << 4) & AT91C_CKGR_MOSCRCF)
#define FAST_RC_OSC_8MHZ    ((0x1 << 4) & AT91C_CKGR_MOSCRCF)
#define FAST_RC_OSC_12MHZ   ((0x2 << 4) & AT91C_CKGR_MOSCRCF)

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------
/// Pushbutton \#1 pin instance.
const Pin pinPB1 = PIN_PUSHBUTTON_1;

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Imported functions
//------------------------------------------------------------------------------
extern void DHRY_testloop(int);

//-----------------------------------------------------------------------------
/// initialize the chip
//-----------------------------------------------------------------------------
void InitChip(void)
{
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

    // Reconfigure PIO for DBGU
    TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
}

//-----------------------------------------------------------------------------
/// Enter Sleep Mode
/// Enter condition: WFE or WFI + SLEEPDEEP bit = 0 + LPM bit = 0
//-----------------------------------------------------------------------------
void EnterSleepMode(void)
{
    AT91C_BASE_PMC->PMC_FSMR &= ~AT91C_PMC_LPM;
    AT91C_BASE_NVIC->NVIC_SCR &= ~AT91C_NVIC_SLEEPDEEP;
    __WFI();
}

//-----------------------------------------------------------------------------
/// Enter Wait Mode
/// Enter condition: WFE + SLEEPDEEP bit = 0 + LPM bit = 1
//-----------------------------------------------------------------------------
void EnterWaitMode(void)
{
    AT91C_BASE_PMC->PMC_FSMR |= AT91C_PMC_LPM;
    AT91C_BASE_NVIC->NVIC_SCR &= ~AT91C_NVIC_SLEEPDEEP;
    __WFE();
}

//-----------------------------------------------------------------------------
/// Enter Backup Mode
/// Enter condition: WFE + SLEEPDEEP bit = 1
//-----------------------------------------------------------------------------
void EnterBackupMode(void)
{
    AT91C_BASE_NVIC->NVIC_SCR |= AT91C_NVIC_SLEEPDEEP;
    __WFE();
}

//-----------------------------------------------------------------------------
/// Interrupt handler for buttons.
//-----------------------------------------------------------------------------
void ISR_ButtonWakeUp(void)
{
    // Check if the button1 has been pressed
    if (!PIO_Get(&pinPB1)) {

        // Do nothing here
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
/// Save and restore working clock.
/// Here working clock must be running from PLL
/// and external crystal oscillator is used.
//------------------------------------------------------------------------------
void SaveWorkingClock(unsigned int *pOldPll, unsigned int *pOldMck)
{
    // Save previous values for PLL A and Master Clock configuration
    *pOldPll = AT91C_BASE_CKGR->CKGR_PLLAR;
    *pOldMck = AT91C_BASE_PMC->PMC_MCKR;
}

void RestoreWorkingClock(unsigned int oldPll, unsigned int oldMck)
{
    // Switch to slow clock first
    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_CSS)
        | AT91C_PMC_CSS_SLOW_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_PRES)
        | AT91C_PMC_PRES_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
    
    // Restart Main Oscillator
    AT91C_BASE_PMC->PMC_MOR = (0x37 << 16) | (0x3F<<8) | AT91C_CKGR_MOSCRCEN | AT91C_CKGR_MOSCXTEN;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCXTS));
    // Switch to moscsel
    AT91C_BASE_PMC->PMC_MOR = (0x37 << 16) | (0x3F<<8) | AT91C_CKGR_MOSCRCEN | AT91C_CKGR_MOSCXTEN | AT91C_CKGR_MOSCSEL;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCSELS));

    // Switch to main oscillator
    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_CSS) |
        AT91C_PMC_CSS_MAIN_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    AT91C_BASE_PMC->PMC_MCKR = (AT91C_BASE_PMC->PMC_MCKR & ~AT91C_PMC_PRES)
        | AT91C_PMC_PRES_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    // Restart PLL A
    AT91C_BASE_CKGR->CKGR_PLLAR = oldPll;
    while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA));

    // Switch to fast clock
    AT91C_BASE_PMC->PMC_MCKR = (oldMck & ~AT91C_PMC_CSS) | AT91C_PMC_CSS_MAIN_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));

    AT91C_BASE_PMC->PMC_MCKR = oldMck;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY));
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

    // Stop PLL A
    // MULA: PLL A Multiplier 0 = The PLL A is deactivated.
    // STMODE must be set at 2 when the PLLA is Off
    AT91C_BASE_PMC->PMC_PLLAR = 0x2 << 14;
}

//------------------------------------------------------------------------------
/// Test Active Mode
//------------------------------------------------------------------------------
void TestActiveMode(void)
{
    printf("Enter in Active Mode\n\r");
    printf(" - do dhrystone test in loop\n\r");

    // Run Dhrystone Activity
    printf("Press a key to go out...\n\r");
    while(!DBGU_IsRxReady()) {
        DHRY_testloop(1);
    }
    DBGU_GetChar(); // empty RX buffer

    printf("Exit Active mode\n\r");
}

//-----------------------------------------------------------------------------
/// display menu of sleep mode
//-----------------------------------------------------------------------------
void DisplayMenuSleep(void)
{
    printf("\n\r");
    printf("Select clock configuration for sleep mode:\n\r");
    printf("  0 : PCK=500Hz by using 32K RC Oscillator\n\r");
    printf("  1 : PCK=32KHz by using 32K RC Oscillator\n\r");
    printf("  2 : PCK=125KHz by using Fast RC Oscillator\n\r");
    printf("  3 : PCK=250KHz by using Fast RC Oscillator\n\r");
    printf("  4 : PCK=500KHz by using Fast RC Oscillator\n\r");
    printf("  5 : PCK=1MHz by using Fast RC Oscillator\n\r");
    printf("  6 : PCK=2MHz by using Fast RC Oscillator\n\r");
    printf("  7 : PCK=4MHz by using Fast RC Oscillator\n\r");
    printf("  8 : PCK=8MHz by using Fast RC Oscillator\n\r");
    printf("  9 : PCK=12MHz by using Fast RC Oscillator\n\r");
    printf("  c : Current clock PCK=%dMHz running from PLL\n\r", CLOCK_GetCurrPCK());
    printf("\n\r");
}

//------------------------------------------------------------------------------
/// Test Sleep Mode
//------------------------------------------------------------------------------
void TestSleepMode(void)
{
    unsigned char key = '0';
    unsigned int oldPll;
    unsigned int oldMck;
    unsigned int oldPeriphClk;
    unsigned int temp;

    // Select frequency of sleep mode
    DisplayMenuSleep();
    key = DBGU_GetChar();
    printf("Select option: %c\n\r", key);

    printf("Enter in Sleep Mode\n\r");
    printf(" - Press USR-LEFT button to wakeup\n\r");

    // Save current working clock
    SaveWorkingClock(&oldPll, &oldMck);

    // Save previous values peripheral clock then disable all
    oldPeriphClk = AT91C_BASE_PMC->PMC_PCSR;
    PMC_DisableAllPeripherals();

    // Stop UTMI
    AT91C_BASE_CKGR->CKGR_UCKR &= ~AT91C_CKGR_UPLLEN;

    // Disable Brownout Detector
    AT91C_BASE_SUPC->SUPC_MR |= (0xA5 << 24) | (0x01 << 13);

    // Configure PIO for warkup
    PIO_Configure(&pinPB1, 1);
    ConfigureWakeUpInterrupt();

    // Switch clock frequency
    switch (key) {
        case '0': // 500Hz
            ConfigureClockUsing32KRcOsc(AT91C_PMC_PRES_CLK_64);
            break;
        case '1': // 32KHz
            ConfigureClockUsing32KRcOsc(AT91C_PMC_PRES_CLK);
            break;
        case '2': // 125KHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_32);
            break;
        case '3': // 250KHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_16);
            break;
        case '4': // 500KHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_8);
            break;
        case '5': // 1MHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_4);
            break;
        case '6': // 2MHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK_2);
            break;
        case '7': // 4MHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK);
            break;
        case '8': // 8MHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_8MHZ, AT91C_PMC_PRES_CLK);
            break;
        case '9': // 12MHz
            ConfigureClockUsingFastRcOsc(FAST_RC_OSC_12MHZ, AT91C_PMC_PRES_CLK);
            break;
        case 'c':
        default:
            // do nothing
            break;
    }

    // Enter Sleep Mode
    EnterSleepMode();

    // Restore working clock
    RestoreWorkingClock(oldPll, oldMck);

    DisableWakeUpInterrupt();
    
    // Restore peripheral clock
    AT91C_BASE_PMC->PMC_PCER = oldPeriphClk;

    // Start UTMI
    AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_UPLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKU));

    // Enable Brownout Detector
    temp = AT91C_BASE_SUPC->SUPC_MR & 0x00FFFFFF;
    AT91C_BASE_SUPC->SUPC_MR = (0xA5 << 24) | (temp & (~(0x01 << 13)));

    // Reconfigure DBGU
    DBGU_Configure(DBGU_STANDARD, 115200, CLOCK_GetCurrMCK()*1000000);

    printf("Exit Sleep Mode\n\r");
}

//------------------------------------------------------------------------------
/// Test Wait Mode
//------------------------------------------------------------------------------
void TestWaitMode(void)
{
    unsigned int oldPll;
    unsigned int oldMck;
    unsigned int temp;

    printf("Enter in Wait Mode\n\r");
    printf(" - Switch to 4MHz Fast RC oscillator, PLL stopped\n\r");
    printf(" - Press USR-LEFT button to wakeup\n\r");

    // Save current working clock
    SaveWorkingClock(&oldPll, &oldMck);

    // Stop UTMI
    AT91C_BASE_CKGR->CKGR_UCKR &= ~AT91C_CKGR_UPLLEN;

    // Disable Brownout Detector
    AT91C_BASE_SUPC->SUPC_MR |= (0xA5 << 24) | (0x01 << 13);

    // Configure 4Mhz fast RC oscillator
    ConfigureClockUsingFastRcOsc(FAST_RC_OSC_4MHZ, AT91C_PMC_PRES_CLK);

    // Enter Wait Mode
    PIO_Configure(&pinPB1, 1);
    SetWakeUpInputsForFastStartup(BUTTON_WAKEUP);
    EnterWaitMode();

    // Restore working clock
    RestoreWorkingClock(oldPll, oldMck);

    // Start UTMI
    AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_UPLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKU));

    // Enable Brownout Detector
    temp = AT91C_BASE_SUPC->SUPC_MR & 0x00FFFFFF;
    AT91C_BASE_SUPC->SUPC_MR = (0xA5 << 24) | (temp & (~(0x01 << 13)));

    // Reconfigure DBGU
    DBGU_Configure(DBGU_STANDARD, 115200, CLOCK_GetCurrMCK()*1000000);

    printf("Exit Wait Mode\n\r");
}

//------------------------------------------------------------------------------
/// Test Backup Mode
//------------------------------------------------------------------------------
void TestBackupMode(void)
{
    printf("Enter in Backup Mode\n\r");
    printf(" - Press FWUP button to wakeup\n\r");

    // GPBR0 is for recording times of entering backup mode
    *AT91C_SYS_GPBR += 1;

    // Enable force wake up
    AT91C_BASE_SUPC->SUPC_WUMR |= AT91C_SUPC_WUMR_FWUPEN_ENABLE;

    // Enter Backup Mode
    EnterBackupMode();
}

//-----------------------------------------------------------------------------
/// display test Core menu
//-----------------------------------------------------------------------------
void DisplayMenuCore(void)
{
    printf("\n\r");
    printf("===============================================================\n\r");
    printf("Menu: press a key to change the configuration or select a mode.\n\r");
    printf("===============================================================\n\r");
    printf("Config:\n\r");
    printf("  f : Clock      = PCK=%d MCK=%d\n\r", CLOCK_GetCurrPCK(), CLOCK_GetCurrMCK());
    printf("Mode:\n\r");
    printf("  A : Active Mode\n\r");
    printf("  S : Sleep Mode\n\r");
    printf("  W : Wait Mode\n\r");
    printf("  B : Backup Mode(Had entered %d times).\n\r", (int)*AT91C_SYS_GPBR);
    printf("Quit:\n\r");
    printf("  Q : Quit test.\n\r");
    printf("---------------------------------------------------------------\n\r");
    printf("\n\r");
}

//-----------------------------------------------------------------------------
/// test Core consumption
//-----------------------------------------------------------------------------
void TestCore(void)
{
    unsigned char key = 0;

    while (1)
    {
        DisplayMenuCore();

        key = DBGU_GetChar();
        switch(key)
        {
            // Configuration
            case 'f': CLOCK_UserChangeConfig();    break;

            // Mode test
            case 'A': TestActiveMode();            break;
            case 'S': TestSleepMode();             break;
            case 'W': TestWaitMode();              break;
            case 'B': TestBackupMode();            break;

            // Quit test
            case 'Q': goto test_core_end;

            default :
                printf("This menu does not exist !\n\r");
                break;
        }//switch
    }

test_core_end:
    printf(" Exit core consumption test mode.\n\r");
}

//-----------------------------------------------------------------------------
/// main function
//-----------------------------------------------------------------------------
int main(void)
{
    // Configure the DBGU
    TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
    printf("-- Test SAM3 Power Consumption Project %s --\n\r", SOFTPACK_VERSION);
    printf("-- %s\n\r", BOARD_NAME);
    printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

    // Set default clock
    CLOCK_SetConfig(0);
    printf("The core (PCK) is running @ %dMhz and peripherals (MCK) @ %dMHz\n\r",
           CLOCK_GetCurrPCK(),
           CLOCK_GetCurrMCK());

    // Set flash WS
    SetFlashWaitState(3);

    // initialize the chip for the power consumption test
    InitChip();

    // Test core consumption
    TestCore();

    return 0;
}
