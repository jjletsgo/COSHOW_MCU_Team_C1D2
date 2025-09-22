/*
 * load_cell.h
 *
 * Created: 2025-09-15 오후 8:07:22
 *  Author: AFHGF
 */ 
#ifndef LOAD_CELL_H
#define LOAD_CELL_H

#include "common.h"
#include <stdbool.h>

// HX711 핀 정의
#define LOAD_CELL_DT_PIN 3    // PC3 : Input
#define LOAD_CELL_DT_DDR DDRC
#define LOAD_CELL_DT_IPA PINC

#define LOAD_CELL_SCK_PIN 5     // PB5 : Output
#define LOAD_CELL_SCK_PORT PORTB
#define LOAD_CELL_SCK_DDR  DDRB

// 보정 계수
#define WEIGHT_FACTOR 7050.0
#define WEIGHT_SAMPLE_COUNT 10

// 함수 프로토타입
void load_cell_init(void);
int32_t load_cell_read(void);
float load_cell_convert(int32_t raw);
bool load_cell_status(int32_t offset);
bool load_cell_status_check(int32_t offset);
#endif // LOAD_CELL_H
