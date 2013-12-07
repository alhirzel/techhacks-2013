#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define ON_PIN 0
#define OFF_PIN 1

// States:
// 0: Really off -> 1a.
// 1: Normally on -> 2a.
// 2: Courtesy time -> 0b or 3a.
// 3: Override option (5s) -> 1b or 0a.

enum State {
	OFF,
	ON,
	COURTESY,
	OVERRIDE_OPTION
} state;
int delay;

int main () {
	state = OFF;
	// Enable LED output.
	DDRB = (1 << 5) | (1 << 1) | (1 << 0);
	// Go to power down mode when sleep is executed.
	SMCR |= (1 << SM0) | (1 << SM1) | (1 << SM2) | (1 << SE);
	// Enable int0 for any change.
	EICRA = 1 << ISC00;
	EIMSK |= 1 << INT0;
	EIFR |= 1 << INTF0;
	// Set up timer.
	TCCR1A = 0x00;
	TCCR1B = 0x05;
	TCCR1C = 0x00;
	TIMSK1 = 0;
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
		__asm__ volatile ("sleep");
	}
}

void wait_for_stability (bool level) {
	for (char t = 0; t < 10; ++t)
		_delay_ms (10);
	if (level == !(PIND & (1 << 2)))
		EIFR |= 1 << INTF0;
}

void switch_on () {
	PORTB |= 1 << ON_PIN;
	state = ON;
	wait_for_stability (1);
	PORTB &= ~(1 << ON_PIN);
}

void switch_off () {
	PORTB |= 1 << OFF_PIN;
	state = OFF;
	wait_for_stability (0);
	PORTB &= ~(1 << OFF_PIN);
}

void start_timer (int seconds) {
	unsigned long long ticks = F_CPU * seconds / 1024;
	TCNT1H = 0xff - (ticks >> 8) & 0xff;
	TCNT1L = 0xff - ticks & 0xff;
	delay = ticks >> 16;
	TIFR1 = 1 << TOV1;
	TIMSK1 = 1 << TOIE1;
}

ISR (INT0_vect) {
	bool level = !(PIND & (1 << 2));
	UDR0 = 'A' + level;
	if (level) {
		switch (state) {
		case OFF:
			switch_on ();
			break;
		case COURTESY:
			state = OVERRIDE_OPTION;
			wait_for_stability (level);
			start_timer (5);
			break;
		default:
			break;
		}
	}
	else {
		switch (state) {
		case ON:
			state = COURTESY;
			wait_for_stability (level);
			start_timer (60);
			break;
		case OVERRIDE_OPTION:
			switch_off ();
			break;
		default:
			break;
		}
	}
}

ISR (TIMER1_OVF_vect) {
	UDR0 = '0' + delay;
	if (--delay < 0)
	{
		switch (state) {
		case COURTESY:
			switch_off ();
			break;
		case OVERRIDE_OPTION:
			state = ON;
			wait_for_stability (true);
			break;
		default:
			break;
		}
	}
}
