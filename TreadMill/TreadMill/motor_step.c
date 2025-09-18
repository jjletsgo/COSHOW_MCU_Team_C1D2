
#include "common.h"
#include <util/delay.h>
#include "motor_step.h"
#include "button.h"
#include <stdbool.h>
#define IN4 PD4
#define IN3 PD5
#define IN2 PD6
#define IN1 PD7
#define COIL_MASK ((1<<IN1)|(1<<IN2)|(1<<IN3)|(1<<IN4))
#define STEP_ANGLE 128

static volatile step_mode_t g_mode = STEP_HALF_STEP;
static volatile uint16_t g_delay_us = 2000;

volatile uint8_t angle_level = 0;
volatile uint8_t value = 0;
volatile int32_t steps = 0;
volatile bool turn_off = false;

static const uint8_t FULL_SEQ[4] = {
	(1<<IN1),
	(1<<IN2),
	(1<<IN3),
	(1<<IN4)
};

static const uint8_t HALF_SEQ[8] = {
	(1<<IN1),
	(1<<IN1)|(1<<IN2),
	(1<<IN2),
	(1<<IN2)|(1<<IN3),
	(1<<IN3),
	(1<<IN3)|(1<<IN4),
	(1<<IN4),
	(1<<IN4)|(1<<IN1)
};

static inline void coils_write(uint8_t mask)
{
	PORTD = (PORTD & ~COIL_MASK) | (mask & COIL_MASK);
}

void motor_step_init(step_mode_t mode)
{
	DDRD |= COIL_MASK;
	g_mode = mode;
	step_release();
}

void step_set_mode(step_mode_t mode)
{
	g_mode = mode;
}

void step_set_speed_rpm(uint16_t rpm)
{
	if (rpm == 0) { g_delay_us = 0xFFFF; return; }

	uint32_t spr = (g_mode == STEP_HALF_STEP) ? 4096UL : 2048UL;
	uint32_t interval_us = 60000000UL / ((uint32_t)rpm * spr);

	if (interval_us < 800UL)     interval_us = 800UL;
	if (interval_us > 50000UL)   interval_us = 50000UL;

	g_delay_us = (uint16_t)interval_us;
}

void motor_step_change(uint8_t level, step_dir_t dir)
{
	if (turn_off == true){
		steps = level * STEP_ANGLE;
	}
	else if (1 <= level && level <= 5){
		steps = STEP_ANGLE;
	}
	else {
		steps = 0;
		return;
	}


	
	const uint8_t *seq;
	uint8_t seq_len;

	if (g_mode == STEP_HALF_STEP) {
		seq = HALF_SEQ; seq_len = 8;
	}
	else {
		seq = FULL_SEQ; seq_len = 4;
	}

	uint32_t count = (steps > 0) ? (uint32_t)steps : (uint32_t)(-steps);
	int8_t idx = 0;

	while (count--) {
		if (dir == STEP_UP) {
			idx++; if (idx >= (int8_t)seq_len) idx = 0;
		}
		else {
			idx--; if (idx < 0) idx = (int8_t)seq_len - 1;
		}

		coils_write(seq[(uint8_t)idx]);

		uint16_t remain = g_delay_us;

		while (remain >= 1000) {
			_delay_ms(1);
			remain -= 1000;
		}
		while (remain--) {
			_delay_us(1);
		}
	}
}

void step_release(void){
	coils_write(0);
}

void motor_step_stop(void){
	turn_off = true;
	motor_step_change(angle_level, STEP_DOWN);
	turn_off = false;
}

void motor_step_up(void){
	step_set_speed_rpm(10);
	if (angle_level < LEVEL_MAX)
	angle_level++;
	motor_step_change(angle_level, STEP_UP);
}

void motor_step_down(void){
	step_set_speed_rpm(10);
	if (angle_level > LEVEL_MIN)
	angle_level--;
	motor_step_change(angle_level, STEP_DOWN);

}
