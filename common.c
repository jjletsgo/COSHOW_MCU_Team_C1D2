/*
 * common.c
 *
 * Created: 2025-09-12 오후 11:45:22
 *  Author: User
 */ 

#include "common.h"

// 8비트 두 개를 합쳐 16비트로 만드는 함수
uint16_t combine_uint8(uint8_t high, uint8_t low) {
    return ((uint16_t)high << 8) | low;
}
