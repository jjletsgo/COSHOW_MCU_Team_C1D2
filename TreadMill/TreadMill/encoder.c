#include "common.h"
#include <avr/interrupt.h>
#include "encoder.h"
#include "timer_0.h"
static volatile uint8_t hall_count = 0;

ISR(INT1_vect)
{
	hall_count++;
}

void encoder_init(void)
{
	DDRD  &= ~(1 << PD3);
	PORTD |=  (1 << PD3);

	EICRA &= ~((1 << ISC11) | (1 << ISC10));
	EICRA |=  (1 << ISC11);

	EIFR  |=  (1 << INTF1);
	EIMSK |=  (1 << INT1);
}

uint8_t encoder_read(void)
{
	uint8_t s = SREG; cli();
	uint8_t value = hall_count;
	hall_count = 0;
	SREG = s;
	return value;
}

