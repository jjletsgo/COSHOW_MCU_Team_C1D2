/*
 * program_A.c
 *
 * Created: 2025-09-23 오후 4:48:14
 * Author: sec
 */ 

#include <avr/pgmspace.h>
#include "common.h"
#include "motor_dc.h"
#include "motor_step.h"
#include "timer_0.h"
#include "program_mode.h"
#include <stdbool.h>
#include <stdint.h>

#define NUM_NOTES (sizeof(lose_my_mind_note) / sizeof(lose_my_mind_note[0]))
#define FREQ_TO_ICR(freq) ((F_CPU / (64UL * (freq))) - 1)

#define TEMPO 100

#define WHOLE_MS      (240000 / TEMPO)   // 온음표
#define HALF_MS       (WHOLE_MS / 2)     // 2분음표
#define QUARTER_MS    (WHOLE_MS / 4)     // 4분음표
#define EIGHTH_MS     (WHOLE_MS / 8)     // 8분음표
#define SIXTEENTH_MS  (WHOLE_MS / 16)    // 16분음표
#define DOTTED8_MS    (EIGHTH_MS + SIXTEENTH_MS)  // 점8분음표

#define G3   196
#define A3   220
#define B3   247
#define C4   262
#define D4   294
#define E4   330
#define F4   349
#define G4   392
#define A4   440
#define B4   494
#define C5   523
#define D5   587
#define E5   659
#define F5   698

#define W   1
#define H   2
#define Q   4
#define EE  8
#define DE  9
#define S   16

timer_ms program_ms_timer;

uint16_t note_index = 0;
uint8_t current_note = 0;
MODE select_mode;
step_dir_t step_direction;
uint8_t prev_speed_level = 0;
uint8_t prev_angle_level = 0;


const uint16_t lose_my_mind_melody[] PROGMEM = {
    E4, D4, E4, G4, A4, E4, E4,
    E4, D4, E4, G4, A4, E4, D4, D4,
    E4, D4, E4, G4, A4, E4, E4,
    E4, D4, E4, G4, A4, E4, D4, D4,
    D4, D4, D4, C4, E4, G3, D4,
    D4, C4, D4, C4, E4, G3, G3, D4,
    D4, C4, B3, B3, B3, A3,
    G3, G3, A3, C4,

    D4, A3, C4, D4, A3, C4,
    D4, E4, D4, C4, D4, A3, C4,
    D4, A4, C4, D4, A3, C4,
    D4, E4, D4, C4, D4, G3, G3,
    G4, G4, A4, E4, E4, D4, D4,
    G3, G3, A4, E4, E4, D4,
    D4, E4, F4, F4, E4, B4, B4,
    A4, C5, D5,

    D5, D5, C5, D5, E5, C5, C5,
    C5, B4, A4, G4, F4,
    E4, A4, A4, A4, G4, A4, B4, G4,
    G4, C5, B4, A4, G4, F4,
    E4, A4, A4, A4, G4, A4, G4, G4,
    G4, G4, G4, B4, D5,
    B4, B4, B4, A4, B4, A4,
    B4, C5, A4, A4, C5, D5,

    D5, D5, C5, D5, E5, C5, C5,
    C5, B4, A4, G4, F4,
    E4, A4, A4, A4, G4, A4, B4, G4,
    G4, C5, B4, A4, G4, F4,
    D4, A4, A4, A4, G4, A4, G4, G4, G4,
    G4, G4, B4, B4, B4,
    B4, B4, A4, B4, A4, B4, C5,
    A4, A4, D5, C5, D5
};

const uint8_t lose_my_mind_note[] PROGMEM = {
    S,S,S,S,EE,EE,Q,S,S,S,S,DE,S,EE,EE,
    S,S,S,S,EE,EE,Q,S,S,S,S,DE,S,EE,EE,
    S,S,S,S,EE,EE,Q,S,S,S,S,DE,S,EE,EE,
    S,S,EE,EE,S,S,EE,Q,Q,Q,

    EE,EE,EE,EE,EE,EE,S,DE,EE,EE,EE,S,DE,
    EE,S,DE,EE,EE,EE,EE,EE,EE,EE,Q,EE,EE,
    EE,EE,DE,S,EE,EE,EE,EE,EE,DE,S,EE,EE,
    EE,Q,EE,EE,Q,EE,H,EE,EE,EE,

    H,DE,S,S,S,EE,Q,EE,EE,EE,EE,EE,
    EE,EE,Q,DE,S,S,S,EE,Q,EE,EE,EE,EE,EE,
    EE,EE,Q,DE,S,S,DE,Q,EE,EE,EE,EE,EE,
    EE,EE,EE,EE,S,S,EE,Q,EE,EE,EE,EE,

    H,DE,S,S,S,EE,Q,EE,EE,EE,EE,EE,
    EE,EE,Q,DE,S,S,S,EE,Q,EE,EE,EE,EE,EE,
    EE,EE,Q,DE,S,S,DE,Q,EE,EE,EE,EE,EE,
    EE,EE,EE,EE,S,S,EE,Q,EE,EE,EE,S,DE
};

const uint8_t lose_my_mind_speed[NUM_NOTES] PROGMEM = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
};
const uint8_t lose_my_mind_angle[NUM_NOTES] PROGMEM = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};


uint16_t note_to_duration(uint8_t note){
    switch(note){
        case W:  return WHOLE_MS;
        case H:  return HALF_MS;
        case Q:  return QUARTER_MS;
        case EE: return EIGHTH_MS;
        case DE: return DOTTED8_MS;
        case S:  return SIXTEENTH_MS;
        default: return QUARTER_MS;
    }
}


void program_init(void){
    DDRB  |= (1 << PB2);
    TCCR1A = 0; TCCR1B = 0;
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    TCCR1A |= (1 << COM1B1);
    OCR1B = 0;
    TCCR1B |= (1 << CS11) | (1 << CS10);  // 분주비 64
}

void program_stop(void){
    OCR1B = 0;
    TCCR1A &= ~(1 << COM1A1);
    PORTB  &= ~(1 << PB2);
}


void program_play(void){
    uint16_t melody_val = pgm_read_word(&lose_my_mind_melody[note_index]);
    uint8_t note_val    = pgm_read_byte(&lose_my_mind_note[note_index]);
    uint16_t duration   = note_to_duration(note_val);

    ICR1 = FREQ_TO_ICR(melody_val);
    OCR1B = ICR1 / 4;

    if (timer_delay_ms(&program_ms_timer, duration)){
        note_index++;
        if (note_index >= NUM_NOTES){
            note_index = 0;   
        }
    }

    speed_level = pgm_read_byte(&lose_my_mind_speed[note_index]);
    angle_level = pgm_read_byte(&lose_my_mind_angle[note_index]);

    if (speed_level != prev_speed_level){
        motor_dc_change();
    }
    if (angle_level != prev_angle_level){
        step_direction = (angle_level > prev_angle_level) ? STEP_UP : STEP_DOWN;
        motor_step_change(angle_level, step_direction);
    }
    prev_speed_level = speed_level;
    prev_angle_level = angle_level;
}
