#include "Timer.h"

void (*timerPulse) ();

void initializeTimer(void (*timerFunc) (), short pulsesPerSecond)
{
	timerPulse = timerFunc;

	// http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/

	// initialize Timer1
	cli();			// disable global interrupts
	TCCR1A = 0;		// set entire TCCR1A register to 0
	TCCR1B = 0;		// same for TCCR1B
 
	// set compare match register to desired timer count:
	OCR1A = (getTimerCountForOneSecond() / pulsesPerSecond) - 1;	// leave one clock cycle, it's needed by the timer when resetting itself to zero

	// turn on CTC mode:
	TCCR1B |= (1 << WGM12);

	// Set CS10 and CS12 bits for 1024 prescaler:
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);

	// enable timer compare interrupt:
	TIMSK1 |= (1 << OCIE1A);

	// enable global interrupts:
	sei();
}

int getTimerCountForOneSecond()
{
	// (target time) = (timer resolution) * (# timer counts + 1)
	// => (# timer counts + 1) = (target time) / (timer resolution)
	// => (# timer counts + 1) = (1 s) / (1 / (clock frequency / prescaler))
	// => (# timer counts + 1) = (clock frequency / prescaler)
	// => (# timer counts + 1) = (16*10^6 / 1024)
	// => (# timer counts + 1) = 15625

	// 16MHz
	return 15625;
}

ISR(TIMER1_COMPA_vect)
{	// This function will trigger on timer-interupts
	timerPulse();
}
