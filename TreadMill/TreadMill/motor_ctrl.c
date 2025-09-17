#include "board.h"
#include "button.h"
#include <util/delay.h>
#include "motor_dc.h"



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
