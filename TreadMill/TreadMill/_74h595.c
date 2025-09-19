/*
 * _7_segment.c
 *
 * Created: 2025-09-12 오후 11:36:43
 *  Author: User
 */ 
#include "_74595.h"

//비트패턴
uint8_t pattern[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09}; //숫자패턴 0~9를 미리 배열에 미리 저장해둠
uint8_t digit[] = {0x08, 0x04, 0x02, 0x01}; //자릿수 선택 
uint8_t num_digits[4];
STATE previous_state = IDLE;
timer_s _7_segment_timer;
timer_ms _7_segment_delay_timer;
timer_ms segment_display_timer;  // 세그먼트 디스플레이용 타이머 추가
timer_ms buzzer_timer;          // 부저용 별도 타이머 추가

//16비트 프로토콜 만드는 함수
// 입력: 몇 번째 자리에 출력할지, 어떤 숫자 출력할지
uint16_t make_16bit_protocol (uint8_t digit_num_minus_one, uint8_t num) {
	uint16_t protocol_16bit = combine_uint8(digit[digit_num_minus_one], pattern[num]);
	if(digit_num_minus_one == 1) protocol_16bit &= ~(1 << 0); //자릿수가1이면 점(.) 출력
	return protocol_16bit;
}

//74595랑 연결된 atmega328p의 핀 초기화
void init_74595(void) {
	// data pin (SER): PC0
	// shift clock pin (SCK) : PC1
	// latch clock pin (RCK) : PB0
	
	DDRC |= (1 << SER) | (1 << SCK) ; //SER,SCK 와 연결된 atmeag328p의 핀을 출력으로 설정
	DDRB |= (1 << RCK); // RCK와 연결된 atmeag328p의 핀을 출력으로 설정
}


void timer_reset_74595() {
	_7_segment_timer.is_init_done=0;
	_7_segment_delay_timer.is_init_done=0;
	segment_display_timer.is_init_done=0;
	buzzer_timer .is_init_done=0;
}

void print_7_segment() {
	static uint16_t min = 0;
	static uint8_t display_digit = 0;  // 현재 표시할 자릿수
	int i = 0;

	if(current_state == RUNNING && previous_state == IDLE) {
		static uint8_t _3sec_counter = 3; //첫 선언문 실행시에만 초기화됨
		static uint8_t buzzer_duration = 0;  // 부저 지속 시간 카운터
		
		// 1초마다 카운터 감소 및 부저 활성화
		if(timer_delay_s(&_7_segment_timer, 1) && current_state == RUNNING) {
			_3sec_counter--;
			buzzer_duration = 200;  // 200ms 동안 부저 켜기 (200/5 = 40회 호출)
		}
		
		// 1초 카운트 if문 true 되야지 실행됨. 부저 지속시간 점점 감소
		// 5ms마다 체크해서 부저 남은시간 갱신
		if(buzzer_duration > 0 && timer_delay_ms(&buzzer_timer, 5)) {  
			buzzer_duration -= 5;
		}
		
		// 화면 출력 (SEGMENT_DELAY 마다 실행됨)
		if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
			if(buzzer_duration > 0) {
				WordDataWrite(make_16bit_protocol(3, _3sec_counter) | (1 << BUZZER));
			} else { //평상시에 실행되는 함수 
				WordDataWrite(make_16bit_protocol(3, _3sec_counter));
			}
		}
		
		if(_3sec_counter == 0) {
			_3sec_counter = 3;
			segment_display_timer.is_init_done = 0; 
			previous_state = current_state; //이 때 previous_state가 RUNNING으로 바뀜
			min = 0; //idle에서 running으로 변할때 실행되므로, 여기서 min을 0으로 초기화
			for(i=0;i<4;i++) num_digits[i]=0; //RUNNING으로 바뀌기 전에 이전 RUNNING 단계에 사용된 num_digits을 0으로 초기화해준다.
		}
		return;
	}

	
	
	
	switch(current_state) {
		case IDLE:
			// 타이머 기반으로 순차 디스플레이
			if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
				WordDataWrite(make_16bit_protocol(display_digit, 0) | (1 << RED) | (1 << BLUE));
				display_digit = (display_digit + 1) % 4;  // 0~3 순환
			}
			break;

		case RUNNING:
			if(timer_delay_s(&_7_segment_timer, 1) && current_state == RUNNING) {
				 min++;  //1초 지나는걸 타이머로 측정하여 1초당 1분을 증가시킴. 원래는 60초당 1분으로 하는게 맞는데 시연을 위해 1분을 1초로 가정
				UART_printString("1_sec_timer_expired, changing segment number\n");
				UART_printString("min: ");
				UART_print8bitNumber(min);
				UART_printString("\n");
				num_digits[0] = min / 60 / 10; //시간의 십의 자리
				num_digits[1] = min / 60 % 10; // 시간의 일의 자리
				num_digits[2] = (min % 60) / 10; //분의 십의 자리
				num_digits[3] = (min % 60) % 10; //분의 일의 자리			
			} 
		
			// 타이머 기반으로 순차 디스플레이
			if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
				WordDataWrite(make_16bit_protocol(display_digit, num_digits[display_digit]) | (1 << RED) | (1 << GREEN));
				display_digit = (display_digit + 1) % 4;  // 0~3 순환
			}
			break;

		case EMERGENCY_STOP:
			// Emergency stop 로직 구현 필요
			break;
	}
	previous_state = current_state;
}

// SCK로 펄스하나 쏴주는 함수. 평소에는 LOW 상태 유지하므로 -> SCK로 positive edge 하나 쏴주는 것 -> 1bit shift 된다.
void ShiftClock(void) {
	PORTC |= (1 << SCK); // SCK 를 HIGH로 설정
	PORTC &= ~(1 << SCK); // SCK 를 LOW로 설정하고 , 나머지는 유지
}

// RCK로 펄스하나 쏴주는 함수. 평소에는 LOW 상태 유지하므로 -> RCK로 positive edge 하나 쏴주는 것 -> G가 GND(=LOW)에 연결되어있으므로
// 이 함수 쓰면 시프트 레지스터에 저장되있던 8비트 값이 병렬 출력된다.
void LatchClock(void)
{
	PORTB |= (1 << RCK); // PORTB의 0번 비트 (PC6=RCK) 를 HIGH로 설정
	PORTB &= ~(1 << RCK); // PORTB의 0번 비트 (PC6=RCK) 를 LOW로 설정
}

void WordDataWrite(uint16_t data) { //인자로 16비트 패턴을 전달받음
	// 16번 반복하며 데이터 전달
	for(uint8_t i = 0 ; i < 16 ; i++) {
		if(data & 0x8000)  // data의 가장 왼쪽 비트 (15번 비트)가 1이면 해당 if문이 실행된다.
		PORTC |= (1 << SER); // PC4=SER 을 1로 set 하고, 나머지는 그대로 유지한다.
		else // data의 가장 왼쪽 비트 (15번 비트)가 0이면 실행된다.
		PORTC &= ~(1 << SER); // PC4=SER 을 0으로 clear 하고, 나머지는 그대로 유지한다.
		
		ShiftClock(); //1bit shift (즉 SER로 쏴준 데이터가 쉬프트 레지스터의 D F/F에 저장됨)
		data = data << 1; //인자로 전달받은 16bit data를 1bit 왼쪽으로 shift
	}
	
	LatchClock(); // 시프트 레지스터에 저장되어있던 16비트값이 병렬로 출력된다.
}
