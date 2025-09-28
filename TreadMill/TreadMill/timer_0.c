/*
 * timer_0.c
 *
 * Created: 2025-09-12 오후 10:35:08
 *  Author: User
 */ 

#include "timer_0.h"

//프로그램 시작 이후 경과시간을 전역변수로 선언
volatile unsigned long timer0_millis = 0; //누적 시간 (밀리 단위)
volatile int timer0_micros = 0 ;// 누적 시간 (마이크로 단위. 찌거기로 사용)
volatile unsigned long present_time = 0;
volatile unsigned long previous_time = 0;

//timer0 초기화
void timer0_init() {
	TCCR0B |= (1 << CS01); // Timer0 활성화, 내부 클럭 소스 사용, 분주비 8로 설정
	TIMSK0 |= (1 << TOIE0); // Timer0 오버플로 인터럽트 활성화
	sei(); //전역 인터럽트 활성화
	previous_time = millis();
}

// 타이머0 오버플로 인터럽트 비활성화 -> timer0_millis 와 timer0_micros가 더이상 업데이트되지않음.
void timer0_end() {
	TIMSK0 &= ~(1 << TOIE0); // Timer0 오버플로 인터럽트 비활성화
	timer0_millis = 0;
	timer0_micros = 0;
	present_time = 0;
	previous_time = 0;
}

//인자로 타이머의 주소와 , ms 단위의 딜레이 간격을 주시면됩니다. 딜레이는 0~255ms 까지 가능합니다
uint8_t timer_delay_ms(timer_ms *timer, uint16_t ms_interval) {
	if (timer->is_init_done && (millis() - timer->ms_time >= ms_interval)) { //초기화 완료된 타이머이고 현재 시간과 타이머의 차이가 ms_interval 이상인 경우
        timer->ms_time = millis();  // 기준점 갱신
		timer->is_init_done = 0; // 다음번 사용을 위해 초기화 안된 것으로 설정
        return 1;           // 딜레이 끝남
    } else if(timer->is_init_done == 0) { //초기화 안된 타이머의 경우
		timer->ms_time = millis(); // 타이머를 현재 누적 ms로 설정
		timer->is_init_done = 1; // 타이머를 초기화 완료된 것으로 설정
	}
    return 0;   
}

//인자로 타이머의 주소와 , s 단위의 딜레이 간격을 주시면됩니다. 딜레이는 0~255s 까지 가능합니다
uint8_t timer_delay_s(timer_s *timer, uint16_t s_interval) {
	if (timer->is_init_done && (millis() - timer->s_time >= s_interval*1000)) { //초기화 완료된 타이머이고 현재 시간과 타이머의 차이가 ms_interval 이상인 경우
		timer->s_time = millis();  // 기준점 갱신
		timer->is_init_done = 0; // 다음번 사용을 위해 초기화 안된 것으로 설정
		return 1;           // 딜레이 끝남
	} else if(timer->is_init_done == 0) { //초기화 안된 타이머의 경우
		timer->s_time = millis(); // 타이머를 현재 누적 ms로 설정
		timer->is_init_done = 1; // 타이머를 초기화 완료된 것으로 설정
	}
	return 0;
}

// 오버플로우가 몇초마다 발생하는지 아니까 그걸 이용해서 오버플로우 발생시마다 경과시간 누적
// 즉, 인터럽트가 주기적으로 계속 발생하며 누적 경과 시간을 계속해서 업데이트함
ISR(TIMER0_OVF_vect) {
	unsigned long m = timer0_millis;
	int f = timer0_micros;
	
	m += MILLIS_INCREMENT_PER_OVERFLOW; //Timer0 오버플로우 발생시마다 걸린 밀리초(몫)단위 누적. 누적(경과) 밀리초
	f += MICROS_INCREMENT_PER_OVERFLOW; //Timer0 오버플로우 발생시마다 걸린 마이크로초단위(나머지) 누적. 누적(경과) 마이크로초
	// 이 몫과 나머지를 전부 더한게 실제로 오버플로 인터럽트 1회 발생시까지 걸리는 시간이다. 즉 카운트가 0~255 까지 총 256개의 수 셀때까지 걸리는 시간.
	
	// 오버플로 인터럽트 발생시마다 누적해온 마이크로초가(나머지,찌꺼기) 1000을 넘어가면 밀리초 단위로 변환
	int micro_to_millis = f / 1000;
	// 앞서 밀리초로 변환한 값을 누적 밀리초에 더함.
	m += micro_to_millis;
	
	// 앞서 밀리초로 변환한 만큼을 f에서 빼준다.
	f = f % 1000;
	
	// 외부로 노출되는 전역 변수에 ISR에서 수정한 누적(경과) 시간 값을 반영해준다.
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

//atomic하게 s단위의 누적 경과 시간 값을 읽어서 반환하는 함수
uint16_t secs() {
	return (uint16_t)(millis()/1000);  
}