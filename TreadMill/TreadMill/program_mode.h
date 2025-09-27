/*
 * program_mode.h
 *
 * Created: 2025-09-23 오후 7:15:11
 *  Author: sec
 */ 


#ifndef PROGRAM_MODE_H_
#define PROGRAM_MODE_H_

typedef enum {
	LOSE_MY_MIND = 0
} MODE;

extern MODE mode;
extern uint16_t note_index;
extern uint8_t prev_speed_level;
extern uint8_t prev_angle_level;
void program_init(void);
void program_stop(void);
void select_program(MODE select_mode);
void program_play(void);




#endif /* PROGRAM_MODE_H_ */