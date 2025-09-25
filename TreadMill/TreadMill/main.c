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
#include "program_mode.h"

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
   previous_state = IDLE;
   timer_ms debug_timer;
   
   
   while(1){
	  previous_state = current_state;
	
      uint16_t adc_value = read_ADC();
      Button_t pressed = Button_ADC_getPressed(adc_value);
	  load_active = load_cell_status_check(load_offset);

      if (pressed != BUTTON_NONE) { // 버튼 1개라도 눌리면 실행됨. 버튼 안눌리면 실행 x

		switch(pressed) {
			case BUTTON_SPEED_UP:
            UART_printString("BUTTON_SPPED_UP is pushed\n");
			if (speed_level < LEVEL_MAX && current_state==RUNNING){
				speed_level++;
				motor_dc_change();
				lcd_speed_up();
			}

            break;
         case BUTTON_SPEED_DOWN:
            UART_printString("BUTTON_SPPED_DOWN is pushed\n");
			if (speed_level > LEVEL_MIN && current_state==RUNNING){
				speed_level--;
				motor_dc_change();
			    lcd_speed_down();
			}
            break;
         case BUTTON_ANGLE_UP:
            UART_printString("BUTTON_ANGLE_UP is pushed\n");
			if (angle_level < LEVEL_MAX && current_state==RUNNING){
				angle_level++;
				motor_step_change(angle_level, STEP_UP);
		        lcd_angle_up();
			}
            break;
         case BUTTON_ANGLE_DOWN:
            UART_printString("BUTTON_ANGLE_DOWN is pushed\n");
			if (angle_level > LEVEL_MIN && current_state==RUNNING){
				angle_level--;
				motor_step_change(angle_level, STEP_DOWN);
				lcd_angle_down();
			}
			break;
		 case BUTTON_PROGRAM_A:
				timer_reset_74595();
				current_state = INIT;
				//select_program(LOSE_MY_MIND);		// 원하는 곡 선택
				next_state = PROGRAM_A;

            break;
         case BUTTON_ON_OFF:
            //UART_printString("BUTTON_ON_OFF is pushed\n");
            if(current_state == IDLE) {
				timer_reset_74595();
				current_state = INIT; //상태를 INIT으로 변경
				next_state = RUNNING;
            }
            else if ((current_state == RUNNING) || (current_state == PROGRAM_A)) {
				timer_reset_74595();
				motor_dc_stop();
				program_stop();
				turn_off = true;
				motor_step_change(angle_level, STEP_DOWN);
				turn_off = false;
                current_state = IDLE; // 상태를 IDLE로 변경
            }
            break;
         default:
            UART_printString("정의되지 않은 버튼 입력\n");
            break;
         }
      }
	  
	 if(is_INIT_done) {
		 switch(next_state) {
			 case PROGRAM_A:
				current_state = PROGRAM_A;
				is_INIT_done = 0;
			 break;
			 case RUNNING:
				current_state = RUNNING;
				is_INIT_done = 0;
			 break;
			 default:
				UART_printString("Unknown next_state");
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
			 speed_level=0;
			 angle_level=1;
			 

		     break;
		     case RUNNING :
		     UART_printString("RUNNING!!!!\n");
			 speed_level=1;
		     motor_dc_setup();
			 next_state = IDLE;
		     break;
			 
		     case EMERGENCY_STOP :
		     UART_printString("EMERGENCY_STOP\n");
		     motor_dc_stop();
			 program_stop();
			 turn_off = true;
			 motor_step_change(angle_level, STEP_DOWN);
			 turn_off = false;
			 speed_level=0;
			 angle_level=1;
			 
		     break;
           case PROGRAM_A :
		     UART_printString("PROGRAM_A!!!!\n");
			 program_init();
		     motor_dc_setup();
			 next_state = IDLE;
		     break;
	     }
     }
		
	  	if(((current_state == RUNNING) || (current_state == PROGRAM_A)) && !(load_active))
	  	{
		  	current_state = IDLE;
	  	}
		/*
	  	if(((current_state == RUNNING) || (current_state == PROGRAM_A)) && !(load_active))
	  	{
		  	current_state = EMERGENCY_STOP;
	  	}
	  	
	  	else if ((current_state == EMERGENCY_STOP) && (load_active)){
		  	current_state = IDLE;
	  	}
		*/
		/*
		if (current_state == PROGRAM_A){
			program_play();
		}
		*/
		print_7_segment();//7세그먼트 동작
		lcd_state_change();
		lcd_print_info();
		lcd_print_program();
		motor_step_update();
			
		if(timer_delay_ms(&debug_timer, 500)) {
				UART_print16bitNumber(current_state);
				UART_printString("\n");
		}
     	


	
}
return 1;
   
}
