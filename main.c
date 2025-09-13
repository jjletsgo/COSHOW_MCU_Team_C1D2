/*
 * 74hc595.c
 *
 * Created: 2025-09-12 오후 4:27:33
 * Author : User
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "common.h"
#include "UART.h"
#include "timer_0.h"
#include "_7_segment.h"





int main(void) {
	timer0_init();//timer0 활성화 -> 이때부터 타이버 오버플로 인터럽트 호출됨
	Init_74595();
	UART_INIT();
	timer0_millis = 0;
	timer0_micros = 0;
	int min=0;
	uint8_t num_digits[4];
	unsigned long present_time=0;
	unsigned long previous_time=millis();
	

	
	int i = 0;
	while(1) {
		
		present_time = millis();
		
		////1초마다 메시지 보내는 디버깅용 uart 송신코드
		/*
		if(present_time-previous_time >= 1000) {
			previous_time = present_time;
			UART_printString("1sec_passed\n");
		}
		*/
		
		if(present_time-previous_time >= 1000) //60000이맞는데 디버깅하려고 10000 으로설정	
		{ //60초 경과시
			previous_time = present_time;
			min++;
	
			num_digits[0] = min / 60 / 10; //시간의 십의 자리
			num_digits[1] = min / 60 % 10; // 시간의 일의 자리
			num_digits[2] = (min % 60) / 10; //분의 십의 자리
			num_digits[3] = (min % 60) % 10; //분의 일의 자리
			//디버깅용
			UART_printString("10sec_passed\n");
		}	
	
		
		for(i=0; i<4; i++) {
		WordDataWrite(make_16bit_protocol(i,num_digits[i])); //74HC595로 16비트 데이터 전송(SER,SCK 16번 반복후 RCK도 쏴준다. 즉 7-segment에 병렬로 출력까지한다)
		_delay_ms(SEGMENT_DELAY);
		}
		
		


	}
	return 1;
}

