/*
 * UART.C
 *
 * Created: 2025-09-07 오전 1:29:13
 *  Author: User
 */ 

#include "UART.h"

// UART 초기화하는 함수
void UART_INIT(void) { 
	UCSR0A |= 0x01 << 1;	// 2배속 모드로 설정(U2X0 비트)     
	
	UBRR0H = 0x00;  //보드레이트 설정에 사용하는 레지스터. 207은 2^8 이하이니 12비트중 8비트만 이용해서 저장할 수 있다. 따라서 UBRR0H는 0으로, UBRR0L을 207로 초기화한다.
	UBRR0L = 207;
	
	// 비동기, 8비트 데이터, 패리티 없음, 1비트 정지 비트 모드
	
	UCSR0C |= 0x06; //0b00000110 , 문자 크기 설정 비트이다. 문자크기를 8비트로 설정. 즉 UART를 8비트 데이터를 송수신하도록 설정하는 것이다.
	
	
	UCSR0B |= 0X01 << 4;	//UCSR0B |= _BV(RXEN0);   수신기 활성화(수신 기능 필요시 주석해제하기)
	
	UCSR0B |= 0X01 << 3; //UCSR0B |= _BV(TXEN0);    송신기 활성화
}

// 수신 버퍼 확인해서 수신 버퍼의 내용 반환하는 함수
unsigned char UART_receive(void)
{
	
	while( !(UCSR0A & (0x01<<7)));    //while( !(UCSR0A & (1<<RXC0))); // unread된 데이터가 존재하면 while문을 탈출함. 즉, 데이터 수신을 대기하는 함수임
	return UDR0; // 수신 버퍼 (RXB) 의 내용 반환
}

// 전송 버퍼에 전송할 내용 쓰는 함수
void UART_transmit(unsigned char data) //char를 unsigned char로 받는다. 아스키코드는 0~127이라 unsigned char (표현범위 0~255)에 넣을 수 있다.
//그래서 char를 usngiend char로 받아도 아무 관계없다.
{
	
	while( !(UCSR0A & (0x01<<5)));  // while( !(UCSR0A & (1<<UDRE0))); // 전송 버퍼가 비어지고, 전송 버퍼가 쓰일 준비가 되면 while문을 탈출한다. 
    // 즉, 전송 버퍼가 다음 통신을위해 준비될떄까지 대기하는 함수임
	UDR0 = data; // 전송 버퍼 (TXB)에 내용을 쓴다.
	
}

// 문자열(char 배열) 송신하는 함수
// 입력: 송신할 문자열(char 배열)
void UART_printString(char *str) { //배열은 배열의 첫 번쨰 요소의 주소를 반환한다. 따라서 인자로 배열을 넘겨받기위해선 함수 원형에서 매개변수를 포인터로 선언해야한다.
	// C언어에서는 문자열을 char 배열로 사용하므로 char 배열을 전달받기위해 char 타입의 포인터 변수를 선언한다.
	for(int i = 0; str[i];i++)  //char 배열은 끝에 항상 '\0' (아스키코드값이 0) 이 존재함을 이용해 반복문 종료
		UART_transmit(str[i]);
}

// 숫자 송신하는 함수
//입력: 0~255 사이의 숫자
void UART_print8bitNumber (uint8_t no) {
	char numString[4] = "0"; //uint8_t는 0~255을 표현할 수 있다. 즉, 3자릿수까지 표현가능하다. 따라서 nullpointer를 포함하여 4칸짜리 char 배열을 만들고 초기화. ['0', '\0', 쓰레기값, 쓰레기값]
	int i, index = 0;
	
	if(no > 0) { // 전달받은 수가 0보다 크면
		for(i=0; no != 0;  i++) {
			numString[i] = no % 10 + '0'; //char배열의 각 요소의 아스키코드 값을 numString[]에 저장한다. '0'은 아스키코드 변환시 바이어스 값으로 사용된다.
			no = no / 10; // 체크하는 no의 자릿수를 한 칸 높여준다. 낮은 자릿수의 아스키코드는 방금 numString[]에 저장했으므로
		}
		numString[i] = '\0'; //numString[]의 마지막 요소에 null pointer ('\0')을 넣어준다.
		index = i - 1;
		
	}
	
	for(i = index; i >= 0; i--) UART_transmit(numString[i]);
}

// 숫자 송신하는 함수
//입력: 0~65535 사이의 숫자
void UART_print16bitNumber (uint16_t no) {
	char numString[6] = "0";  //uint16_t는 0~65535 를 표현할 수 있다. 즉, 5자릿수까지 표현가능하다.  따라서 nullpointer를 포함하여 6칸짜리 char 배열을 만들고 초기화.
	int i, index = 0;
	
	if(no > 0) {
		for(i=0; no != 0;  i++) {
			numString[i] = no % 10 + '0';
			no = no / 10;
		}
		numString[i] = '\0';
		index = i - 1;
		
	}
	
	for(i = index; i >= 0; i--) UART_transmit(numString[i]);
}

// 숫자 송신하는 함수
//입력: 0~429496729 사이의 숫자
void UART_print32bitNumber (uint32_t no) {
	char numString[11] = "0";  //uint32_t는 0~429496729 를 표현할 수 있다. 즉, 10자릿수까지 표현가능하다.   따라서 nullpointer를 포함하여 11칸짜리 char 배열을 만들고 초기화.
	int i, index = 0;
	
	if(no > 0) {
		for(i=0; no != 0;  i++) {
			numString[i] = no % 10 + '0';
			no = no / 10;
		}
		numString[i] = '\0';
		index = i - 1;
		
	}
	
	for(i = index; i >= 0; i--) UART_transmit(numString[i]);
}