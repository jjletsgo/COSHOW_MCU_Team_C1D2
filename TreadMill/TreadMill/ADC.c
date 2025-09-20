
#include "ADC.h"

void ADC_init(void) {
	ADMUX |= 0x40; //0b01000000, 6번 비트를 1로 set한다. -> ADC의 기준 전압을 AVcc(5V)로 설정한다. 추가적으로 ADLAR 건들지않았으므로 현재 우측정렬인 상태
	
	ADCSRA |= 0x07; //0b00000111, 2~0 번 비트를 1로 set 한다 -> 시스템 클럭 분주기의 division factor를 128 로 설정한다 ->
	// ADC Clock Frequency는 16Mhz/128=125khz 가 된다. (최대 해상도로 사용하려면 100kHz~200kHz로 클럭 주파수 설정해야한다고
	// 데이터시트에 나와있음.
	ADCSRA |= (1 << ADEN); // ADEN을 1로 set 한다. -> ADC를 활성화한다.
	ADCSRA |= (1 << ADATE); // ADATE를 1로 set 한다. -> ADC 를 FREE RUNNIGN 모드(Auto triggering)로 돌린다. -> 선택된 신호의 positive edge마다 자동 AD변환 실행
	
	
	// ADMUX = ((ADMUX & 0xE0) | channel); // 0XE0은 왜 해주는건지 잘 모르겠다. channel로 ADC에 연결할 입력 ADC 채널을 선택한다.
	ADCSRA |= (1 << ADSC); //ADSC 비트를 1로 set 한다. 현재 Free Running mode이므로 지금부터 자동 AD 변환을 시작한다.
}

//channel로 0~5 입력해주면 ADC MUX가 해당 ADC 채널과 연결됨
void ADC_select_channel (unsigned char channel) {
	ADMUX |= channel;
	_delay_ms(3); //채널 변화 후 안정화를 위한 delay
}

//while문 내부에서 지속적으로 읽을 때 쓰는 함수
int read_ADC(void) {
	
	while(!(ADCSRA & (1 << ADIF))); // ADCSRA의 ADIF가 1되면 전체가 0이 되면서 while문을 탈출한다.
	// (ADIF 비트는 ADC 변환이 끝나고 데이터 레지스터가 업데이트되면 1로 set된다)
	return ADC; //ADC 데이터 레지스터를 반환한다.
}




