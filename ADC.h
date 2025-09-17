/*
 * ADC.h
 *
 * Created: 2025-09-15 오후 4:50:56
 *  Author: User
 */ 

#ifndef F_CPU
#define F_CPU 16000000L
#endif

#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART.h"
void ADC_init(void);
void ADC_select_channel(unsigned char channel);
int read_ADC(void);


#endif /* ADC_H_ */