/*
 * load_cell.c
 *
 * Created: 2025-09-15 오후 8:07:02
 * Author : AFHGF
 */ 

#define F_CPU 16000000UL

#include <util/delay.h>
#include <stdio.h>
#include "load_cell.h"

// USART0
static void USART0_init(unsigned int ubrr);
static void USART0_transmit(char data);
static void USART0_transmit_string(const char *str);
static void USART0_transmit_number(long num);
static void USART0_transmit_float(float f, unsigned char decimal_places);

int32_t load_offset = 0;

int main(void){
	load_cell_init();
	
	USART0_init(103);
	USART0_transmit_string("HX711 Scale Test\r\n");
	_delay_ms(500);

	load_offset = load_cell_read();		// offset 구하기
	
	USART0_transmit_string("offset: ");
	USART0_transmit_number(load_offset);
	USART0_transmit_string("\r\n");

	while(1){
		long raw_value = load_cell_read();
		long net_value = raw_value - load_offset;
		float weight = load_cell_convert(net_value);

		USART0_transmit_string("Raw: ");
		USART0_transmit_number(net_value);
		USART0_transmit_string("  Weight: ");
		USART0_transmit_float(weight, 2);
		USART0_transmit_string("\r\n");

		_delay_ms(500);
	}
	return 0;
}

void USART0_init(unsigned int ubrr){
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<TXEN0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void USART0_transmit(char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void USART0_transmit_string(const char *str){
	while(*str){
		USART0_transmit(*str++);
	}
}

void USART0_transmit_number(long num){
	char buffer[12];
	int i = 0, j;
	unsigned long n;
	if(num < 0){
		USART0_transmit('-');
		n = (unsigned long)(-num);
		} else {
		n = (unsigned long)num;
	}
	do {
		buffer[i++] = (n % 10) + '0';
		n /= 10;
	} while(n);
	for(j = i-1; j >= 0; j--){
		USART0_transmit(buffer[j]);
	}
}

void USART0_transmit_float(float f, unsigned char decimal_places){
	long int_part = (long)f;
	float frac_part = f - int_part;
	if(f < 0){
		USART0_transmit('-');
		int_part = -int_part;
		frac_part = -frac_part;
	}
	USART0_transmit_number(int_part);
	USART0_transmit('.');
	for(unsigned char i=0; i<decimal_places; i++){
		frac_part *= 10;
	}
	long frac_int = (long)(frac_part + 0.5);
	USART0_transmit_number(frac_int);
}
