/*
 * display_ctrl.h
 *
 * Created: 2025-09-09 오전 11:04:15
 *  Author: AFHGF
 */ 

#ifndef DISPLAY_CTRL_H
#define DISPLAY_CTRL_H

#include "common.h"
#include "button.h"
#include "encoder.h"

#include <stdbool.h>

#define TWI_FREQ       100000UL
#define I2C_PRESCALER  1

#define LCD_I2C_ADDR   0x27
#define LCD_RS         (1<<0)
#define LCD_RW         (1<<1)
#define LCD_EN         (1<<2)
#define LCD_BL         (1<<3)

void i2c_init(void);
void i2c_start(uint8_t addr7);
void i2c_write(uint8_t data);
void i2c_stop(void);

void lcd_set_ctrl(uint8_t data);
void lcd_pulse_enable(uint8_t data);
void lcd_write4(uint8_t nibble, uint8_t mode_rs);
void lcd_send(uint8_t value, uint8_t mode_rs);
void lcd_command(uint8_t cmd);
void lcd_backlight_on(bool on);
void lcd_write_char(uint8_t c);
void lcd_clear(void);

void lcd_set_cursor(uint8_t col, uint8_t row);
void lcd_create_char(uint8_t location, const uint8_t pattern[8]);
void lcd_init(void);

void lcd_print_str(const char *str);
void lcd_print_int(uint16_t value);
void lcd_print_float(float value);


void lcd_speed_up(void);
void lcd_speed_down(void);
void lcd_angle_up(void);
void lcd_angle_down(void);
void lcd_button_on(void);

void lcd_print_level(void);
void lcd_print_info (void);
void lcd_state_change(void);
#endif // DISPLAY_CTRL_H
