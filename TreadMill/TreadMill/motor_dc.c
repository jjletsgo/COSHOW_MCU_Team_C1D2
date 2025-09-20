#include "common.h"
#include "motor_dc.h"
#include "button.h"

const uint8_t  SpeedPwm[LEVEL_MAX + 1]  = SPEED_TABLE;
volatile uint8_t speed_level = 0;
volatile uint8_t duty = 0;


// dc 모터 초기화
void motor_dc_init(void)
{
	DDRB  |= (1 << PB3);        // PB3 = OC2A 출력
	TCCR2A = 0;
	TCCR2B = 0;

	// Fast PWM 모드, TOP=0xFF (OCR2A는 duty 레지스터로만 사용)
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B &= ~(1 << WGM22);

	// OC2A 비반전 모드
	TCCR2A |= (1 << COM2A1);
	// 분주비 64
	TCCR2B |= (1 << CS22);

	OCR2A = 0;   // 듀티 0% 시작
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
	if (speed_level < LEVEL_MAX)
	speed_level++;
	motor_dc_start(speed_level);
}

void motor_dc_down(void){
	if (speed_level > LEVEL_MIN)
	speed_level--;
	motor_dc_start(speed_level);
}


