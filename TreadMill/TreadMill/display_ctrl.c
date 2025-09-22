/*
 * display_ctrl.c
 *
 * Created: 2025-09-17 오후 11:29:50
 *  Author: AFHGF
 */ 

#include "common.h"
#include "display_ctrl.h"
#include "button.h"
#include "UART.h"
#include "timer_0.h"
#include <util/delay.h>
#include "encoder.h"
#define DIAMETER 2

static uint8_t lcd_backlight = LCD_BL;
timer_s encoder_timer;
volatile uint8_t count = 0;
volatile uint16_t dist = 0;
static uint8_t vel = 1;
static uint8_t deg = 1;
static uint8_t col_v = 2, col_d = 10;
//static bool status = false;

static STATE last_state = -1;
static timer_s init_msg_timer;
static bool init_msg = false;

void i2c_init(void) {
   TWSR = 0x00; // prescaler = 1
   TWBR = (uint8_t)((F_CPU / TWI_FREQ - 16) / 2);
}

void i2c_start(uint8_t addr7) {
   TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
   while (!(TWCR & (1<<TWINT)));
   TWDR = (addr7 << 1) | 0; // SLA+W
   TWCR = (1<<TWINT) | (1<<TWEN);
   while (!(TWCR & (1<<TWINT)));
}

void i2c_write(uint8_t data) {
   TWDR = data;
   TWCR = (1<<TWINT) | (1<<TWEN);
   while (!(TWCR & (1<<TWINT)));
}

