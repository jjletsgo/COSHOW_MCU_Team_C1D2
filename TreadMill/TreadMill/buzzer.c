/*
 * buzzer.c
 *
 * Created: 2025-09-28 오후 12:05:05
 *  Author: sec
 */ 

#include "buzzer.h"

uint16_t buzzer_duration = 0;
timer_ms buzzer_timer;          // 부저용 별도 타이머 추가


void buzzer_init(void){
	DDRB  |= (1 << PB2); // PB2 출력 설정
	TCCR1A = 0; TCCR1B = 0; //비교일치 인터럽트 해제
	TCCR1A |= (1 << WGM11); 
	TCCR1B |= (1 << WGM13) | (1 << WGM12);
	TCCR1A |= (1 << COM1B1);
	OCR1B = 0; // PWM값 출력. 여기에 적으면 PWM 출력된다.
	TCCR1B |= (1 << CS11) | (1 << CS10);  // 분주비 64
}

void buzzer_stop(void){
	OCR1B = 0; //PWM값 0으로 선택
	TCCR1A &= ~(1 << COM1B1); //PWM모드 해제
	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10)); 
	DDRB  &= ~(1 << PB2); //출력모드 해제
	PORTB &= ~(1 << PB2); //출력 모드 해제

}

void buzzer_work(void){
	if (buzzer_duration){
		ICR1 = FREQ_TO_ICR(1000);
		OCR1B = ICR1 / 2; //실제로 쏘는 부분
	}

	if(buzzer_duration && (timer_delay_ms(&buzzer_timer, 5))) {
		buzzer_duration -= 5;
	}
	else if (!buzzer_duration){
		buzzer_stop();
	}
}

void set_buzzer_duration() {
	buzzer_duration = 100;
}