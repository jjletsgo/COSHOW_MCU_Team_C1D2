#include <avr/io.h>
#include "dc_motor.h"

#define PWM_TOP 15999

void dc_motor_init(void)
{
	DDRB  |= (1 << PB2);                 

	TCCR1A = 0;                       
	TCCR1B = 0;

	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);

	TCCR1A |= (1 << COM1B1);

	ICR1  = PWM_TOP;                   
	OCR1B = 0;                         

	TCCR1B |= (1 << CS10);              
}

void dc_motor_start(uint8_t duty)        
{
	uint32_t pwm = (uint32_t)duty * PWM_TOP / 255U;
	if (pwm >= PWM_TOP) pwm = PWM_TOP - 1;   
	OCR1B = (uint16_t)pwm;
}

void dc_motor_stop(void)
{
	OCR1B = 0;                         
	TCCR1A &= ~(1 << COM1B1);
	PORTB  &= ~(1 << PB2);
}
