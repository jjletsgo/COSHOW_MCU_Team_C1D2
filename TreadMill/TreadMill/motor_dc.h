#ifndef MOTOR_DC_H_
#define MOTOR_DC_H_

#include <stdint.h>



void motor_dc_init(void);
void motor_dc_start(uint8_t duty);
void motor_dc_stop(void);
void motor_dc_control(Button_t pressed);
void motor_setup(void);

#endif
