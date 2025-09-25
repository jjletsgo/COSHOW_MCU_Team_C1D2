/*
 * common.c
 *
 * Created: 2025-09-12 오후 11:45:22
 *  Author: User
 */ 

#include "common.h"
volatile STATE next_state = IDLE;
volatile STATE previous_state = IDLE;
volatile STATE current_state = IDLE;  
volatile uint8_t go_to_next_state = 0;
volatile uint8_t is_INIT_done = 0;
volatile uint8_t rgb_mask_red_flag = 0;
volatile uint8_t speed_level = 1; // 속도 레벨(1~5)
volatile uint8_t angle_level = 1; // 기울기 레벨(1~5)

volatile bool turn_off = false; 

// 8비트 두 개를 합쳐 16비트로 만드는 함수
uint16_t combine_uint8(uint8_t high, uint8_t low) {
    return ((uint16_t)high << 8) | low;
}

