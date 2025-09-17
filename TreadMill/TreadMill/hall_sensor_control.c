#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include "Hall_Sensor.h"

int main(void)
{
	hall_init();
	timer_init();
	sei();  

	while (1);
	
}
