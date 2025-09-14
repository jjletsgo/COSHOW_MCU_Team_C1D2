#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "dc_motor.h"

void dc_motor_control(void){
	dc_motor_init();
	while(1){
		
		for(uint8_t d=0; d<=40; d++){ 
			dc_motor_start(d); 
			_delay_ms(30); 
		}
		_delay_ms(7000);
		dc_motor_stop();
	}

}
