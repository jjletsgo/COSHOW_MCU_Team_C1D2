/*
 * _74hc595.h
 *
 * Created: 2025-09-12 오후 11:39:09
 *  Author: User
 */ 




#ifndef _74HC595_H_
#define _74HC595_H_
#include "timer_0.h"
#include <avr/io.h> 
#include "common.h"
#include "UART.h"
#include <util/delay.h>
/*
현재 Atmega328p의 PC4,5,6이 SER,SCK,RCK에 연결되어있음
*/
#define SER 0
#define SCK 1
#define RCK 0
#define SEGMENT_DELAY 5// 7세그먼트 한 자리 표시마다 딜레이
#define LV_1_DELAY 1000
#define LV_2_DELAY 800
#define LV_3_DELAY 600
#define LV_4_DELAY 500
#define LV_5_DELAY 400

// 500 280 140 70 30



void init_74595(void);
void timer_reset_74595(void);
void ShiftClock(void);
void LatchClock(void);
void WordDataWrite(uint64_t data);
uint16_t make_16bit_protocol (uint8_t digit_num, uint8_t num);
void print_7_segment();


extern uint8_t _3sec_counter;

#endif /* _74595_H_ */