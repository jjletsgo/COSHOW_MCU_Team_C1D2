/*
 * common.h
 *
 * Created: 2025-09-12 오후 11:45:12
 *  Author: User
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#include <avr/io.h> 
#include "timer_0.h"
#include <stdbool.h>
#include "buzzer.h"

uint16_t combine_uint8(uint8_t high, uint8_t low);
typedef enum {
    IDLE = 0,
    INIT = 1,
    RUNNING = 2,
    EMERGENCY_STOP = 3,
    PROGRAM_A = 4
} STATE;


extern volatile STATE previous_state;
extern volatile STATE current_state;
extern volatile STATE next_state;
extern volatile uint8_t is_INIT_done;

extern int32_t load_offset;
extern volatile uint8_t go_to_next_state;
extern volatile uint8_t rgb_mask_red_flag;

extern volatile uint8_t speed_level; // 속도 레벨(1~5)
extern volatile uint8_t angle_level; // 기울기 레벨(1~5)

// 각 버튼에 대한 레벨을 저장하기 위한 함수 부분

#define LEVEL_MIN 1 // 최소레벨 1
#define LEVEL_MAX 5 // 최대레벨 5

#define SPEED_TABLE {0, 40, 43, 46, 49, 52} // Level 값과 동일화하기 위해서 index 0은 미사용


extern const uint8_t SpeedPwm[LEVEL_MAX + 1]; //사용하고자 하는 c 코드에서 위의 SPEED_TABLE 불러와서 리스트로 정의

//level -> pwm 변환 함수
static inline uint8_t pwm_setup(uint8_t level){
	return SpeedPwm[level];
}

extern timer_ms INIT_timer;
extern volatile uint8_t INIT_counter;
#endif /* COMMON_H_ */