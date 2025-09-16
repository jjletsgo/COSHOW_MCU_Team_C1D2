#include "board.h"
#include "motor_dc.h"

#define PWM_TOP 249   
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
	speed_level++;
	mode = true;
	value = level_setup(speed_level);
	motor_dc_start(value);
}

void motor_dc_init(void)
{
    DDRB  |= (1 << PB2);  

    TCCR1A = 0;
    TCCR1B = 0;

    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);

    TCCR1A |= (1 << COM1B1);

    ICR1  = PWM_TOP; 
    OCR1B = 0;

    TCCR1B |= (1 << CS11) | (1 << CS10);
}

void motor_dc_start(uint8_t duty)  
{
    uint32_t pwm = (uint32_t)duty * PWM_TOP / 255U;
    if (pwm >= PWM_TOP) pwm = PWM_TOP - 1;
    OCR1B = (uint16_t)pwm;
}

void motor_dc_stop(void)
{
    OCR1B = 0;
    TCCR1A &= ~(1 << COM1B1);
    PORTB  &= ~(1 << PB2);
}
