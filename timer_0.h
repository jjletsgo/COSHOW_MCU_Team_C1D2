/*
 * timer_0.h
 *
 * Created: 2025-09-12 오후 10:35:17
 *  Author: User
 */ 

#ifndef F_CPU 
#define F_CPU 16000000L
#endif

#ifndef TIMER_1_H_
#define TIMER_1_H_
//프로그램 시작 이후 경과시간

#include <avr/io.h>
#include <avr/interrupt.h>

#define SEGMENT_DELAY 5
//마이크로초당 클럭을 저장. 값은 16
#define CLOCKS_PER_MICRO ( F_CPU / 1000000L ) 
//매크로 함수. 클럭수를 인자로 전달하면, 해당 클럭이 총 몇 마이크로초인지 반환해준다. 시스템 클럭이 16Mhz 일때를 기준으로 만들어진 매크로함수이다.
#define CLOCKS_TO_MICROSECONDS(a) ( (a) / CLOCKS_PER_MICRO )  
// Timer0의 분주비가 64이다 -> 클럭 주파수가 1/64 배되므로, 입력되는 펄스의 수가 기존(16Mhz기준)의 1/64배된다.
// 이는 카운터 입장에서 시스템 클럭을 16Mhz로 그대로 쓰면서 펄스를 64배 더 세는것과 동일한 효과이다.
// 따라서 단순히 256이 아니라 256*64를 전달해야 오버플로 인터럽트 발생까지
// 몇 마이크로초 걸리는지를 반환해준다.
#define MICROSECONDS_PER_TIMER0_OVERFLOW ( CLOCKS_TO_MICROSECONDS(64*256))
// 오버플로 인터럽트 발생까지 몇 밀리초 걸리는지 (밀리 단위 몫 느김)
#define MILLIS_INCREMENT_PER_OVERFLOW ( MICROSECONDS_PER_TIMER0_OVERFLOW / 1000 ) 
// 오버플로 인터럽트 발생까지 밀리초 단위 증가 제외하고 남은 마이크로초 단위 (마이크로단위의 아주작은 나머지 느낌)
#define MICROS_INCREMENT_PER_OVERFLOW ( MICROSECONDS_PER_TIMER0_OVERFLOW % 1000 ) 
//프로그램 시작 이후 경과시간을 외부에서 쓸 수 있도록 extern 으로 선언
extern volatile unsigned long timer0_millis = 0;
extern volatile int timer0_micros = 0;
// 함수 프로토타입
void timer0_init();
unsigned long millis();
#endif /* TIMER_0_H_ */