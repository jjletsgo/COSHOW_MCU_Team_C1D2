#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

/* ----- 설정값 ----- */
// 한 바퀴당 펄스 수(자석 개수 × 센서 트리거 수)
#define PULSES_PER_REV   1U      // 자석 1개면 1, 2개면 2 ...

/* ----- 전역 ----- */
volatile uint32_t g_hall_pulse_count = 0;   // INT1에서 증가
volatile uint16_t g_rpm = 0;                // 1초마다 계산된 RPM
volatile uint16_t g_ms = 0;                 // 1ms 타이머 누적

/* ----- INT1: PD3 하강엣지에서 펄스 카운트 ----- */
ISR(INT1_vect)
{
	g_hall_pulse_count++;
}

/* ----- Timer2 CTC: 1ms tick ----- */
ISR(TIMER2_COMPA_vect)
{
	if (++g_ms >= 1000) {               // 1초 경과
		uint32_t cnt = g_hall_pulse_count;
		g_hall_pulse_count = 0;
		g_ms = 0;

		// RPM = (초당 펄스) * 60 / (한바퀴당 펄스)
		if (PULSES_PER_REV > 0)
		g_rpm = (uint16_t)((cnt * 60UL) / PULSES_PER_REV);
		else
		g_rpm = 0;
	}
}

/* ----- 초기화 ----- */
static void hall_int1_init(void)
{
	// PD3 입력 + 내부 풀업(외부 풀업 있으면 이 줄 생략 가능)
	DDRD  &= ~(1<<PD3);
	PORTD |=  (1<<PD3);

	// 외부 인터럽트 INT1: 하강엣지(ISC11=1, ISC10=0)
	EICRA &= ~(1<<ISC10);
	EICRA |=  (1<<ISC11);
	EIFR  |=  (1<<INTF1);   // pending clear
	EIMSK |=  (1<<INT1);    // INT1 enable
}

static void timer2_1ms_init(void)
{
	// Timer2 CTC, 1ms 주기
	// 16MHz / 64 = 250kHz -> 1ms(1000Hz) 위해 OCR2A = 249
	TCCR2A = (1<<WGM21);             // CTC
	TCCR2B = (1<<CS22);              // prescale 64
	OCR2A  = 249;
	TCNT2  = 0;
	TIFR2  |= (1<<OCF2A);            // pending clear
	TIMSK2 |= (1<<OCIE2A);           // compare A interrupt
}

int main(void)
{
	hall_int1_init();
	timer2_1ms_init();
	sei();   // 글로벌 인터럽트 허용

	// 여기서부터는 원하는 제어(모터 제어/표시 등)를 수행
	while (1) {
		// g_rpm 값은 1초마다 갱신됨
		// 필요하면 이 자리에서 g_rpm을 읽어 처리
		// 예: 일정 RPM 유지 루프, UART로 출력 등
		_delay_ms(50);
	}
}
