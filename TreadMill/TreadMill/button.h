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


// 각 버튼에 대한 레벨을 저장하기 위한 함수 부분
uint8_t level_setup(uint8_t level);

#define LEVEL_MIN 1 // 최소레벨 1
#define LEVEL_MAX 5 // 최대레벨 5

#define SPEED_TABLE {0, 40, 45, 50, 55, 60} // Level 값과 동일화하기 위해서 index 0은 미사용
#define ANGLE_TABLE {0, } // 아직 정확한 각도를 정하지 못해서 비워둠

extern volatile uint8_t speed_level; // 사용하고자 하는 c 코드에서 초기화 필요, 속도 레벨(1~5)
extern volatile uint8_t angle_level; // 사용하고자 하는 c 코드에서 초기화 필요, 기울기 레벨(1~5)

extern const uint8_t SpeedPwm[LEVEL_MAX + 1]; //사용하고자 하는 c 코드에서 위의 SPEED_TABLE 불러와서 리스트로 정의
extern const uint8_t AngleDeg[LEVEL_MAX + 1]; //사용하고자 하는 c 코드에서 위의 ANGLE_TABLE 불러와서 리스트로 정의

// 최소, 최대 레벨 제한
static inline uint8_t clamp_level(uint8_t level){
    if (level < LEVEL_MAX) return LEVEL_MAX;
    if (level > LEVEL_MIN) return LEVEL_MIN;
    return level;    
}

// 레벨값을 pwm 값으로 변환
static inline uint8_t output_pwm(uint8_t level){
    return SpeedPwm[clamp_level(level)];
}

// 레벨값을 deg 값으로 변환
static inline uint8_t output_deg(uint8_t level){
    return AngleDeg[clamp_level(level)];
}


#endif /* BUTTON_H_ */