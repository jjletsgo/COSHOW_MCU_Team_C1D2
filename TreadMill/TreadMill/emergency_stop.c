#include "common.h"
#include <avr/interrupt.h>
#include "emergency_stop.h"
#include "motor_dc.h"
#include "motor_step.h"
#include "button.h"

void emergency_stop_init(void)
{
    // INT0 입력
    DDRD  &= ~(1 << PD2);
    PORTD |=  (1 << PD2);

    // 상승엣지에서만 감지
    EICRA |=  (1 << ISC00);
	EICRA &= ~(1 << ISC01);

    // pending flag 클리어 후 enable
    EIFR  |=  (1 << INTF0);
    EIMSK |=  (1 << INT0);
}

ISR(INT0_vect)
{
    // HIGH인지 확인
    if (PIND & (1<<PD2)) {
        EIMSK &= ~(1 << INT0); // 인터럽트 재진입 방지

        motor_dc_stop();
        motor_step_stop();
		current_state = IDLE;
    }
	elif (PIND & ~(1<<PD2)){
		EIMSK &= ~(1 << INT0);
		motor_dc_init();
		motor_step_init();
	}
}