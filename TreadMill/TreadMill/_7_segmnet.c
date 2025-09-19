/*
 * _7_segment.c
 *
 * Created: 2025-09-12 오후 11:36:43
 *  Author: User
 */ 
#include "_7_segment.h"

//비트패턴
uint8_t pattern[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09}; //숫자패턴 0~9를 미리 배열에 미리 저장해둠
uint8_t digit[] = {0x08, 0x04, 0x02, 0x01}; //자릿수 선택 
uint8_t num_digits[4];
uint8_t cnt = 0;
STATE previous_state = IDLE;


//16비트 프로토콜 만드는 함수
// 입력: 몇 번째 자리에 출력할지, 어떤 숫자 출력할지
uint16_t make_16bit_protocol (uint8_t digit_num_minus_one, uint8_t num) {
	uint16_t protocol_16bit = combine_uint8(digit[digit_num_minus_one], pattern[num]);
	if(digit_num_minus_one == 1) protocol_16bit &= ~(1 << 0); //자릿수가1이면 점(.) 출력
	return protocol_16bit;
}

//74595랑 연결된 atmega328p의 핀 초기화
void Init_74595(void) {
	// data pin (SER): PC0
	// shift clock pin (SCK) : PC1
	// latch clock pin (RCK) : PB0
	
	DDRC |= (1 << SER) | (1 << SCK) ; //SER,SCK 와 연결된 atmeag328p의 핀을 출력으로 설정
	DDRB |= (1 << RCK); // RCK와 연결된 atmeag328p의 핀을 출력으로 설정
}

//timer0 초기화 및 74595랑 연결된 atmega328p의 핀 초기화 -> 7세그먼트 쓰려면 이 함수 호출하면됩니다.
void init_7_segment() {
	timer1_init(); //타이머0 활성화
	Init_74595(); //74595랑 연결된 atmega328p의 핀 초기화
}

// 타이머 0 오버플로 인터럽트 활성화 + timer0_millis 와 timer0_micros가 업데이트되기 시작함
// 이 함수호출하면 카운트 시작
void count_on_7_segment() {
	timer1_count_start();
}


// 타이머0 오버플로 인터럽트 비활성화 + timer0_millis 와 timer0_micros 전부 0으로 초기화 
// 이 함수 호출하면 카운트 중지
void count_off_7_segment() {
	timer1_count_end();
}



void print_7_segment() {
	int i = 0;
	if(current_state == RUNNING && previous_state == IDLE) {
		static uint8_t _3sec_counter = 3; //첫 선언문 실행시에만 초기화됨
		if(is_1_sec_passed(0) && current_state == RUNNING) { //1초지날떄마다 한 번 씩 호출
			_3sec_counter--;
			WordDataWrite(make_16bit_protocol(3,_3sec_counter) | (1 << BUZZER)); //1초마다 부저로 소리내기
			_delay_ms(SEGMENT_DELAY);
		} else { // 초기화 단계 - 평상시에 호출
			WordDataWrite(make_16bit_protocol(3,_3sec_counter));
		    _delay_ms(SEGMENT_DELAY);
		}
		if(_3sec_counter == 0) {
			_3sec_counter = 3;
			previous_state = current_state; //이 때 previous_state가 RUNNING으로 바뀜
			min = 0;
			for(i=0;i<4;i++) num_digits[i]=0; //RUNNING으로 바뀌기 전에 이전 RUNNING 단계에 사용된 num_digits을 0으로 초기화해준다.
		}
		return;
		}
	
	switch(current_state) {
		
		
		case IDLE:
			for(i=0; i<4; i++) {
			WordDataWrite(make_16bit_protocol(i,0) | (1 << RED) | (1 << BLUE)); //74HC595로 1,2,3,4 자리에 0 출력하도록하는 프로토콜 전송, 초록불(12,14를 1로)
			_delay_ms(SEGMENT_DELAY);
			}
			
			break;

		case RUNNING:
			if(is_1_sec_passed(0) () && current_state == RUNNING) { //1초 지났는지 + cnt가 1인지 check
				num_digits[0] = min / 60 / 10; //시간의 십의 자리 timer1.c의 extern 선언된 min에 직접 접근하여 값을 복사해옴
				num_digits[1] = min / 60 % 10; // 시간의 일의 자리
				num_digits[2] = (min % 60) / 10; //분의 십의 자리
				num_digits[3] = (min % 60) % 10; //분의 일의 자리			
			} 
		
			for(i=0; i<4; i++)  { //1초 지나기전가지 num_digit이 그대로 남아있다
			WordDataWrite(make_16bit_protocol(i,num_digits[i]) | (1 << RED) | (1 << GREEN)); //74HC595로 16비트 데이터 전송(SER,SCK 16번 반복후 RCK도 쏴준다. 즉 7-segment에 병렬로 출력까지한다)
			// 파란색 -> 12,13을 1로
			_delay_ms(SEGMENT_DELAY);
			}
			break;

		case EMERGENCY_STOP:

			break;


	}
	previous_state = current_state;

}


