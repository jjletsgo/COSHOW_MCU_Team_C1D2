#include "board.h"
#include "button.h"
#include <util/delay.h>
#include "motor_dc.h"

const uint8_t  SpeedPwm[LEVEL_MAX + 1]  = SPEED_TABLE;
const uint8_t AngleDeg[LEVEL_MAX + 1] = ANGLE_TABLE;
volatile uint8_t speed_level = 0;
volatile uint8_t angle_level = 0;
volatile bool mode = false;	// mode true : 속도제어 / mode false : 기울기 제어
volatile uint8_t value = 0;

uint8_t level_setup(uint8_t level){
	if (mode == true) return output_pwm(level);
	else return output_deg(level);
}

void motor_setup(void){
	motor_dc_init();
	speed_level++;
	mode = true;
	value = level_setup(speed_level);
	motor_dc_start(value);
}


void motor_dc_control(Button_t pressed){
	motor_dc_init();
	
	switch (pressed){
	case BUTTON_SPEED_UP:
		speed_level++;
		mode = true;
		
		break;
	
	default:
		break;
	}
	motor_dc_start(d);
}

}
