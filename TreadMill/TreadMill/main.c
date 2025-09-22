/*
 * main.c
 *
 * Created: 2025-09-17 오후 10:53:11
 *  Author: AFHGF
 */ 
#include "common.h"
#include "ADC.h"
#include "button.h"
#include "display_ctrl.h"
#include "UART.h"
#include "motor_dc.h"
#include "motor_step.h"
#include "encoder.h"
#include "_74hc595.h"
#include "load_cell.h"
#include "emergency_stop.h"
#include "timer_0.h"

#include <util/delay.h>
int32_t load_offset = 0;
bool load_active = false;

static const uint8_t fullBlock[8] = {
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b11111,
   0b11111
};

int main(void){
   
   i2c_init();
   lcd_init();
   timer0_init();
   lcd_create_char(0, fullBlock);
   UART_INIT();
   Button_Init();
   ADC_init();
   ADC_select_channel(2);
   init_74595();
   encoder_init();
   load_cell_init();
   emergency_stop_init();

   load_offset = load_cell_read();		// offset 구하기
   STATE previous_state = IDLE;
   
   
   while(1){

      uint16_t adc_value = read_ADC();
      Button_t pressed = Button_ADC_getPressed(adc_value);
	  load_active = load_cell_status_check(load_offset);

      if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x

		switch(pressed) {
			case BUTTON_SPEED_UP:
            UART_printString("BUTTON_SPPED_UP is pushed\n");
            lcd_speed_up();
            motor_dc_up();
            break;
         case BUTTON_SPEED_DOWN:
            UART_printString("BUTTON_SPPED_DOWN is pushed\n");
            lcd_speed_down();
            motor_dc_down();
            break;
         case BUTTON_ANGLE_UP:
            UART_printString("BUTTON_ANGLE_UP is pushed\n");
			   motor_step_up();
            lcd_angle_up();
            break;
         case BUTTON_ANGLE_DOWN:
            UART_printString("BUTTON_ANGLE_DOWN is pushed\n");
			   motor_step_down();
            lcd_angle_down();
			break;
			case BUTTON_PROGRAM_A:
			
            break;
         case BUTTON_ON_OFF:
            //UART_printString("BUTTON_ON_OFF is pushed\n");
            lcd_button_on();
            if(current_state == IDLE) {
				timer_reset_74595();

				current_state = INIT; //상태를 INIT으로 변경

            }
            else if (current_state == RUNNING) {
				timer_reset_74595();
				motor_dc_stop();
				motor_step_stop();
                current_state = IDLE; // 상태를 IDLE로 변경
            }
            break;
         default:
            UART_printString("정의되지 않은 버튼 입력\n");
            break;
         }
      }
     if(current_state != previous_state) {
	     
	     switch(current_state) {
		     case IDLE :
		     UART_printString("IDLE!!!!\n");
		     
		     break;
		     case INIT :
		     UART_printString("INIT!!!!\n");
		     motor_dc_init();
			 motor_step_init(STEP_HALF_STEP);

		     break;
		     case RUNNING :
		     UART_printString("RUNNING!!!!\n");
		     motor_dc_setup();
		     break;
			 
		     case EMERGENCY_STOP :
		     UART_printString("EMERGENCY_STOP\n");
		     motor_dc_stop();
		     motor_step_stop();
			 
		     break;
           case PROGRAM_A :
		     UART_printString("PROGRAM_A!!!!\n");
		     motor_dc_setup();
		     break;
	     }
     }
     previous_state = current_state;
	  
     print_7_segment();//7세그먼트 동작
	 lcd_state_change();
     lcd_print_info();
     motor_step_update();
	
	
	
	if((current_state == RUNNING) && !(load_active)) 
	{
		current_state = EMERGENCY_STOP;
		//current_state = IDLE;
		// UART_printString("EMERGENCY_STOP");
	}
	
	else if ((current_state == EMERGENCY_STOP) && (load_active) && !emergency_trigger){
		 current_state = IDLE;
		 UART_printString("IDLE");
	}
	
   }
	return 1;
   
}
