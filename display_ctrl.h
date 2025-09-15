/*
 * display_ctrl.h
 *
 * Created: 2025-09-09 오전 11:04:15
 *  Author: AFHGF
 */ 

#ifndef DISPLAY_CTRL_H
#define DISPLAY_CTRL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#define F_CPU          16000000UL
#define TWI_FREQ       100000UL
#define I2C_PRESCALER  1

#define LCD_I2C_ADDR   0x27
#define LCD_RS         (1<<0)
#define LCD_RW         (1<<1)
#define LCD_EN         (1<<2)
#define LCD_BL         (1<<3)

static void i2c_init(void);
static void i2c_start(uint8_t addr7);
static void i2c_write(uint8_t data);
static void i2c_stop(void);

static void lcd_set_ctrl(uint8_t data);
static void lcd_pulse_enable(uint8_t data);
static void lcd_write4(uint8_t nibble, uint8_t mode_rs);
static void lcd_send(uint8_t value, uint8_t mode_rs);
static void lcd_command(uint8_t cmd);
static void lcd_backlight_on(bool on);
static void lcd_write_char(uint8_t c);
static void lcd_clear(void);
static void lcd_set_cursor(uint8_t col, uint8_t row);
static void lcd_create_char(uint8_t location, const uint8_t pattern[8]);
static void lcd_init(void);

static void lcd_print_str(const char *str);
static void lcd_print_int(uint16_t value);
static void lcd_print_float(float value);

#endif // DISPLAY_CTRL_H
