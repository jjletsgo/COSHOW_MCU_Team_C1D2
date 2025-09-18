#ifndef HALL_SENSOR_H_
#define HALL_SENSOR_H_

#include <stdint.h>
#include <avr/io.h>

#ifndef MAGNET
#define MAGNET 1U
#endif

void hall_init(void);
void timer_init(void);

uint32_t hall_read(void); 
uint16_t timer_read(void);             

#endif
