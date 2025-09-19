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

uint16_t combine_uint8(uint8_t high, uint8_t low);
typedef enum {
    IDLE = 0,
    RUNNING = 1,
    EMERGENCY_STOP = 2
} STATE;

extern STATE current_state;

#endif /* COMMON_H_ */