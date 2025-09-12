/*
 * _74hc595_d1c2.h
 *
 * Created: 2025-09-12 오후 10:01:39
 *  Author: User
 */ 


#ifndef _74hc595_d1c2_H_
#define _74hc595_d1c2_H_
#include <avr/io.h>
void Init_74595(void);
void ShiftClock(void);
void LatchClock(void);
void WordDataWrite(uint16_t data);

#endif /* _74hc595_d1c2_H_ */