#ifndef MOTOR_DC_H_
#define MOTOR_DC_H_

#include <stdint.h>



void motor_dc_init(void);
void motor_dc_start(uint8_t duty);
void motor_dc_stop(void);
void motor_dc_control(Button_t pressed);
void motor_setup(void);
uint8_t level_setup(uint8_t level);

#define LEVEL_MIN 1
#define LEVEL_MAX 5

#define SPEED_TABLE {0, 40, 45, 50, 55, 60} // Level 값과 동일화하기 위해서 index 0은 미사용
#define ANGLE_TABLE {0, } // 아직 정확한 각도를 정하지 못해서 비워둠

extern volatile uint8_t speed_level;
extern volatile uint8_t angle_level;

extern const uint8_t SpeedPwm[LEVEL_MAX + 1];
extern const uint8_t AngleDeg[LEVEL_MAX + 1];

static inline uint8_t clamp_level(uint8_t level){
    if (level < LEVEL_MAX) return LEVEL_MAX;
    if (level > LEVEL_MIN) return LEVEL_MIN;
    return level;    
}

static inline uint8_t output_pwm(uint8_t level){
    return SpeedPwm[clamp_level(level)];
}

static inline uint8_t output_deg(uint8_t level){
    return AngleDeg[clamp_level(level)];
}

#endif
