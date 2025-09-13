/*
 * timer_0.c
 *
 * Created: 2025-09-12 오후 10:35:08
 *  Author: User
 */ 


#include "timer_0.h"

//프로그램 시작 이후 경과시간을 전역변수로 선언
volatile unsigned long timer0_millis = 0;
volatile int timer0_micros = 0;

//timer0 초기화
void timer0_init() {
	TCCR0B |= (1 << CS01) | (1 << CS00); // Timer0 활성화, 내부 클럭 소스 사용, 분주비 64로 설정
	TIMSK0 |= (1 << TOIE0); // Timer0 오버플로 인터럽트 활성화
	sei(); //전역 인터럽트 활성화
}


// 오버플로우가 몇초마다 발생하는지 아니까 그걸 이용해서 오버플로우 발생시마다 경과시간 누적
// 즉, 인터럽트가 주기적으로 계속 발생하며 누적 경과 시간을 계속해서 업데이트함
ISR(TIMER0_OVF_vect) {
	unsigned long m = timer0_millis;
	int f = timer0_micros;
	
	m += MILLIS_INCREMENT_PER_OVERFLOW; //Timemr0 오버플로우 발생시마다 걸린 밀리초(몫)단위 누적. 누적(경과) 밀리초
	f += MICROS_INCREMENT_PER_OVERFLOW; //Timer0 오버플로우 발생시마다 걸린 마이크로초단위(나머지) 누적. 누적(경과) 마이크로초
	// 이 몫과 나머지를 전부 더한게 실제로 오버플로 인터럽트 1회 발생시까지 걸리는 시간이다. 즉 카운트가 0~255 까지 총 256개의 수 셀때까지 걸리는 시간.
	
	// 오버플로 인터럽트 발생시마다 누적해온 마이크로초가(나머지,찌꺼기) 1000을 넘어가면 밀리초 단위로 변환
	int micro_to_millis = f / 1000;
	// 앞서 밀리초로 변환한 값을 누적 밀리초에 더함.
	m += micro_to_millis;
	
	// 앞서 밀리초로 변환한 만큼을 f에서 빼준다.
	f = f % 1000;
	
	// 전역 변수에 ISR에서 수정한 누적(경과) 시간 값을 반영해준다.
	timer0_millis = m;
	timer0_micros = f;
	
}

//atomic하게 ms단위의 누적 경과 시간 값을 읽어서 반환하는 함수
unsigned long millis() {
	unsigned long m;
	uint8_t oldSREG = SREG; //SREG 상태 레지스터값을 저장
	
	cli(); // SREG 레지스터의 I를 0으로 clear하여 인터럽트 비활성화
	
	m = timer0_millis; // m에 오버플로 인터럽트로 계산한 경과 밀리초 저장
	
	SREG = oldSREG; // 다시 인터럽트 활성화
	
	return m; // 수정한 경과 밀리초 반환
}

// timer0_millis 자료형의 오버플로 방지를위하여 1시간마다 누적 시간을 0으로 다시 초기화	
void timer0_reset() {
	unsigned long m;
	uint8_t oldSREG = SREG; //SREG 상태 레지스터값을 저장
	
	cli(); // SREG 레지스터의 I를 0으로 clear하여 인터럽트 비활성화
	
	timer0_millis = 0;
	timer0_micros = 0; 
	
	SREG = oldSREG; // 다시 인터럽트 활성화
}



