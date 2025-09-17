/*
 * _7_segment.h
 *
 * Created: 2025-09-12 오후 11:39:09
 *  Author: User
 */ 




#ifndef _7_SEGMENT_H_
#define _7_SEGMENT_H_
#include "timer_1.h"
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
#define SEGMENT_DELAY 6// 7세그먼트 한 자리 표시마다 딜레이
#define RED 12
#define GREEN 13
#define BLUE 14
#define BUZZER 15

void Init_74595(void);
void ShiftClock(void);
void LatchClock(void);
void WordDataWrite(uint16_t data);
uint16_t make_16bit_protocol (uint8_t digit_num, uint8_t num);
void init_7_segment();
void count_on_7_segment();
void count_off_7_segment();
void print_7_segment();




#endif /* _7_SEGMENT_H_ */