//init_7_segment() 호출하고나서 메인 이벤트 내부에서 써야하는 함수
/* 
void print_7_segment() {
	
	//previous_cnt가 0을 유지하다가 cnt=1이 들어온순간
	if(current_state == RUNNING && previous_state == IDLE) {
		static uint8_t _3sec_counter = 3; //첫 선언문 실행시에만 초기화됨
		if(is_1sec_passed () && cnt) {
			_3sec_counter--;
		}
		WordDataWrite(make_16bit_protocol(3,_3sec_counter));
		_delay_ms(SEGMENT_DELAY);
		if(_3sec_counter == 0) {
			_3sec_counter = 3;
			previous_state = current_state;
			min = 0;
		}
		return;
	}
	

	//previous_cnt가 0을 유지하다가 cnt=1이 들어온순간
	if(current_state == EMERGENCY_STOP && previous_state == RUNNING) {
		static uint8_t _3sec_counter = 3; //첫 선언문 실행시에만 초기화됨
		if(is_1sec_passed () && cnt) {
			_3sec_counter--;
		}
		WordDataWrite(make_16bit_protocol(3,_3sec_counter));
		_delay_ms(SEGMENT_DELAY);
		if(_3sec_counter == 0) {
			_3sec_counter = 3;
			previous_state = current_state;
			min = 0;
		}
		return;
	}
	previous_state = current_state;


	//
	if(is_1sec_passed () && current_state == RUNNING) { //1초 지났는지 + cnt가 1인지 check
		num_digits[0] = min / 60 / 10; //시간의 십의 자리
		num_digits[1] = min / 60 % 10; // 시간의 일의 자리
		num_digits[2] = (min % 60) / 10; //분의 십의 자리
		num_digits[3] = (min % 60) % 10; //분의 일의 자리			
	}
	int i = 0;
	if(current_state == RUNNING)  // 7_segment가 카운트중인 경우
	{
		for(i=0; i<4; i++) 
		{
			WordDataWrite(make_16bit_protocol(i,num_digits[i]) | (1 << 12) | (1 << 13)); //74HC595로 16비트 데이터 전송(SER,SCK 16번 반복후 RCK도 쏴준다. 즉 7-segment에 병렬로 출력까지한다)
			// 파란색 -> 12,13을 1로
			_delay_ms(SEGMENT_DELAY);
		}
	} else if(current_state == IDLE) //7_segment가 idle 상태인경우
	{ 
		for(i=0; i<4; i++) {
			WordDataWrite(make_16bit_protocol(i,0) | (1 << 12) | (1 << 14)); //74HC595로 1,2,3,4 자리에 0 출력하도록하는 프로토콜 전송, 초록불(12,14를 1로)
			_delay_ms(SEGMENT_DELAY);
		}
	} 
}
*/


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