
#include "common.h"
#include <avr/interrupt.h>
#include "emergency_stop.h"
#include "motor_dc.h"
#include "motor_step.h"
#include "button.h"
#include "state.h"

void emergency_stop_init(void)
{
	// INT0 입력
	DDRD  &= ~(1 << PD2);
	PORTD |=  (1 << PD2);

	// INT0: any logical change (0↔1 모두 트리거)
	EICRA &= ~(1 << ISC01);
	EICRA |= (1 << ISC00);


	// pending flag 클리어 후 enable
	EIFR  |=  (1 << INTF0);
	EIMSK |=  (1 << INT0);
}

// ISR만 교체
ISR(INT0_vect)
{
	static uint8_t expecting_rising = 1; // 1: 상승만 처리, 0: 하강만 처리
	uint8_t s2 = (PIND & (1<<PD2)) ? 1 : 0;


	// 이제 s2가 "안정된 현재 레벨"
	if (expecting_rising) {
		// 상승엣지(LOW->HIGH)만 처리: 현재가 HIGH라면 진짜 상승했다고 판단
		if (s2) {
			// 정지
			motor_dc_stop();
			motor_step_stop();
			
			
			// 다음은 하강엣지(FALLING)만 받도록 전환
			expecting_rising = 0;
			EICRA = (EICRA | (1<<ISC01)) & ~(1<<ISC00); // ISC01=1, ISC00=0 → falling only
		}
		// 아니면(LOW) 무시
		} else {
		// 하강엣지(HIGH->LOW)만 처리: 현재가 LOW라면 진짜 하강했다고 판단
		if (!s2) {
			// 재시작
			UART_printString("call here\n");
			motor_dc_init();
			motor_step_init(STEP_HALF_STEP);
			current_state = IDLE;
			// 다음은 상승엣지(RISING)만 받도록 전환
			expecting_rising = 1;
			EICRA |= (1<<ISC01) | (1<<ISC00); // rising only
		}
		// 아니면(HIGH) 무시
	}

	// 남은 pending 플래그 정리
	EIFR |= (1<<INTF0);
}

