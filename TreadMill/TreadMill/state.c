#include <avr/interrupt.h>
#include "state.h"

volatile system_state_t g_state = {
	.current   = IDLE,
};

void state_set(state_id_t state){
	uint8_t sreg = SREG; 
	cli();
	g_state.current = state;
	SREG = sreg;
}

state_id_t state_get(void){
	uint8_t sreg = SREG; 
	cli();
	state_id_t state = g_state.current;
	SREG = sreg;
	return state;
}
