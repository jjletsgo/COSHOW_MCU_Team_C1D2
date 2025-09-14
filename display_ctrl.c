/*
 * display_ctrl.c
 *
 * Created: 2025-09-09 오전 11:03:34
 * Author : AFHGF
 */ 

#include "display_ctrl.h"
#include <avr/io.h>
#include <util/delay.h>

static uint8_t lcd_backlight = LCD_BL;

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
	_delay_us(1);
	i2c_write((data & ~LCD_EN) | lcd_backlight);
	i2c_stop();
	_delay_us(50);
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
	if (cmd == 0x01 || cmd == 0x02) {
		_delay_ms(2);
	}
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
	lcd_command(0x01);
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
	lcd_clear();
	lcd_command(0x06);      // entry mode set
	lcd_command(0x0C);      // display on
}

void lcd_print_str(const char *s) {
	while (*s) {
		lcd_write_char((uint8_t)*s++);
	}
}

void lcd_print_int(int32_t v) {
	char buf[12];
	char *p = buf + sizeof(buf);
	bool neg = (v < 0);
	uint32_t n = neg ? (uint32_t)(-v) : (uint32_t)v;
	*--p = '\0';
	if (n == 0) {
		*--p = '0';
		} else {
		while (n > 0) {
			*--p = (char)('0' + (n % 10));
			n /= 10;
		}
	}
	if (neg) *--p = '-';
	lcd_print_str(p);
}

void lcd_print_float(float x) {
	int32_t whole = (int32_t)x;
	int32_t frac  = (int32_t)((x - (float)whole) * 10.0f);
	if (x < 0 && whole == 0 && frac != 0) {
		lcd_print_str("-");
	}
	lcd_print_int(whole);
	lcd_print_str(".");
	if (frac < 0) frac = -frac;
	lcd_print_int(frac);
}



