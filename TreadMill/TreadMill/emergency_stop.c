#include "common.h"
#include <avr/interrupt.h>
#include "emergency_stop.h"
#include "motor_dc.h"
#include "motor_step.h"

static volatile uint32_t hall_count = 0; 
static volatile uint16_t timer_ms   = 0; 

ISR(INT0_vect)
{
	motor_dc_stop();
	motor_step_stop();

	// 정지해야할 기능들 넣기

}

void emergency_stop_init(void)
{
	DDRD  &= ~(1 << PD2);
	PORTD |=  (1 << PD2); 

	EICRA &= ~((1 << ISC01) | (1 << ISC00));
	EICRA |=  (1 << ISC01) | (1 << ISC00);

	EIFR  |=  (1 << INTF0); 
	EIMSK |=  (1 << INT0);   
}

