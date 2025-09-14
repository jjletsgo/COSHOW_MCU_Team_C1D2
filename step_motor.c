#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "step_motor.h"

// ---- 핀 매핑: PD4~PD7 ----
#define IN1 PD4
#define IN2 PD5
#define IN3 PD6
#define IN4 PD7
#define COIL_MASK ((1<<IN1)|(1<<IN2)|(1<<IN3)|(1<<IN4))

static volatile stepper_mode_t g_mode = STEPPER_HALF_STEP;
static volatile uint16_t g_delay_us = 2000;  

static const uint8_t FULL_SEQ[4] = {
	(1<<IN1),
	(1<<IN2),
	(1<<IN3),
	(1<<IN4)
};

// 8상 Half-step 시퀀스
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
	// D포트 상위 4비트만 갱신 (PD4~PD7)
	PORTD = (PORTD & ~COIL_MASK) | (mask & COIL_MASK);
}

void stepper_init(stepper_mode_t mode)
{
	// PD4~PD7 출력으로
	DDRD |= COIL_MASK;
	g_mode = mode;
	stepper_release();
}

void stepper_set_mode(stepper_mode_t mode)
{
	g_mode = mode;
}

// 단순 계산: interval(us) = 60e6 / (rpm * steps_per_rev)
void stepper_set_speed_rpm(uint16_t rpm)
{
	if (rpm == 0) { g_delay_us = 0xFFFF; return; }

	uint32_t spr = (g_mode == STEPPER_HALF_STEP) ? 4096UL : 2048UL; // 28BYJ-48 기준
	uint32_t interval_us = 60000000UL / ((uint32_t)rpm * spr);

	// 너무 빠르거나 느린 값 클램프
	if (interval_us < 800UL)     interval_us = 800UL;     // 전류 상승/토크 확보
	if (interval_us > 50000UL)   interval_us = 50000UL;

	g_delay_us = (uint16_t)interval_us;
}

void stepper_step(int32_t steps, stepper_dir_t dir)
{
	if (steps == 0) return;

	const uint8_t *seq;
	uint8_t seq_len;

	if (g_mode == STEPPER_HALF_STEP) {
		seq = HALF_SEQ; seq_len = 8;
		} else {
		seq = FULL_SEQ; seq_len = 4;
	}

	uint32_t count = (steps > 0) ? (uint32_t)steps : (uint32_t)(-steps);
	int8_t idx = 0;

	while (count--) {
		// 방향에 따라 인덱스 증감
		if (dir == STEPPER_CW) {
			idx++; if (idx >= (int8_t)seq_len) idx = 0;
			} else {
			idx--; if (idx < 0) idx = (int8_t)seq_len - 1;
		}

		coils_write(seq[(uint8_t)idx]);

		// --- 변수 인자 금지: 상수 기반 루프 지연 ---
		uint16_t remain = g_delay_us;

		while (remain >= 1000) {   // 1 ms 단위
			_delay_ms(1);          // 상수 인자
			remain -= 1000;
		}
		while (remain--) {
			_delay_us(1);
		}
	}
}

void stepper_release(void)
{
	coils_write(0);  
}
