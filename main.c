/**
 * Basic 1ms interupt timer for 16mhz atmega328p.
 *
 * time1 counts down the milliseconds from T1 until it reaches zero.
 * The main loop checks for time1 == 0 then performs the task & resets the countdown.
 *
 * @see http://people.ece.cornell.edu/land/courses/ece4760/Timers/index.html
 */

/********************************************************************************
Includes
********************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>

/********************************************************************************
Macros and Defines
********************************************************************************/

#define T1 1000 // timeout value for the blink (mSec)


/********************************************************************************
Function Prototypes
********************************************************************************/

void initialize(void); //all the usual mcu stuff


/********************************************************************************
Global Variables
********************************************************************************/

volatile unsigned int time1;


/********************************************************************************
Interupt Routines
********************************************************************************/

//timer 0 compare ISR
ISR (TIMER0_COMPA_vect)
{
	// Called every millis

	// 75 cycles in
	// 32 cycles save cpu state
	// 8 cycles overhead
	// 32 cycles resume cpu state
	// 75 cycles out

	// Decrement the time if not already zero
    if (time1 > 0)  --time1;
}


/********************************************************************************
Main
********************************************************************************/

int main (void)
{
	initialize();

	// main loop
    while(1) {
    	// check for countdown reached 0
    	if (time1 == 0) {
    		// reset the timer
    		time1 = T1;
    		// toggle the led
    		PORTB ^= (1 << PB0);
    	}
    }
}

/********************************************************************************
Functions
********************************************************************************/

void initialize(void)
{
	// set up timer 0 for 1 mSec ticks (timer 0 is an 8 bit timer)

	// Interupt mask register - to enable the interupt (datasheet: 14.9.6)
	// (Bit 1 – OCIE0A: Timer/Counter0 Output Compare Match A Interrupt Enable)
	TIMSK0 = (1 << OCIE0A); // (2) turn on timer 0 cmp match ISR

	// Compare register - when to interupt (datasheet: 14.9.4)
	OCR0A = 249; // set the compare reg to 250 time ticks

    // Timer mode (datasheet: 14.9.1)
	TCCR0A = (1 << WGM01); // (0b00000010) turn on clear-on-match

	// Prescaler (datasheet: 14.9.2)
	// calculation to show why 64 is required the prescaler:
	// 1 / (16000000 / 64 / 256) = 0.001024 = 1ms (near enough)
	TCCR0B = ((1 << CS10) | (1 << CS11)); // (0b00000011)(3) clock prescalar to 64

	// Timer initialization
    time1 = T1;

	// Port initialization
	DDRB = (1 << PB0); // set to led pin output
	PORTB |= (1 << PB0); // turn the LED on

	// crank up the ISRs
	sei();
}

