/*
 * ADC_test.c
 *
 * Created: 2025-09-08 오후 4:21:46
 * Author : User
 */ 

#include <avr/io.h>
#define F_CPU 16000000L
#include <util/delay.h>
#include <stdio.h>
#include "UART.h"
#include "ADC.h"
#include "button.h"


uint16_t ADC_value;
Button_t button;




int main(void) {
	Button_Init();
	UART_INIT();
	ADC_init();
	ADC_select_channel(2);
	while(1) {
		uint16_t adc_value = read_ADC();
		
		Button_t pressed = Button_ADC_getPressed(adc_value); //폴링으로 눌린 버튼을 저장
		
		if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x
			switch(pressed) {
				case BUTTON_SPEED_UP:
				UART_printString("Button 1 is pushed");
				break;
				case BUTTON_SPEED_DOWN:
				UART_printString("Button 2 is pushed");
				break;
				case BUTTON_ANGLE_UP:
				UART_printString("Button 3 is pushed");
				break;
				case BUTTON_ANGLE_DOWN:
				UART_printString("Button 4 is pushed");
				break;
				case BUTTON_ON_OFF:
				UART_printString("Button 5 is pushed");
				break;
			}
		}
		//UART_printString("uart is working");
		
		
		// 다른 작업들...
	}
}





