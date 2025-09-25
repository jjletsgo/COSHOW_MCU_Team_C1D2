/*
 * _7_segment.c
 *
 * Created: 2025-09-12 오후 11:36:43
 *  Author: User
 */ 
#include "_74hc595.h"

//비트패턴
uint8_t pattern[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09}; //숫자패턴 0~9를 미리 배열에 미리 저장해둠
uint8_t digit[] = {0x08, 0x04, 0x02, 0x01}; //자릿수 선택 
uint8_t num_digits[4];
timer_s _7_segment_timer;
timer_ms GP_timer;
timer_ms segment_display_timer;  // 세그먼트 디스플레이용 타이머 추가
timer_ms debug_timer;          // 부저용 별도 타이머 추가
uint8_t _3sec_counter = 3; //첫 선언문 실행시에만 초기화됨
STATE next_state_of_INIT = IDLE;
uint32_t rgb_mask;
uint16_t segment_mask;
uint64_t _64bit_protocol;




//세그먼트 마스크 업데이트하는 함수
// 입력: 몇 번째 자리에 출력할지, 어떤 숫자 출력할지
// 제대로 만드는거 확인됨
void update_12bit_segment_mask (uint8_t digit_num_minus_one, uint8_t num) {
	segment_mask = combine_uint8(digit[digit_num_minus_one], pattern[num]); //8비트 high와 8비트 low결합해서 16비트만듬
	if(digit_num_minus_one == 1) segment_mask &= ~(1 << 0); //자릿수가1이면 점(.) 출력
}

// 왼쪽으로 순환 시프트 함수 (27비트용)
void left_shift_rgb_mask_18_to_26(void) {
	//const uint32_t mask = 0x07FFFFFF; // 27비트 마스크 0b000001111 11111111 11111111 11111111
	static uint32_t blue_9bit = 0x000001E0;
	if(previous_state!=current_state) blue_9bit = 0x000001E0; //상태 바뀐 경우 초기화
	blue_9bit = (blue_9bit << 1 | blue_9bit >> 8) & 0x000001FF ; //하위 9비트만 살림
	//하위9비트만 살린거 왼쪽으로 18번 쉬프트후에 하위 18비트 1로 만들고 27비트 패턴 완성
	rgb_mask = (blue_9bit  << 18) | 0x0003FFFF;
	return;
}



void update__64bit_protocol () {
	_64bit_protocol = (((uint64_t)(rgb_mask & 0x07FFFFFF)) << 12) | segment_mask; //segment_mask는 0000(4비트)자릿수(4비트) + 표시할 숫자(8비트) 총 16비트 형태라 그냥 or 떄리면 하위 12비트가 그대로 들어감
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
	GP_timer.is_init_done=0;
	segment_display_timer.is_init_done=0;
}

void set_74595_next_state_of_INIT(STATE next_state) {
	switch(next_state) {
		case RUNNING:
		next_state_of_INIT = RUNNING;
		break;
		case PROGRAM_A:
		next_state_of_INIT = PROGRAM_A;
		break;
		default:
		UART_printString("Unknown next_state_of_INIT\n" );
		break;
	}
}



