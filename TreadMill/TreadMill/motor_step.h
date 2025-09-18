#ifndef STEPPER_H
#define STEPPER_H

#include <avr/io.h>
#include <stdint.h>

typedef enum {
	STEP_FULL_STEP = 0,   // 4상 Full-step (토크↑, 속도↓)
	STEP_HALF_STEP = 1    // 8상 Half-step (분해능↑, 진동↓)
} step_mode_t;

typedef enum {
	STEP_UP  = 0,          
	STEP_DOWN = 1          
} step_dir_t;

void motor_step_init(step_mode_t mode);
void step_set_mode(step_mode_t mode);
void step_set_speed_rpm(uint16_t rpm);   
void motor_step_change(uint8_t level, step_dir_t dir);
void step_release(void);              
void motor_step_up(void);
void motor_step_down(void);
void motor_step_stop(void);
#endif
