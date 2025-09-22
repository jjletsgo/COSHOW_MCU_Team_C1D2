#include "common.h"
#include "motor_step.h"
#include "button.h"
#include <stdbool.h>
#include "timer_0.h"

#define IN4 PD4
#define IN3 PD5
#define IN2 PD6
#define IN1 PD7
#define COIL_MASK ((1<<IN1)|(1<<IN2)|(1<<IN3)|(1<<IN4))
#define STEP_ANGLE 128
#include "UART.h"

// 스텝모터 상태 머신을 위한 열거형
typedef enum {
	STEP_IDLE = 0,
	STEP_RUNNING,
	STEP_STOPPING
} step_state_t;

// 전역 변수들
static volatile step_mode_t g_mode = STEP_HALF_STEP;
static volatile uint16_t g_delay_ms = 2;  // ms 단위로 변경
static volatile step_state_t g_state = STEP_IDLE;
static volatile step_dir_t g_direction = STEP_UP;
static volatile uint32_t g_remaining_steps = 0;
static volatile int8_t g_current_seq_idx = 0;

timer_ms step_timer = {0, 0};  // 타이머 구조체 초기화
volatile uint8_t angle_level = 1;
volatile uint8_t value = 0;
volatile int32_t steps = 0;
volatile bool turn_off = false;

// 시퀀스 테이블
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

// 코일 출력 함수
static inline void coils_write(uint8_t mask)
{
	PORTD = (PORTD & ~COIL_MASK) | (mask & COIL_MASK);
}

// 한 스텝 실행하는 함수
static void execute_single_step(void)
{
	const uint8_t *seq;
	uint8_t seq_len;
	
	if (g_mode == STEP_HALF_STEP) {
		seq = HALF_SEQ;
		seq_len = 8;
		} else {
		seq = FULL_SEQ;
		seq_len = 4;
	}
	
	// 방향에 따라 시퀀스 인덱스 조정
	if (g_direction == STEP_UP) {
		g_current_seq_idx++;
		if (g_current_seq_idx >= (int8_t)seq_len)
		g_current_seq_idx = 0;
		} else {
		g_current_seq_idx--;
		if (g_current_seq_idx < 0)
		g_current_seq_idx = (int8_t)seq_len - 1;
	}
	
	// 실제 코일 출력
	coils_write(seq[(uint8_t)g_current_seq_idx]);
	
	// 남은 스텝 감소
	if (g_remaining_steps > 0) {
		g_remaining_steps--;
		if (g_remaining_steps == 0) {
			g_state = STEP_IDLE;
		}
	}
}

// 초기화 함수
void motor_step_init(step_mode_t mode)
{
	DDRD |= COIL_MASK;
	g_mode = mode;
	g_state = STEP_IDLE;
	g_current_seq_idx = 0;
	step_release();
	angle_level=1;
}

// 스텝모터 업데이트 함수 (메인루프에서 호출)
void motor_step_update(void)
{
	// 스텝모터가 동작 중이고 타이머 딜레이가 완료되었을 때
	if (g_state == STEP_RUNNING && timer_delay_ms(&step_timer, g_delay_ms)) {
		execute_single_step();
	}
}

// 모드 설정
void step_set_mode(step_mode_t mode)
{
	g_mode = mode;
}

// RPM을 ms 딜레이로 변환하여 설정
void step_set_speed_rpm(uint16_t rpm)
{
	if (rpm == 0) {
		g_delay_ms = 255;
		return;
	}
	
	uint32_t spr = (g_mode == STEP_HALF_STEP) ? 4096UL : 2048UL;
	uint32_t interval_us = 60000000UL / ((uint32_t)rpm * spr);
	
	// 마이크로초를 밀리초로 변환 (최소 1ms)
	uint32_t interval_ms = interval_us / 1000;
	if (interval_ms == 0) interval_ms = 1;
	if (interval_ms > 255) interval_ms = 255;
	
	g_delay_ms = (uint8_t)interval_ms;
}

// 스텝모터 동작 시작 함수
void motor_step_change(uint8_t level, step_dir_t dir)
{
	uint32_t target_steps = 0;
	
	if (turn_off == true) {
		if (level == 1) return;
		else {
			target_steps = (level-1) * STEP_ANGLE;
		}
	}
	else if (1 <= level && level <= 5) {
		target_steps = STEP_ANGLE;
	}
	else {
		angle_level = clamp_level(level);
		return;
	}
	
	// 스텝모터 파라미터 설정
	g_direction = dir;
	g_remaining_steps = target_steps;
	g_state = STEP_RUNNING;
	
	// 타이머 초기화
	step_timer.is_init_done = 0;
}

// 코일 해제
void step_release(void)
{
	coils_write(0);
	g_state = STEP_IDLE;
}

// 스텝모터 정지
void motor_step_stop(void)
{
	turn_off = true;
	motor_step_change(angle_level, STEP_DOWN);
	turn_off = false;
	angle_level = 1;
}

// 스텝모터 위로
void motor_step_up(void)
{
	if (g_state == STEP_IDLE) {  // 정지 상태일 때만 실행
		step_set_speed_rpm(10);
		angle_level++;
		motor_step_change(angle_level, STEP_UP);
	}
}

// 스텝모터 아래로
void motor_step_down(void)
{
	if (g_state == STEP_IDLE) {  // 정지 상태일 때만 실행
		step_set_speed_rpm(10);
		angle_level--;
		motor_step_change(angle_level, STEP_DOWN);
	}
}

// 스텝모터가 동작 중인지 확인하는 함수
bool motor_step_is_running(void)
{
	return (g_state == STEP_RUNNING);
}