void print_7_segment() {
	static uint16_t min = 0;
	static uint8_t display_digit = 0;  // 현재 표시할 자릿수
	int i = 0;


	switch(current_state) {
		case INIT:
			//------------------------------------rgbmask 업데이트---------------------------------------
			//현재 상태가 INIT이고 현재 상태와 이전상태가 다르면 rgb_mask 초기화
			if(current_state != previous_state) {
				//0x07e3f1f8 = 0b 00000111111000111111000111111000	(L1,L2,L3에 WHITE, 나머지는 끄기)
				rgb_mask = 0x07e3f1f8; 
				UART_printString("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
				//UART_printString("this is first INIT!\n");
				//UART_print32bitNumber(rgb_mask);
				//UART_printString("\n");
			} 
			//1초마다 실행
			if(timer_delay_s(&_7_segment_timer, 1) && current_state == INIT) {
				_3sec_counter--; //_3sec_counter 1 감소
				rgb_mask &= rgb_mask << 3;
				//UART_print32bitNumber(rgb_mask);
				//UART_printString("\n");
			}
			
			
			// --------------------SEGMENT_DELAY 마다 실행----segment_mask 업데이트->rgb_mask와 결합해서 64비트 생성
			// -> 병렬 출력
			if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
				if(_3sec_counter ) {  //_3sec_counter가 1 or 2 or 3 이면
					update_12bit_segment_mask(3, _3sec_counter);
					if(timer_delay_ms(&debug_timer, 500)) {
						//UART_print64bitNumber(rgb_mask);
						//UART_printString("\n");
					}
					update__64bit_protocol();
					WordDataWrite(_64bit_protocol);
				} 
			}
			// _3sec_counter가 0인 경우
			if(_3sec_counter == 0) {
				_3sec_counter = 3;
				_7_segment_timer.is_init_done = 0;
				segment_display_timer.is_init_done = 0; 
				min = 0; //idle에서 running으로 변할때 실행되므로, 여기서 min을 0으로 초기화
				for(i=0;i<4;i++) num_digits[i]=0; //RUNNING으로 바뀌기 전에 이전 RUNNING 단계에 사용된 num_digits을 0으로 초기화해준다.
				is_INIT_done = 1;
			}
			break;
		case IDLE:
			//현재 상태가 IDLE이고 현재 상태와 이전상태가 다르면 rgb_mask 초기화
			//상태 달라졌을때만 업데이트0x07fc01ff = 0b 00000111 11111100 00000001 11111111 (Green color mask)
			rgb_mask = 0x07fc01ff;  
			// 타이머 기반으로 순차 디스플레이
			if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
				update_12bit_segment_mask(display_digit, 0);
				update__64bit_protocol();
				WordDataWrite(_64bit_protocol);
				display_digit = (display_digit + 1) % 4;  // 0~3 순환
			}

			break;
			
		
		case RUNNING:	
		case PROGRAM_A:
		//현재 상태가 RUNNING OR PROGRAM이고 현재 상태와 이전상태가 다르면 rgb_mask 초기화
		if(current_state != previous_state) {
			rgb_mask = 0x0783ffff;
			UART_printString("744444444444444444444444444444444444RPRPRPRRP");
		}
		if(timer_delay_s(&_7_segment_timer, 1) && (current_state == RUNNING || current_state == PROGRAM_A)) {
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
		//SPEED LEVEL에 따라 다른 속도로 rgb_mask 업데이트
		switch(speed_level) {
				case 1:
					if(timer_delay_ms(&GP_timer, LV_1_DELAY)) left_shift_rgb_mask_18_to_26();
				break;				
				case 2:
					if(timer_delay_ms(&GP_timer, LV_2_DELAY)) left_shift_rgb_mask_18_to_26();
				break;
				case 3:
					if(timer_delay_ms(&GP_timer, LV_3_DELAY)) left_shift_rgb_mask_18_to_26();
				break;
				case 4:
					if(timer_delay_ms(&GP_timer, LV_4_DELAY)) left_shift_rgb_mask_18_to_26();
				break;
				case 5:
					if(timer_delay_ms(&GP_timer, LV_5_DELAY)) left_shift_rgb_mask_18_to_26();
				break;
				default:
					UART_printString("Undefined speed level\n");
				break;

		}
		// 타이머 기반으로 순차 디스플레이
		if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
			update_12bit_segment_mask(display_digit, num_digits[display_digit]);
			update__64bit_protocol();
			WordDataWrite(_64bit_protocol);
			display_digit = (display_digit + 1) % 4;  // 0~3 순환
		}
		break;

		case EMERGENCY_STOP:
			//현재 상태가 EMERGENCY_STOP이고 현재 상태와 이전상태가 다르면 rgb_mask 초기화
			if(current_state != previous_state) rgb_mask = 0x07fffe00;  // 0b111111111111111111000000000
				
			
			if(timer_delay_ms(&segment_display_timer, SEGMENT_DELAY)) {
				update_12bit_segment_mask(display_digit, num_digits[display_digit]);
				update__64bit_protocol();
				WordDataWrite(_64bit_protocol);
				display_digit = (display_digit + 1) % 4;  // 0~3 순환
			}
			break;
	}
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

void WordDataWrite(uint64_t data) { //인자로 64비트 패턴을 전달받음
	// 64번 반복하며 데이터 전달
	//UART_print64bitNumber (data);
	//UART_printString("\n");
	for(uint8_t i = 0 ; i < 64 ; i++) {
		if(data & 0x8000000000000000)  // data의 가장 왼쪽 비트 (63번 비트)가 1이면 해당 if문이 실행된다.
		PORTC |= (1 << SER); // PC4=SER 을 1로 set 하고, 나머지는 그대로 유지한다.
		else // data의 가장 왼쪽 비트 (63번 비트)가 0이면 실행된다.
		PORTC &= ~(1 << SER); // PC4=SER 을 0으로 clear 하고, 나머지는 그대로 유지한다.
		
		ShiftClock(); //1bit shift (즉 SER로 쏴준 데이터가 쉬프트 레지스터의 D F/F에 저장됨)
		data = data << 1; //인자로 전달받은 64bit data를 1bit 왼쪽으로 shift
	}
	
	LatchClock(); // 시프트 레지스터에 저장되어있던 16비트값이 병렬로 출력된다.
	/*///////////////디버깅용!!!!!!!!!!!!!!!!!!!!!!!!!!111
	if(timer_delay_ms(&debug_timer, 1000) && current_state == IDLE) {
		UART_print64bitNumber(_64bit_protocol);
		UART_printString("\n");
	}
	*/
}
