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
	UART_INIT();
	init_7_segment(); //타이머0 활성화 및 74595와 연결된 atmeag328p의 핀 설정
	count_on_7_segment(); //타이머0 오버플로 인터럽트 활성화 및 previous time을 millis()로 설정 및 cnt를 1로 설정
	
	while(1) {
		
		work_7_segment();//7세그먼트 동작
		
		
	}
	
	
	
	return 1;
}

