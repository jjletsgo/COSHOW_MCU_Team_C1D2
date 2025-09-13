#define F_CPU 16000000L
#include "dc_motor.h"
#define PWM_TOP 799


void dc_motor_init(void)
{
	DDRB |= 0x04;

	TCCR1A = 0; TCCR1B = 0;
	
	TCCR1A = (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12);

	TCCR1A |= (1 << COM1B1);

	ICR1 = PWM_TOP;

	TCCR1B |= (1 << CS10);

	OCR1B = 0;
}

void dc_motor_start(uint8_t duty)
{
	uint32_t val = ((uint32_t)duty * PWM_TOP) / 255;
	
	if (val >= PWM_TOP){
		val = PWM_TOP - 1;
	}
	
	OCR1B = (uint16_t)val;

	TCCR1A |= (1 << COM1B1);
}

void dc_motor_stop(void)
{
	OCR1B = 0;               
	TCCR1A &= ~(1 << COM1B1);  
	PORTB  &= ~0x04;
}