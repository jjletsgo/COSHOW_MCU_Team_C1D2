#ifndef ENCODER_H_
#define ENCODER_H_
#include "board.h"
#include <stdint.h>

#ifndef MAGNET
#define MAGNET 1U
#endif

void encoder_init(void);
void timer2_init(void);

uint32_t encoder_read(void); 
uint16_t timer2_read(void);             

#endif
