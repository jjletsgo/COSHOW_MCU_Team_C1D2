#include "common.h"
#include "motor_dc.h"
#include "button.h"
#include <util/delay.h>

const uint8_t  SpeedPwm[LEVEL_MAX + 1]  = SPEED_TABLE;
volatile uint8_t speed_level = 0;
volatile uint8_t duty = 0;


// dc 모터 초기화
void motor_dc_init(void)
{
    DDRB  |= (1 << PB3);  
    TCCR2A = 0; TCCR2B = 0;

    TCCR2A |= (1 << WGM21) | (1 << WGM20);   // Fast PWM
    TCCR2A |= (1 << COM2A1);                // OC2A 비반전 모드
    TCCR2B |= (1 << CS22);                   // 분주비 64
    
    OCR2A = 0;
}


// On되는 순간 초기속도 설정
void motor_dc_setup(void){
	speed_level = 1;
	motor_dc_start(speed_level);
}


// level 입력받아 pwm 출력
void motor_dc_start(uint8_t speed_level)  
{
    uint8_t pwm = pwm_setup(speed_level);
    OCR2A = pwm;
}


// dc 모터 정지
void motor_dc_stop(void)
{
    speed_level = 0;
    OCR2A = 0;
    TCCR2A &= ~(1 << COM2A1);
    PORTB  &= ~(1 << PB3);
}


void motor_dc_up(void){
    speed_level++;
    motor_dc_start(speed_level);
}

void motor_dc_down(void){
    speed_level--;
    motor_dc_start(speed_level);
}


