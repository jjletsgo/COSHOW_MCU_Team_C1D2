#ifndef STEPPER_H
#define STEPPER_H

#include <avr/io.h>
#include <stdint.h>

typedef enum {
	STEPPER_FULL_STEP = 0,   // 4상 Full-step (토크↑, 속도↓)
	STEPPER_HALF_STEP = 1    // 8상 Half-step (분해능↑, 진동↓)
} stepper_mode_t;

typedef enum {
	STEPPER_CW  = 0,         // 시계방향
	STEPPER_CCW = 1          // 반시계방향
} stepper_dir_t;

void stepper_init(stepper_mode_t mode);
void stepper_set_mode(stepper_mode_t mode);
void stepper_set_speed_rpm(uint16_t rpm);   // 목표 RPM(출력축 기준, 28BYJ-48은 기어비 때문에 매우 낮게)
void stepper_step(int32_t steps, stepper_dir_t dir); // 지정 스텝만큼 이동(블로킹)
void stepper_release(void);                  // 코일 OFF(발열/소음↓, 홀딩토크 X)

#endif
