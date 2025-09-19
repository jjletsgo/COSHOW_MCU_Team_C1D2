/*
 * timer_0.c
 *
 * Created: 2025-09-12 오후 10:35:08
 *  Author: User
 */ 


#include "timer_1.h"

//프로그램 시작 이후 경과시간을 전역변수로 선언
volatile unsigned long timer1_millis = 0; //누적 시간 (밀리 단위)
volatile int timer1_micros = 0 ;// 누적 시간 (마이크로 단위. 찌거기로 사용)
volatile int min = 0; // 누적 분
//volatile uint8_t num_digits[4]; // 이건 7세그먼트쪽에서 쓰는게 맞을듯
volatile unsigned long present_time = 0; //현재시간
volatile unsigned long previous_time = 0; //과거시간

//timer0 초기화
void timer1_init() {
	TCCR1B |= (1 << CS10) | (1 << CS11); // Timer1 활성화, 내부 클럭 소스 사용, 분주비 1024로 설정
	//TIMSK1 |= (1 << TOIE1); // Timer1 오버플로 인터럽트 활성화
	sei(); //전역 인터럽트 활성화
}

void timer1_ovf_start() {
	TIMSK1 |= (1 << TOIE1); // Timer1 오버플로 인터럽트 활성화
	
}

void timer1_ovf_end() {
	TIMSK0 &= ~(1 << TOIE1); // Timer1 오버플로 인터럽트 비활성화
}

// 타이머 1 오버플로 인터럽트 활성화 -> timer1_millis 와 timer1_micros가 업데이트되기 시작함
void timer1_count_start() {
	timer1_ovf_start(); 
	previous_time=millis();
}

// 타이머1 오버플로 인터럽트 비활성화 -> timer1_millis 와 timer1_micros가 더이상 업데이트되지않음.
void timer1_count_end() {
	timer1_ovf_end(); 
	timer1_millis = 0;
	timer1_micros = 0;
	min = 0;
	present_time = 0;
	previous_time= 0;
}


//메인루프에서 실행해야하는 함수 -> 매 반복마다 1초 지났는지 체크
uint8_t is_1sec_passed () {
	present_time = millis();
	//1초 경과 and cnt 플래그가 1이면 실행
	if(present_time-previous_time >= 1000) //시연을 위해 1000ms=1초 로 설정
	{ //1초 경과시
		previous_time = present_time; //이전 시간을 현재 시간으로 설정
		min++;//1분 증가
		//디버깅용, pc로 uart 송신
		UART_printString("1sec_passed\n");
		return 1;
	} else {
		return 0;
	}
}


// 오버플로우가 몇초마다 발생하는지 아니까 그걸 이용해서 오버플로우 발생시마다 경과시간 누적
// 즉, 인터럽트가 주기적으로 계속 발생하며 누적 경과 시간을 계속해서 업데이트함
ISR(TIMER1_OVF_vect) {
	unsigned long m = timer1_millis;
	int f = timer1_micros;
	
	m += MILLIS_INCREMENT_PER_OVERFLOW; //Timemr0 오버플로우 발생시마다 걸린 밀리초(몫)단위 누적. 누적(경과) 밀리초
	f += MICROS_INCREMENT_PER_OVERFLOW; //Timer0 오버플로우 발생시마다 걸린 마이크로초단위(나머지) 누적. 누적(경과) 마이크로초
	// 이 몫과 나머지를 전부 더한게 실제로 오버플로 인터럽트 1회 발생시까지 걸리는 시간이다. 즉 카운트가 0~255 까지 총 256개의 수 셀때까지 걸리는 시간.
	
	// 오버플로 인터럽트 발생시마다 누적해온 마이크로초가(나머지,찌꺼기) 1000을 넘어가면 밀리초 단위로 변환
	int micro_to_millis = f / 1000;
	// 앞서 밀리초로 변환한 값을 누적 밀리초에 더함.
	m += micro_to_millis;
	
	// 앞서 밀리초로 변환한 만큼을 f에서 빼준다.
	f = f % 1000;
	
	// 외부로 노출되는 전역 변수에 ISR에서 수정한 누적(경과) 시간 값을 반영해준다.
	timer1_millis = m;
	timer1_micros = f;
	
}

//atomic하게 ms단위의 누적 경과 시간 값을 읽어서 반환하는 함수
unsigned long millis() {
	unsigned long m;
	uint8_t oldSREG = SREG; //SREG 상태 레지스터값을 저장
	
	cli(); // SREG 레지스터의 I를 0으로 clear하여 인터럽트 비활성화
	
	m = timer1_millis; // m에 오버플로 인터럽트로 계산한 경과 밀리초 저장
	
	SREG = oldSREG; // 다시 인터럽트 활성화
	
	return m; // 수정한 경과 밀리초 반환
}



