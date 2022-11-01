/********************************** Attempt2
 * ADC Controlled LED
 *
 * Example which controls the LED Brightness via PWM which is varied based on an ADC Reading
 *
 * Author: Russell Trafford
 * Revision: 1.1
 * Last Revision Date: 12OCT22
 *
 * Change Log:
 *  - Changed LED Pin to 6.0 to allow for control by Timer3B module
 *  - Using Pin 1.1 for being connected to Grove Connector Analog Channel 1
 *
 *
 * Pin Map:
 *               MSP430FR2355
 *            -----------------
 *        /|\.  |                             |
 *         |.    |                               |
 *         --   |RST                       |
 *               |                               |
 *       >---|P1.1/A1      P6.0|--> LED --> Wired to LED 2 through external wire to P6.6
 *
 *       ****WARNING****
 *       Please disconnect J11 and connect to Pin 6.0 to the lower part of the jumper
 *
 *       ____
 *      | NC | <-- Keep this Disconnected
 *      |----  |
 *      | **    | <-- Connect to this part of J11
 *      |____. |
 *
 *
 * Peripherals Used:
 *      ADC12 - Channel 1 (Pin 1.1)
 *      GPIO - Pin 6.0
 *      TimerB0 - UP Mode, CCR1 for PWM Control, controls Pin 6.0
 */
#include <msp430.h> 
/**
 * main.c
 */
void gpioInit();        // Setup the GPIO Pins for controlling the LEDS through PWM (no input arguments)
void timerInit();       // Setup Timer to control the LEDS (no input arguments)
void adcInit();         // Setup ADC to do single, on demand samples (no input arguments)
int readADCSingle();    // Single Read ADC, no input arguments, returns current ADC reading
void setLEDBrightness(int DutyCycle);   // Takes in a duty cycle from 0-100 and converts it to an appropriate CCR value
#include <msp430.h>
/**
 * main.c
 */
void gpioInit();        // Setup the GPIO Pins for controlling the LEDS through PWM (no input arguments)
void timerInit();       // Setup Timer to control the LEDS (no input arguments)
void adcInit();         // Setup ADC to do single, on demand samples (no input arguments)
int readADCSingle();    // Single Read ADC, no input arguments, returns current ADC reading


int main(void)
{
WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
PM5CTL0 &= ~LOCKLPM5;

gpioInit();
adcInit();
timerInit();
while(1){
   int brightness =  readADCSingle();
   brightness /= 50;
    setLEDBrightness(brightness);
}
return 0;
}
/**
 * GPIO INIT
 * Initialize all GPIO Pins. Current Pin configuration:
 * Pin 1.0: OUTPUT, GPIO
 * Pin 1.1: INPUT,  ADC Channel 1
 */
void gpioInit(){
    // Configure GPIO

   //Configure Pin1.1 as analog input A1

        //Dir is do not care for analog.
        P1SEL0 |= BIT1;
        P1SEL1 |= BIT1;


   //configure Pin6.0 as output
        P6DIR |= BIT0;
        P6SEL0 |= BIT0;
        P6SEL1 &= ~BIT0;

    // Configure ADC Pin

}
/**
 * Timer INIT
 * Initialize the Timer Peripheral to control a GPIO Pin with PWM in up mode
 * Timer Mode: Up Mode
 */
void timerInit(){
    // Configure Timer Module to Up Mode
    TB3CCR0 = 1000; //period of signal
    TB3CCTL1 = OUTMOD_7;
    TB3CCR1  = 500;//number pwm goes low. controls square/duty cycle
    TB3CTL = TBSSEL__SMCLK | MC__UP | TBCLR;   //uses sampling clock, up mode, when it clears


    // Configure CCR Registers
}
/**
 * ADC INIT
 * Initialize the ADC Peripheral to read Single measurements on demand.
 * ADC MODE: Single Sample, Single Conversion
 */
void adcInit(){
       ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks 16 clock cycles per sample adcon turns adc on.
       ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer sampling signal from internal way
       ADCCTL2 &= ~ADCRES;                                      // clear ADCRES in ADCCTL
       ADCCTL2 |= ADCRES_2;                                     // 12-bit conversion results
       ADCMCTL0 |= ADCINCH_1;                                   // A1 ADC input select; Vref=AVCC
       ADCIE |= ADCIE0;                                         // Enable ADC conv complete interrupt
}

int readADCSingle(){ //code provided

    ADCCTL0 |= ADCENC | ADCSC;     // Sampling and conversion start
    while (!(ADCIFG & ADCIFG0));   // Wait for sample to be sampled and converted
    return ADCMEM0;

}
void setLEDBrightness(int a){

    TB3CCR1  = (int)(a *9.9);
}
/**

* Set LED Brightness on the LED configured from gpioInit. This modifies the

* CCR register based on the input.

* int DutyCycle: Value between 0-100

*/
