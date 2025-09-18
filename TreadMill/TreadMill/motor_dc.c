#include "common.h"
#include "motor_dc.h"
#include "timer_1.h"
#include "button.h"
#include <util/delay.h>

#define PWM_TOP 249   
const uint8_t  SpeedPwm[LEVEL_MAX + 1]  = SPEED_TABLE;
volatile uint8_t speed_level = 0;
volatile uint8_t duty = 0;

// dc 모터 초기화
void motor_dc_init(void)
{
    DDRB  |= (1 << PB2);  
    TCCR1A = 0; TCCR1B = 0;
    TCCR1A |= (1 << WGM11);
    TCCR1B |= (1 << WGM13) | (1 << WGM12);
    TCCR1A |= (1 << COM1B1);
    ICR1  = PWM_TOP; 
    OCR1B = 0;

    timer1_init()
}


// On되는 순간 초기속도 설정
void motor_setup(void){
	speed_level = 1;
	motor_dc_start(speed_level);
}


// level 입력받아 pwm 출력
void motor_dc_start(uint8_t speed_level)  
{
    duty = pwm_setup(speed_level);
    uint32_t pwm = (uint32_t)duty * PWM_TOP / 255U;
    if (pwm >= PWM_TOP) pwm = PWM_TOP - 1;
    OCR1B = (uint16_t)pwm;
}


// dc 모터 정지
void motor_dc_stop(void)
{
    speed_level = 0;
    OCR1B = 0;
    TCCR1A &= ~(1 << COM1B1);
    PORTB  &= ~(1 << PB2);
}


void motor_dc_up(void){
    speed_level++;
    motor_dc_start(speed_level);
}

void motor_dc_down(void){
    speed_level--;
    motor_dc_start(speed_level);
}


