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
#include "buzzer.h"

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
				reset_74595();
				current_state = INIT;
				next_state = PROGRAM_A;

            break;
         case BUTTON_ON_OFF:
            //UART_printString("BUTTON_ON_OFF is pushed\n");
            if(current_state == IDLE) {
				reset_74595();
				current_state = INIT; //상태를 INIT으로 변경
				next_state = RUNNING;
            }
            else if ((current_state == RUNNING) || (current_state == PROGRAM_A)) {
				reset_74595();
				motor_dc_stop();
				turn_off = true;
				motor_step_change(angle_level, STEP_DOWN);
				turn_off = false;
                current_state = IDLE; // 상태를 IDLE로 변경
            }
            break;
		 case BUTTON_MUSIC_PLAY:
			buzzer_init();
		 
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
			 buzzer_stop();
			 reset_74595();
			 IDLE_rgb_mask_init();
			
		     break;
		     case INIT :
		     UART_printString("INIT!!!!\n");
		     motor_dc_init();
			 OCR2A = 80;	 
			 motor_step_init(STEP_HALF_STEP);
			 speed_level=0;
			 angle_level=1;
			 reset_74595();
			 INIT_rgb_mask_init();
			 buzzer_init();
			 set_buzzer_duration();
			 

		     break;
		     case RUNNING :
		     UART_printString("RUNNING!!!!\n");
			 speed_level=1;
		     motor_dc_setup();
			 next_state = IDLE;
			 reset_74595();
			 RUNNING_OR_PROGRAM_mask_init();
		     break;
			 
		     case EMERGENCY_STOP :
		     UART_printString("EMERGENCY_STOP\n");
		     motor_dc_stop();
			 buzzer_stop();
			 turn_off = true;
			 motor_step_change(angle_level, STEP_DOWN);
			 turn_off = false;
			 
		     break;
           case PROGRAM_A :
		     UART_printString("PROGRAM_A!!!!\n");
		     motor_dc_setup();
			 next_state = IDLE;
			 reset_74595();
			 RUNNING_OR_PROGRAM_mask_init();
		     break;
	     }
     }
		
	  	if(((current_state == RUNNING) || (current_state == PROGRAM_A)) && !(load_active))
	  	{
		     motor_dc_stop();
		     buzzer_stop();
		     turn_off = true;
		     motor_step_change(angle_level, STEP_DOWN);
		     turn_off = false;
		  	 current_state = IDLE;
	  	}

		if (current_state == PROGRAM_A){
			program_play();
		}
		
		switch(current_state) {
			
		case IDLE:
			IDLE_rgb_mask_init();
			break;
		case INIT:
			if(timer_delay_ms(&INIT_timer, 1000)) {
				OCR2A -= 20; //DC초기속도
				set_buzzer_duration();
				INIT_counter--; //INIT_counter 1 감소
				INIT_rgb_mask_shift();
				buzzer_stop();
				buzzer_init();
			}
			if(INIT_counter == 0) {
				INIT_counter = 3;
				is_INIT_done = 1;
				reset_74595();
				buzzer_stop();
			}
			break;
		case RUNNING:
		
			break;
		case PROGRAM_A:
			
			break;
		case EMERGENCY_STOP:
			EMERGENCY_STOP_mask_init();
			break;		
		}
	 
	 
	 	buzzer_work();	
		print_7_segment();//7세그먼트 동작
		lcd_state_change();
		lcd_print_info();
		lcd_print_program();
		motor_step_update();
		
			
		
	
}
return 1;
   
}
