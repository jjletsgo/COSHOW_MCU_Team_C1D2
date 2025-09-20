
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

	// INT0: any logical change (0↔1 모두 트리거)
	EICRA = (EICRA & ~((1<<ISC01)|(1<<ISC00))) | (1<<ISC00);


	// pending flag 클리어 후 enable
	EIFR  |=  (1 << INTF0);
	EIMSK |=  (1 << INT0);
}

ISR(INT0_vect)
{
	// HIGH인지 확인
	if (PIND & (1<<PD2)) {
		motor_dc_stop();
		motor_step_stop();
	}
	else {
		motor_dc_init();
		motor_dc_setup();
		motor_step_init(STEP_HALF_STEP);
	}
}
