/*
 * timer_0.h
 *
 * Created: 2025-09-12 오후 10:35:17
 *  Author: User
 */ 
#include "common.h"

#ifndef TIMER_0_H_
#define TIMER_0_H_
//프로그램 시작 이후 경과시간

#include <avr/interrupt.h>
#include "UART.h"

//마이크로초당 클럭을 저장. 값은 16
#define CLOCKS_PER_MICRO ( F_CPU / 1000000L ) 
//매크로 함수. 클럭수를 인자로 전달하면, 해당 클럭이 총 몇 마이크로초인지 반환해준다. 시스템 클럭이 16Mhz 일때를 기준으로 만들어진 매크로함수이다.
#define CLOCKS_TO_MICROSECONDS(a) ( (a) / CLOCKS_PER_MICRO )  
// Timer0의 분주비가 8이다 -> 클럭 주파수가 1/8 배되므로, 초당 입력되는 펄스의 수가 기존(16Mhz기준)의 1/8배된다.
// 이는 카운터 입장에서 같은 수의 클럭을 셀 때, 시스템 클럭을 16Mhz로 그대로 쓰면서 펄스를 8배로 세는것과 동일하다.
// 따라서 단순히 256이 아니라 8*256을 전달해야 오버플로 인터럽트 발생까지
// 몇 마이크로초 걸리는지를 반환해준다.
#define MICROSECONDS_PER_TIMER0_OVERFLOW ( CLOCKS_TO_MICROSECONDS(8*256))
// 오버플로 인터럽트 발생까지 몇 밀리초 걸리는지 (밀리 단위 몫) -> 오버플로 인터럽트 발생까지 0.128ms 
#define MILLIS_INCREMENT_PER_OVERFLOW ( MICROSECONDS_PER_TIMER0_OVERFLOW / 1000 ) 
// 오버플로 인터럽트 발생까지 밀리초 단위 증가 제외하고 남은 마이크로초 단위 (마이크로단위의 아주작은 나머지)
#define MICROS_INCREMENT_PER_OVERFLOW ( MICROSECONDS_PER_TIMER0_OVERFLOW % 1000 ) 

//프로그램 시작 이후 경과시간을 외부에서 쓸 수 있도록 extern 으로 선언
extern volatile unsigned long timer0_millis; 
extern volatile int timer0_micros;
extern volatile unsigned long present_time;
extern volatile unsigned long previous_time;

typedef struct {
    uint16_t is_init_done; // 첫 번째 호출인지 여부확인용 플래그변수 (C99 표준에서는 초기값 설정 불가)
    unsigned long ms_time; // ms 시간 저장할 변수
} timer_ms;

typedef struct {
    uint16_t is_init_done; // 첫 번째 호출인지 여부확인용 플래그변수 
    unsigned long s_time; // s 시간 저장할 변수
} timer_s;

// 함수 프로토타입
void timer0_init();
void timer0_end();
unsigned long millis();
uint16_t secs();  // secs() 함수 선언 추가
uint16_t timer_delay_ms(timer_ms *timer, uint16_t ms_interval);
uint16_t timer_delay_s(timer_s *timer, uint16_t s_interval);

#endif /* TIMER_0_H_ */