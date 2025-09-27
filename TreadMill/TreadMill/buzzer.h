/*
 * buzzer.h
 *
 * Created: 2025-09-28 오후 12:05:14
 *  Author: sec
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_
#define FREQ_TO_ICR(freq) ((F_CPU / (64UL * (freq))) - 1)
#include "common.h"
#include "timer_0.h"

void buzzer_init(void);
void buzzer_stop(void);
void set_buzzer_duration(void);
void buzzer_work(void);


#endif /* BUZZER_H_ */