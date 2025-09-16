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
#include "ADC.h"
#include "button.h"


//UART_INIT(), init_7_segment() 는 main에서 해주고 count_on_7_segment()랑 idle_7_segment() 는 버튼 입력에따라 메인 루프에서 호출해주면됩니다.

int main(void) {
	UART_INIT();
	Button_Init();
	ADC_init();
	ADC_select_channel(2);
	init_7_segment(); //타이머0 활성화 및 74595와 연결된 atmeag328p의 핀 설정
	//count_off_7_segment(); //타이머1 비활성화 -> 0만 출력.
	//count_on_7_segment(); //타이머1 오버플로 인터럽트 활성화 및 previous time을 millis()로 설정 및 cnt를 1로 설정
	
	
	while(1) {
		//UART_print16bitNumber(Button_ADC_getDebounced(read_ADC()));
		//UART_print16bitNumber(Button_ADC_getPressed(read_ADC()));
		//UART_printString("\n");
		
		//////////// 눌려진 버튼에 따른 분기
		uint16_t adc_value = read_ADC();
		//UART_print16bitNumber(adc_value);
		//UART_printString("\n");
		//_delay_ms(200);
		Button_t pressed = Button_ADC_getPressed(adc_value); //폴링으로 눌린 버튼을 저장
		//UART_print8bitNumber(pressed);
		if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x
			switch(pressed) {
				case BUTTON_SPEED_UP:UART_print8bitNumber(pressed);
					UART_printString("Button 1 is pushed\n");
					break;
				case BUTTON_SPEED_DOWN:
					UART_printString("Button 2 is pushed\n");
					break;
				case BUTTON_ANGLE_UP:
					UART_printString("Button 3 is pushed\n");
					break;
				case BUTTON_ANGLE_DOWN:
					UART_printString("Button 4 is pushed\n");
					break;
				case BUTTON_ON_OFF:
					UART_printString("Button 5 is pushed\n");
					if(current_state == IDLE) {
						current_state = RUNNING;
						timer1_count_start();
					} else if (current_state == RUNNING) {
						current_state = IDLE;
						count_off_7_segment();
					}
				
					break;
				default:
					UART_printString("정의되지 않은 버튼 입력");
					break;
			}
		}

		//////////// FSM 상태에 따른 분기
		switch(current_state) {
			case IDLE :
			
			
				break;
			case RUNNING :
				
			
			break;
			case EMERGENCY_STOP :
			
			
			break;

				
		}
		
		
	
		
		
		

		
		
		print_7_segment();//7세그먼트 동작
		
	}
	return 1;
}

