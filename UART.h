/*
 * UART.h
 *
 * Created: 2025-09-07 오전 1:31:16
 *  Author: User
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

void UART_INIT(void); // UART 초기화
unsigned char UART_receive(void); // 수신 버퍼 확인해서 수신 버퍼의 내용 반환하는 함수
void UART_transmit(unsigned char data); // 송신 버퍼에 실제로 8비트 데이터를 쓰는 함수
void UART_printString(char *str);
void UART_print8bitNumber(uint8_t no);
void UART_print16bitNumber(uint16_t no);
void UART_print32bitNumber(uint32_t no);


#endif /* UART_H_ */