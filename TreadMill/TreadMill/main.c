
#include "board.h"
#include "state.h"
#include "ADC.h"
#include "button.h"

#include <util/delay.h>


int main(void){
	Button_Init();
	ADC_init();
	ADC_select_channel(2);

	while (1){
		
		switch (state_get()) {
			case IDLE:
		
			break;
			
			case RUN:

			break;
			
			case STOP:

			break;
		}
		
		uint16_t adc_value = read_ADC();

		Button_t pressed = Button_ADC_getPressed(adc_value); //폴링으로 눌린 버튼을 저장

		if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x
			switch(pressed) {
				case BUTTON_SPEED_UP:
				
				break;
				
				case BUTTON_SPEED_DOWN:
				
				break;
				
				case BUTTON_ANGLE_UP:
				
				break;
				
				case BUTTON_ANGLE_DOWN:
				
				break;
				
				case BUTTON_ON_OFF:

				break;
				
				default: break;
			}
		}
		//UART_printString("uart is working");


		// 다른 작업들...
		
	}
}
