/*
 * button.h
 *
 * Created: 2025-09-15 오후 6:54:13
 *  Author: User
 */ 

#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>
// 디바운스 설정 (연속으로 같은 값이 몇 번 읽혀야 안정된 값으로 인정할지)
#define DEBOUNCE_COUNT 2

// 버튼별 ADC 임계값 (실제 회로에 맞게 조정 필요)
#define BUTTON_SPEED_UP_MAX    45
#define BUTTON_SPEED_DOWN_MAX  150
#define BUTTON_ANGLE_UP_MAX    290
#define BUTTON_ANGLE_DOWN_MAX  450
#define BUTTON_ON_OFF_MAX      600

// 버튼 타입 정의
typedef enum {
    BUTTON_NONE = 0,
    BUTTON_SPEED_UP,
    BUTTON_SPEED_DOWN,
    BUTTON_ANGLE_UP,
    BUTTON_ANGLE_DOWN,
    BUTTON_ON_OFF
} Button_t;

extern Button_t debug_button;
// 함수 프로토타입
Button_t Button_ADC_getRaw(uint16_t ADC_val);     // Raw ADC 값 -> 버튼 상태
Button_t Button_ADC_getPressed(uint16_t ADC_val);     // 디바운싱된 버튼 눌림 감지
Button_t Button_ADC_getDebounced(uint16_t ADC_val);
void Button_Init(void);                           // 초기화

#endif /* BUTTON_H_ */