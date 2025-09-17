#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "Step_motor.h"

void step_motor_control(void)
{
	step_init(STEP_HALF_STEP);   
	step_set_speed_rpm(10);         

	while (1) {
		step_step(1024, STEP_CW);  
		_delay_ms(500);
		step_step(1024, STEP_CCW);  
		_delay_ms(500);
	}
}