void i2c_stop(void) {
   TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

/* LCD 제어 함수 */
void lcd_set_ctrl(uint8_t data) {
   i2c_start(LCD_I2C_ADDR);
   i2c_write(data | lcd_backlight);
   i2c_stop();
}

void lcd_pulse_enable(uint8_t data) {
   i2c_start(LCD_I2C_ADDR);
   i2c_write((data | LCD_EN) | lcd_backlight);
   //_delay_us(1);
   
   i2c_write((data & ~LCD_EN) | lcd_backlight);
   i2c_stop();
   //_delay_us(50);
}

void lcd_write4(uint8_t nibble, uint8_t mode_rs) {
   uint8_t out = 0;
   out |= (nibble & 0xF0);
   if (mode_rs) out |= LCD_RS;
   lcd_pulse_enable(out);
}

void lcd_send(uint8_t value, uint8_t mode_rs) {
   lcd_write4(value & 0xF0, mode_rs);           // 상위 4bit
   lcd_write4((value << 4) & 0xF0, mode_rs);    // 하위 4bit
}

void lcd_command(uint8_t cmd) {
   lcd_send(cmd, 0);
   
   /*
   if (cmd == 0x01 || cmd == 0x02) {
      _delay_ms(2);
   }
   */
}


void lcd_backlight_on(bool on) {
   if (on) lcd_backlight = LCD_BL;
   else lcd_backlight = 0;
   lcd_set_ctrl(0);
}

void lcd_write_char(uint8_t c) {
   lcd_send(c, 1);
}

void lcd_clear(void) {
   static uint8_t i = 0;
   lcd_set_cursor(0, 0); // 커서를 첫 번째 줄로 이동
   for(i = 0; i < 16; i++){
      lcd_write_char(' ');
   }
   lcd_set_cursor(0, 1); // 커서를 두 번째 줄로 이동
   for(i = 0; i < 16; i++){
      lcd_write_char(' ');
   }
   lcd_set_cursor(0, 0); // 커서를 다시 원위치
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
   static const uint8_t line_offset[2] = {0x00, 0x40};
   lcd_command(0x80 | (col + line_offset[row]));
}

void lcd_create_char(uint8_t location, const uint8_t pattern[8]) {
   location &= 0x7;
   lcd_command(0x40 | (location << 3));
   for (uint8_t i = 0; i < 8; i++) {
      lcd_write_char(pattern[i]);
   }
}

void lcd_init(void) {
   _delay_ms(50);
   lcd_backlight_on(true);
   lcd_write4(0x30, 0);
   _delay_ms(5);
   lcd_write4(0x30, 0);
   _delay_us(150);
   lcd_write4(0x30, 0);
   _delay_us(150);
   lcd_write4(0x20, 0);    // 4bit mode
   _delay_us(50);
   lcd_command(0x28);      // 4bit, 2line, 5x8
   lcd_command(0x08);      // display off
   //lcd_clear();
   lcd_command(0x06);      // entry mode set
   lcd_command(0x0C);      // display on
   lcd_clear();
}

void lcd_print_str(const char *str) {
   while (*str) {
      lcd_write_char((uint8_t)*str++);      // 문자열 시작 주소 -> NULL write
   }
}

void lcd_print_int(uint16_t value) {
   char buffer[6];
   char *ptr = buffer + sizeof(buffer) - 1;
   uint16_t x = value;
   
   *ptr-- = '\0';
   
   if (x == 0){
      *ptr-- = '0';
   }
   else {
      while (x > 0) {      // 숫자를 문자로 변환해 버퍼에 역순으로 저장
         *ptr-- = (char)('0' + (x % 10));
         x /= 10;
      }
   }
   ptr++;
   lcd_print_str(ptr);
}


void lcd_print_float(float value) {
   uint16_t whole = (uint16_t)value;
   uint8_t frac  = (uint8_t)((value - (float)whole) * 10.0f);
 
   
   lcd_print_int(whole);
   lcd_print_str(".");
   lcd_print_int(frac);
}

void lcd_speed_up(void)
{   
   if (current_state != RUNNING) return;
   if (vel < LEVEL_MAX){
      vel++;
      col_v ++;
      lcd_set_cursor(col_v, 0);
      lcd_write_char(0);
   }
}

void lcd_speed_down(void){
   if (current_state != RUNNING) return;
   if (vel > LEVEL_MIN){
      vel --;
      lcd_set_cursor(col_v, 0);
      lcd_write_char(' ');
      col_v --;
   }
}

void lcd_angle_up(void){
   if (current_state != RUNNING) return;
   if (deg < LEVEL_MAX){
      deg ++;
      col_d ++;
      lcd_set_cursor(col_d, 0);
      lcd_write_char(0);
   }
}

void lcd_angle_down(void){
   if (current_state != RUNNING) return;
   if (deg > LEVEL_MIN ){
      deg --;
      lcd_set_cursor(col_d, 0);
      lcd_write_char(' ');
      col_d --;
   }
}

/*
void lcd_button_on(void){
   if (status){
      status = false;
      vel = 1, deg = 1;
      col_v = 2, col_d = 10;
     
      dist = 0;
      //cal = 0;
      
   }
   else{
      status = true;
      encoder_timer.is_init_done = 0;
   }
}
*/

void lcd_print_level(void){
   lcd_set_cursor(0,0);
   lcd_print_str("S:");
   lcd_set_cursor(8,0);
   lcd_print_str("A:");

   lcd_set_cursor(2,0);
   lcd_write_char(0); // 속도 레벨 1칸 채우기
   lcd_set_cursor(10,0);
   lcd_write_char(0); // 각도 레벨 1칸 채우기
}

void lcd_print_info(void){
	static uint16_t cal = 0;

   if (current_state != RUNNING) {
      return;
   }
   
   if (timer_delay_s(&encoder_timer, 1)){
      // 1. 값 계산
      count = encoder_read();
      UART_print8bitNumber(count);
      dist += (uint16_t)(count * 2 * DIAMETER * 3) / 100;		// 미터 단위
      cal = (70 * dist)/1000;

      // 2. 화면 업데이트
      lcd_set_cursor(0, 1);
      lcd_print_str("D:      "); 
      lcd_set_cursor(2, 1);
      lcd_print_int(dist);

      lcd_set_cursor(8, 1);
      lcd_print_str("CAL:    ");		// kcal 단위
      lcd_set_cursor(12, 1);
      lcd_print_int(cal);
   }
}

void lcd_state_change(void)
{
   if (current_state != last_state) {
      
      init_msg = false; 

      switch (current_state) {
         case (INIT):
         case (IDLE):
		 vel = 1, deg = 1;
		 col_v = 2, col_d = 10;
		 dist = 0;
         lcd_clear();
         lcd_print_str("Initializing...");
         init_msg_timer.is_init_done = 0; // 1초 타이머 리셋
         init_msg = true;   // 메시지 표시 중 플래그 활성화
         break;
         
         case RUNNING:
         lcd_clear();
         lcd_print_level();
		 encoder_timer.is_init_done = 0;
         break;
         
         default:
         lcd_clear();
         break;
      }
      // 현재 상태를 last_state에 저장. 로직 반복 실행 방지.
      last_state = current_state;
   }

   if (init_msg) {
      if (timer_delay_s(&init_msg_timer, 2)) {
         lcd_clear();
         init_msg = false;
      }
   }
}