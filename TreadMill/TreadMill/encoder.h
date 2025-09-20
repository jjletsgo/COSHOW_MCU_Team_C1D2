#ifndef ENCODER_H_
#define ENCODER_H_
#include "common.h"
#include <stdint.h>

#ifndef MAGNET
#define MAGNET 1U
#endif

void encoder_init(void);
uint8_t encoder_read(void);

#endif
