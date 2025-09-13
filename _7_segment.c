/*
 * _7_segment.c
 *
 * Created: 2025-09-12 오후 11:36:43
 *  Author: User
 */ 
#include "_7_segment.h"

//비트패턴
uint8_t numbers[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09}; //숫자패턴 0~9를 미리 배열에 미리 저장해둠
	
uint16_t make_12bit_protocol {
	


	return protocol_12bit;
}

void Init_74595(void) {
	// data pin (SER): PC4
	// shift clock pin (SCK) : PC5
	// latch clock pin (RCK) : PC6
	
	DDRC |= (1 << SER) | (1 << SCK) | (1 << RCK) //SER,SCK,RCK와 연결된 atmeag328p의 핀을 출력으로 설정
}

// SCK로 펄스하나 쏴주는 함수. 평소에는 LOW 상태 유지하므로 -> SCK로 positive edge 하나 쏴주는 것 -> 1bit shift 된다.
void ShiftClock(void) {
	PORTC |= (1 << SCK); // PORTC의 5번 비트 (PC5=SCK) 를 HIGH로 설정
	PORTC &= ~(1 << SCK); // PORTC의 5번 비트 (PC5=SCK) 를 LOW로 설정하고 , 나머지는 유지
}

// RCK로 펄스하나 쏴주는 함수. 평소에는 LOW 상태 유지하므로 -> RCK로 positive edge 하나 쏴주는 것 -> G가 GND(=LOW)에 연결되어있으므로
// 이 함수 쓰면 시프트 레지스터에 저장되있던 8비트 값이 병렬 출력된다.
void LatchClock(void)
{
	PORTC |= (1 << RCK); // PORTC의 6번 비트 (PC6=RCK) 를 HIGH로 설정
	PORTC &= ~(1 << RCK); // PORTC의 6번 비트 (PC6=RCK) 를 LOW로 설정
}

void WordDataWrite(uint16_t data) { //인자로 16비트 패턴을 전달받음
	// 12번 반복하며 데이터 전달
	for(uint8_t i = 0 ; i < 12 ; i++) {
		if(data & 0x8000)  // data의 가장 왼쪽 비트 (15번 비트)가 1이면 해당 if문이 실행된다.
		PORTC |= (1 << SER); // PC4=SER 을 1로 set 하고, 나머지는 그대로 유지한다.
		else // data의 가장 왼쪽 비트 (15번 비트)가 0이면 실행된다.
		PORTC &= ~(1 << SER); // PC4=SER 을 0으로 clear 하고, 나머지는 그대로 유지한다.
		
		ShiftClock(); //1bit shift (즉 SER로 쏴준 데이터가 쉬프트 레지스터의 D F/F에 저장됨)
		data = data << 1; //인자로 전달받은 16bit data를 1bit 왼쪽으로 shift
	}
	
	LatchClock(); // 시프트 레지스터에 저장되어있던 8비트값이 병렬로 출력된다.
}