#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>



void extended_standby_mode(void) {
	SMCR |= SM2 | SM1 | SM0;
}



void enable_rising_falling_edge_interrupt(void) {
	EIMSK |= INT0;
	// I bit in SREG must be 1
}



void main(void) {


	extended_standby_mode();
}

