#ifndef STATE_H
#define STATE_H

#include <stdint.h>

typedef enum {
	IDLE = 0,
	RUN,
	STOP
} state_id_t;

typedef struct {
	state_id_t current;
	uint16_t   rpm;
	uint8_t    emergency;
} system_state_t;

extern volatile system_state_t g_state;

/* 4) API 프로토타입 */
void state_set(state_id_t s);
state_id_t state_get(void);

#endif
