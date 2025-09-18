/*
 * 74hc595.c
 *
 * Created: 2025-09-12 오후 4:27:33
 * Author : User
 */ 
#include "common.h"
#include <util/delay.h>
#include "UART.h"
#include "timer_1.h"
#include "_7_segment.h"
#include "ADC.h"
#include "button.h"
#include "motor_dc.h"
#include "motor_step.h"

//UART_INIT(), init_7_segment() 는 main에서 해주고 count_on_7_segment()랑 idle_7_segment() 는 버튼 입력에따라 메인 루프에서 호출해주면됩니다.

int main(void) {
	UART_INIT();
	Button_Init();
	ADC_init();
	ADC_select_channel(2);
	motor_step_init(STEP_HALF_STEP);
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

		//해당 switch문에는 각 버튼 입력에따라 분기해서 수행할 동작을 넣어주세요.
		if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x
			switch(pressed) {
				case BUTTON_SPEED_UP:
					UART_printString("BUTTON_SPPED_UP is pushed\n");
					motor_dc_up();
					break;
				case BUTTON_SPEED_DOWN:
					UART_printString("BUTTON_SPPED_DOWN is pushed\n");
					motor_dc_down();
					break;
				case BUTTON_ANGLE_UP:
					UART_printString("BUTTON_ANGLE_UP is pushed\n");
					motor_step_up();
					break;
				case BUTTON_ANGLE_DOWN:
					UART_printString("BUTTON_ANGLE_DOWN is pushed\n");
					motor_step_down();
					break;
				case BUTTON_ON_OFF:
					UART_printString("BUTTON_ON_OFF is pushed\n");
					if(current_state == IDLE) {
						current_state = RUNNING; //상태를 RUNNING으로 변경
						motor_dc_init();
						motor_dc_setup();
						timer1_count_start();
					} else if (current_state == RUNNING) {
						motor_dc_stop();
						motor_step_stop();
						current_state = IDLE; // 상태를 IDLE로 변경
						count_off_7_segment();
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
			case RUNNING :

			break;
			case EMERGENCY_STOP :
			
			
			break;

				
		}
		
		
	
		
		
		

		
		
		print_7_segment();//7세그먼트 동작
		
	}
	return 1;
}

