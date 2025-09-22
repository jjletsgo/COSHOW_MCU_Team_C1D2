#ifndef EMERGENCY_STOP_H_
#define EMERGENCY_STOP_H_

#include <stdint.h>
#include "common.h"

void emergency_stop_init(void);
bool load_cell_emergency(void);
extern bool emergency_trigger;
#endif
