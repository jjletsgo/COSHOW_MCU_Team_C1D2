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

void program_init(void);
void program_stop(void);
void select_program(MODE select_mode);
void program_play(void);




#endif /* PROGRAM_MODE_H_ */