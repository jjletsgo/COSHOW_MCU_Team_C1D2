#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include <stdint.h>

void dc_motor_init(void);         
void dc_motor_start(uint8_t duty); 
void dc_motor_stop(void);         
void dc_motor_control(void);

#endif
