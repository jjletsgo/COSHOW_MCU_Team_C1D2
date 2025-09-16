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
#include "timer_1.h"
#include "_7_segment.h"



//UART_INIT(), init_7_segment() 는 main에서 해주고 count_on_7_segment()랑 idle_7_segment() 는 버튼 입력에따라 메인 루프에서 호출해주면됩니다.

int main(void) {
	UART_INIT();
	init_7_segment(); //타이머0 활성화 및 74595와 연결된 atmeag328p의 핀 설정
	idle_7_segment(); //타이머0 비활성화 -> 0만 출력.
	count_on_7_segment(); //타이머1 오버플로 인터럽트 활성화 및 previous time을 millis()로 설정 및 cnt를 1로 설정
	
	while(1) {
		work_7_segment();//7세그먼트 동작
		
	}
	return 1;
}

