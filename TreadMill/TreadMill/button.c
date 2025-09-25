/*
 * button.c
 *
 * Created: 2025-09-15 오후 6:54:13
 *  Author: User
 */ 
#include "button.h"



// 정적 변수들 (디바운싱을 위한 상태 저장)
static Button_t lastRead = BUTTON_NONE;
static Button_t lastStable = BUTTON_NONE;  // 누락된 초기화 추가
static uint8_t count = 0;                  // 누락된 카운터 변수 추가



// =============================
// 버튼 상태 판별 (Raw ADC 값)
// =============================
Button_t Button_ADC_getRaw(uint16_t ADC_val) {
    if (ADC_val < BUTTON_SPEED_UP_MAX) return BUTTON_SPEED_UP;
    else if (ADC_val < BUTTON_SPEED_DOWN_MAX) return BUTTON_SPEED_DOWN;
    else if (ADC_val < BUTTON_ANGLE_UP_MAX) return BUTTON_ANGLE_UP;
    else if (ADC_val < BUTTON_ANGLE_DOWN_MAX) return BUTTON_ANGLE_DOWN;
    else if (ADC_val < BUTTON_PROGRAM_A_MAX) return BUTTON_PROGRAM_A;
	else if (ADC_val < BUTTON_ON_OFF_MAX) return BUTTON_ON_OFF ;
    else return BUTTON_NONE;
}

// =============================
// 디바운스 처리된 버튼 판별
// =============================
Button_t Button_ADC_getDebounced(uint16_t ADC_val) {
    Button_t current = Button_ADC_getRaw(ADC_val);
    
    // 현재 읽은 값이 이전 값과 같으면 카운터 증가
    if (current == lastRead) {
        if (count < DEBOUNCE_COUNT) {
            count++;
        }
    } else {
        // 값이 변경되면 카운터 리셋
        count = 0;
    }
    
    // 현재 값을 저장
    lastRead = current;
     
    // 충분한 시간 동안 같은 값이 유지되면 안정된 값으로 업데이트
    if (count >= DEBOUNCE_COUNT) {
        lastStable = current;
		
    }
    
    return lastStable;
}

// =============================
// 버튼 눌림 감지 (Edge Detection)
// =============================
Button_t Button_ADC_getPressed(uint16_t ADC_val) {
    static Button_t previousStable = BUTTON_NONE;
    
    Button_t currentStable = Button_ADC_getDebounced(ADC_val);
    
    // 이전 상태가 BUTTON_NONE이고 현재 상태가 버튼이면 눌림 감지
    if (previousStable == BUTTON_NONE && currentStable != BUTTON_NONE) {
        previousStable = currentStable;
        return currentStable;  // 버튼 눌림 이벤트 반환
    }
    
    // 이전 상태를 업데이트
    previousStable = currentStable;
    
    return BUTTON_NONE;  // 눌림 이벤트가 없으면 BUTTON_NONE 반환
}

// =============================
// 디바운싱 초기화 (필요시 호출)
// =============================
void Button_Init(void) {
    lastRead = BUTTON_NONE;
    lastStable = BUTTON_NONE;
    count = 0;
}
// ??