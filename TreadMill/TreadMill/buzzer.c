/*
 * buzzer.c
 *
 * Created: 2025-09-28 오후 12:05:05
 *  Author: sec
 */ 
#include "common.h"
#include "timer_0.h"

void buzzer_init(void){
	DDRB  |= (1 << PB2);
	TCCR1A = 0; TCCR1B = 0;
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	TCCR1A |= (1 << COM1B1);
	OCR1B = 0;
	TCCR1B |= (1 << CS11) | (1 << CS10);  // 분주비 64
}

void buzzer_stop(void){
	OCR1B = 0;
	TCCR1A &= ~(1 << COM1B1);
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
	DDRB  &= ~(1 << PB2);
	PORTB &= ~(1 << PB2);

}
