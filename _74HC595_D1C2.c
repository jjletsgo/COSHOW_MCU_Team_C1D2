/*
 * _74hc595_d1c2.c
 *
 * Created: 2025-09-12 오후 10:01:15
 *  Author: User
 */ 
#include "_74hc595_d1c2.h"

void Init_74595(void) {
	// data pin (SER): PBC4
    // shift clock pin (SCK) : PC5
	// latch clock pin (RCK) : PC6
	
	DDRC |= 0b01110000; //PC4,5,6 을 출력으로 설정
}

// SCK로 펄스하나 쏴주는 함수. 평소에는 LOW 상태 유지하므로 -> SCK로 positive edge 하나 쏴주는 것 -> 1bit shift 된다.
void ShiftClock(void) {
	PORTC |= 0b00100000; // PORTC의 5번 비트 (PC5=SCK) 를 HIGH로 설정
	PORTC &= 0b11011111; // PORTC의 5번 비트 (PC5=SCK) 를 LOW로 설정하고 , 나머지는 유지
}

// RCK로 펄스하나 쏴주는 함수. 평소에는 LOW 상태 유지하므로 -> RCK로 positive edge 하나 쏴주는 것 -> G가 GND(=LOW)에 연결되어있으므로 
// 이 함수 쓰면 시프트 레지스터에 저장되있던 8비트 값이 병렬 출력된다.
void LatchClock(void)
{
	PORTC |= 0b01000000; // PORTC의 6번 비트 (PC6=RCK) 를 HIGH로 설정
	PORTC &= 0b10111111; // PORTC의 6번 비트 (PC6=RCK) 를 LOW로 설정
}

void WordDataWrite(uint16_t data) { //인자로 16비트 패턴을 전달받음
	for(uint8_t i = 0 ; i < 16 ; i++) {
		if(data & 0x8000)  // data의 가장 왼쪽 비트 (15번 비트)가 1이면 해당 if문이 실행된다.
			PORTC |= 0b00010000; // PC4=SER 을 1로 set 하고, 나머지는 그대로 유지한다.
		else // data의 가장 왼쪽 비트 (15번 비트)가 0이면 실행된다.
			PORTC &= 0b11101111; // PC4=SER 을 0으로 clear 하고, 나머지는 그대로 유지한다.
			
		ShiftClock(); //1bit shift (즉 SER로 쏴준 데이터가 쉬프트 레지스터의 D F/F에 저장됨)
		data = data << 1; //인자로 전달받은 16bit data를 1bit 왼쪽으로 shift
	}
	
	LatchClock(); // 시프트 레지스터에 저장되어있던 8비트값이 병렬로 출력된다.
}

/*
int main(void) {
	uint8_t index = 0; 
	
	Init_74595();
	
	while(1) {
		uint16_t pattern = 0x0001 << index;  
		index = (index + 1) % 16; // 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,,,,이 반복되며 index에 저장된다.
		
		WordDataWrite(pattern); //해당 함수에 인자로 16비트 패턴을 넣어주면, 인자로 넣어준 16bit 데이터를 cascade 된 74HC595 2개에 저장시킨다.
		
		_delay_ms(200); //200ms 딜레이
	}
*/
