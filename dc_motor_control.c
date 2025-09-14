#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "dc_motor.h"

void dc_motor_control(uint8_t duty)
{
	dc_motor_init();

	dc_motor_start(duty);
	_delay_ms(5000);

	dc_motor_stop();
}
