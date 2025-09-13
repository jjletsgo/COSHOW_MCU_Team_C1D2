/*
 * _7_segment.h
 *
 * Created: 2025-09-12 오후 11:39:09
 *  Author: User
 */ 




#ifndef _7_SEGMENT_H_
#define _7_SEGMENT_H_

#include <avr/io.h>
#include "common.h"
/*
현재 Atmega328p의 PC4,5,6이 SER,SCK,RCK에 연결되어있음
*/
#define SER 4
#define SCK 5
#define RCK 6

void Init_74595(void);
void ShiftClock(void);
void LatchClock(void);
void WordDataWrite(uint16_t data);
uint16_t make_12bit_protocol (uint8_t digit_num, uint8_t num);



#endif /* _7_SEGMENT_H_ */