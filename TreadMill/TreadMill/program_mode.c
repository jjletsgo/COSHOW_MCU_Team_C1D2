/*
 * program_A.c
 *
 * Created: 2025-09-23 오후 4:48:14
 *  Author: sec
 */ 

/*
#include "common.h"
#include "motor_dc.h"
#include "motor_step.h"
#include "timer_0.h"
#include "program_mode.h"
#include <stdbool.h>
#include <stdint.h>

#define FREQ_TO_ICR(freq) ((F_CPU / (64 * (freq))) - 1)

#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523

#define WHOLE_NOTE      1
#define HALF_NOTE       2
#define QUATER_NOTE     4
#define EIGHTH_NOTE     8
#define SIXTEENTH_NOTE  16

timer_s program_s_timer;
timer_ms program_ms_timer;

uint16_t scale[] = {C4, D4, E4, F4, G4, A4, B4, C5};
uint16_t *melody;
uint8_t *note_value;
uint8_t index = 0;
uint8_t current_note = 0;
uint8_t need_quater_count = 0;
MODE select_mode;

uint16_t lose_my_mind_melody[] = {};
uint8_t lose_my_mind_note[] = {};
uint8_t lose_my_mind_speed[] = {};
uint8_t lose_my_mind_angle[] = {};
	
void program_init(void){
	DDRB  |= (1 << PB2);
	TCCR1A = 0; TCCR1B = 0;
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	TCCR1A |= (1 << COM1B1);
	OCR1B = 0;
	TCCR1B |= (1 << CS11) | (1 << CS10);
}

void program_stop(void){
	OCR1B = 0;
	TCCR1A &= ~(1 << COM1A1);
	PORTB  &= ~(1 << PB2);
}

void select_program(MODE mode){
	select_mode = mode;
	switch(select_mode){
		case LOSE_MY_MIND:
			melody = lose_my_mind_melody;
			note_value = lose_my_mind_note;
			break;
		default:
		break;
	}
	
}


void program_play(void){
	current_note = note_value[index];
	
	switch(current_note){
		case WHOLE_NOTE:
			if (timer_delay_s(&program_s_timer, 2)) index++;
			break;
			
		case HALF_NOTE:
			if (timer_delay_s(&program_s_timer, 1)) index++;
			break;
			
		case QUATER_NOTE:
			
			if (timer_delay_ms(&program_ms_timer, 250)) index++;
			if (timer_delay_ms(&program_ms_timer, 250)) index++;
	
			break;
			
		case EIGHTH_NOTE:
			if (timer_delay_ms(&program_ms_timer, 250)) index++;
			break;
			
		case SIXTEENTH_NOTE:
			if (timer_delay_ms(&program_ms_timer, 125)) index++;
			break;
	}

	ICR1 = FREQ_TO_ICR(melody[index]);
	OCR1B = ICR1 / 2;
}


*/