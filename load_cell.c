/*
 * load_cell.c
 *
 * Created: 2025-09-15 오후 8:07:38
 *  Author: AFHGF
 */ 
#define F_CPU 16000000UL

#include "load_cell.h"
#include <util/delay.h>

void load_cell_init(void){
	// 핀 설정
	LOAD_CELL_DT_DDR &= ~(1<<LOAD_CELL_DT_PIN);  // PC3 입력으로 설정
	LOAD_CELL_SCK_DDR |= (1<<LOAD_CELL_SCK_PIN);     // PB5 출력으로 설정
	LOAD_CELL_SCK_PORT &= ~(1<<LOAD_CELL_SCK_PIN);		// SCK 초기 값: 0
}

/*
 * @note: 24bit ADC 데이터 읽기
 * @return: 24bit data, 2's complement
 * - 25번째 클럭펄스: gain, type 결정 (gain: 128, type A) 
 * - 24bit -> 32bit 부호 확장 
*/
int32_t load_cell_read(void){
	int32_t count;
	unsigned char i;
	while(LOAD_CELL_DT_IPA & (1<<LOAD_CELL_DT_PIN));
	count = 0;
	for(i=0; i<24; i++){
		LOAD_CELL_SCK_PORT |= (1<<LOAD_CELL_SCK_PIN);
		_delay_us(1);
		count = count << 1;
		LOAD_CELL_SCK_PORT &= ~(1<<LOAD_CELL_SCK_PIN);
		_delay_us(1);
		if(LOAD_CELL_DT_IPA & (1<<LOAD_CELL_DT_PIN)) count++;
	}
	// 25번째 클럭
	LOAD_CELL_SCK_PORT |= (1<<LOAD_CELL_SCK_PIN);
	_delay_us(1);
	LOAD_CELL_SCK_PORT &= ~(1<<LOAD_CELL_SCK_PIN);
	_delay_us(1);
	// 부호 확장
	if(count & 0x800000) count |= 0xFF000000;
	return count;
}

float load_cell_convert(int32_t raw){
	return ((float)raw) / WEIGHT_FACTOR;	
}

bool load_cell_status(float data){
	if(data < 10.0 && data > -10.0){
		return false;
	}
	else return true;
}
