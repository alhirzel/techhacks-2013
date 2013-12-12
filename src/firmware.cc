#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define ON_PIN 0
#define OFF_PIN 1

#define RELAY_COIL_TIME (10)

bool state;

void wait (unsigned ms) {
	for (unsigned i = 0; i < ms / 10; ++i)
		_delay_ms (10);
}

void switch_on () {
	PORTB |= 1 << ON_PIN;
	wait (RELAY_COIL_TIME);
	PORTB &= ~(1 << ON_PIN);
	wait (RELAY_COIL_TIME);
}

void switch_off () {
	PORTB |= 1 << OFF_PIN;
	wait (RELAY_COIL_TIME);
	PORTB &= ~(1 << OFF_PIN);
	wait (RELAY_COIL_TIME);
}

int main () {
	state = false;
	// Enable LED output.
	DDRB = (1 << 5) | (1 << ON_PIN) | (1 << OFF_PIN);
	// Go to power down mode when sleep is executed.
	SMCR |= (1 << SM0) | (1 << SM1) | (1 << SM2) | (1 << SE);
	// Force relay off.
	switch_off ();
	// Enable int0 for any change.
	EICRA = 1 << ISC00;
	EIMSK |= 1 << INT0;
	EIFR |= 1 << INTF0;
	// Pull up INT0.
	PORTD = 1 << 2;
	// Set up serial port.
	UCSR0A = 0x40;
	UCSR0B = 0x08;
	UCSR0C = 0x06;
	UBRR0H = 0;
	UBRR0L = 8;
	// Enable interrupts and wait forever.
	sei ();
	while (1) {
		//__asm__ volatile ("sleep");
	}
}

#define NOW_OFF (0x00)
#define NOW_ON (0x01)

ISR (INT0_vect) {
	_delay_ms (10);
	uint8_t state = 0x01 ^ ((PIND & (1 << 2)) >> 2);

	static uint8_t old_state = 0;

	if (old_state != state) {
		old_state = state;

		if (state == NOW_ON) {
			UDR0 = 'N';
			switch_off ();
		} else {
			UDR0 = 'F';
			switch_on ();
			wait (30000);
			switch_off ();
			wait (1000);
		}
	}
	// clear flag
	//EIFR |= 1 << INTF0;
	/*
	wait (100);
	bool newstate = ~PIND & (1 << 2);
	if (state != newstate) {
		state = newstate;
		if (!state) {
			UDR0 = 'A';
			// Switched off.
			switch_on ();
			wait (5000);
			switch_off ();
		}
		else {
			UDR0 = 'B';
		}
	}
	*/
}
