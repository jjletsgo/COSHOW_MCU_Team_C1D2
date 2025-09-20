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
#include "_74595.h"
#include "ADC.h"
#include "button.h"


//UART_INIT(), init_7_segment() 는 main에서 해주고 count_on_7_segment()랑 idle_7_segment() 는 버튼 입력에따라 메인 루프에서 호출해주면됩니다.

int main(void) {
	timer0_init();
	UART_INIT();
	Button_Init();
	ADC_init();
	ADC_select_channel(2);
	init_74595(); //타이머0 활성화 및 74595와 연결된 atmeag328p의 핀 설정
	
	
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

		//해당 switch문에는 각 버튼 입력에따라 분기해서 수행할 동작을 넣어주세요.
		if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x
			//버튼 디버깅용 UART 송신코드
			UART_print8bitNumber(pressed);
			switch(pressed) {
				case BUTTON_SPEED_UP:UART_print8bitNumber(pressed);
					UART_printString("BUTTON_SPPED_UP is pushed\n");
					break;
				case BUTTON_SPEED_DOWN:
					UART_printString("BUTTON_SPPED_DOWN is pushed\n");
					break;
				case BUTTON_ANGLE_UP:
					UART_printString("BUTTON_ANGLE_UP is pushed\n");
					break;
				case BUTTON_ANGLE_DOWN:
				UART_printString("BUTTON_ANGLE_DOWN is pushed\n");
					break;
				case BUTTON_ON_OFF:
					UART_printString("BUTTON_ON_OFF is pushed\n");
					if(current_state == IDLE) {
						timer_reset_74595();
						current_state = INIT; //상태를 INIT으로 변경
						UART_printString("STATE CHANGED : IDLE -> INIT\n");
						
					} else if (current_state == RUNNING) {
						timer_reset_74595();
						current_state = IDLE; // 상태를 IDLE로 변경
						UART_printString("STATE CHANGED : RUNNING -> IDLE\n");
					}
				
					break;
				default:
					UART_printString("정의되지 않은 버튼 입력");
					break;
			}
		}

		//////////// FSM 상태에 따른 분기
		////해당 switch문에는 각 FSM 상태에따라 분기해서 수행할 동작을 넣어주세요.
		switch(current_state) {
			case IDLE :
			
			
				break;
			case INIT :
						
						
			break;
			case RUNNING :
				
			
			break;
			case EMERGENCY_STOP :
			
			
			break;

				
		}
		
		
		
	
		
		
		

		
		
		print_7_segment();//7세그먼트 동작, INIT 상태에서 실행되면 3초후 RUNNING으로 상태 변경
		
	}
	return 1;
}

