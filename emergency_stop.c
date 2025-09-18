#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "emergency_stop.h"

static volatile uint32_t hall_count = 0; 
static volatile uint16_t timer_ms   = 0; 

ISR(INT0_vect)
{
	hall_count++;
}

ISR(TIMER2_COMPA_vect)
{
	if (++timer_ms >= 1000) {
		timer_ms = 0; 
	}
}

void hall_init(void)
{
	DDRD  &= ~(1 << PD2);
	PORTD |=  (1 << PD2); 

	EICRA &= ~((1 << ISC01) | (1 << ISC00));
	EICRA |=  (1 << ISC01) | (1 << ISC00);

	EIFR  |=  (1 << INTF0); 
	EIMSK |=  (1 << INT0);   
}

void timer_init(void)
{
	TCCR2A = (1 << WGM21);    
	TCCR2B = 0;
	TCCR2B |= (1 << CS22);   
	
	OCR2A  = 249;             
	TCNT2  = 0;
	TIFR2 |= (1 << OCF2A);    
	TIMSK2 |= (1 << OCIE2A);  
}

uint32_t hall_read(void)
{
	uint8_t s = SREG; cli();
	uint32_t value = hall_count;
	hall_count = 0;       
	SREG = s;
	return value;
}

uint16_t timer_read(void)
{
	uint8_t s = SREG; cli();
	uint16_t value = timer_ms; 
	SREG = s;
	return value;
}
