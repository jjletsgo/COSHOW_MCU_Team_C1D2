/*
 * load_cell.c
 *
 * Created: 2025-09-15 오후 8:07:38
 *  Author: AFHGF
 */ 
#include "load_cell.h"
#include "timer_0.h"
#include "UART.h"
#include <stdbool.h>
#include <util/delay.h>


timer_ms debug_timer;

void load_cell_init(void){
   // 핀 설정
   LOAD_CELL_DT_DDR &= ~(1<<LOAD_CELL_DT_PIN);  // PC3 입력으로 설정
   LOAD_CELL_SCK_DDR |= (1<<LOAD_CELL_SCK_PIN);     // PB5 출력으로 설정
   LOAD_CELL_SCK_PORT &= ~(1<<LOAD_CELL_SCK_PIN);      // SCK 초기 값: 0
}

int32_t load_cell_read(void){
   int32_t count;
   unsigned char i;
   while(LOAD_CELL_DT_IPA & (1<<LOAD_CELL_DT_PIN));
   count = 0;
   for(i=0; i<24; i++){
      LOAD_CELL_SCK_PORT |= (1<<LOAD_CELL_SCK_PIN);
     // _delay_us(1);
      count = count << 1;
      LOAD_CELL_SCK_PORT &= ~(1<<LOAD_CELL_SCK_PIN);
     // _delay_us(1);
      if(LOAD_CELL_DT_IPA & (1<<LOAD_CELL_DT_PIN)) count++;
   }
   // 25번째 클럭
   LOAD_CELL_SCK_PORT |= (1<<LOAD_CELL_SCK_PIN);
   //_delay_us(1);
   LOAD_CELL_SCK_PORT &= ~(1<<LOAD_CELL_SCK_PIN);
  // _delay_us(1);
   // 부호 확장
   if(count & 0x800000) count |= 0xFF000000;
   return count;
}

float load_cell_convert(int32_t raw){
   return ((float)raw) / WEIGHT_FACTOR;   
}
/*
bool load_cell_status(int32_t offset){
   int32_t raw_value = load_cell_read();
   int32_t net_value = raw_value - offset;
   float weight = load_cell_convert(net_value);
 
   if(weight < 1.0 && weight > -1.0){
      return false;
   }
   else return true;
}
*/

bool load_cell_status(int32_t offset){
	static timer_s weight_check_timer; 
	static bool weight_in_range = false; // 무게가 범위 내에 있는지 확인하는 플래그

	int32_t raw_value = load_cell_read();
	int32_t net_value = raw_value - offset;
	float weight = load_cell_convert(net_value);

	if(weight < 3.0 && weight > -3.0){
		if (!weight_in_range) {
			// 처음 범위에 들어온 경우, 타이머를 초기화하고 플래그를 설정
			weight_in_range = true;
			weight_check_timer.is_init_done = 0; 
		}

		// 1초가 지났는지 확인
		if (timer_delay_s(&weight_check_timer, 2)) {
			return false; 
		}
	}
	else {
		// 무게가 범위를 벗어난 경우, 플래그와 타이머를 리셋
		weight_in_range = false;
	}
	return true; 
}

bool load_cell_status_check(int32_t offset) {
	static timer_s load_cell_timer; // 타이머 초기화
	static bool last_status = false; // 마지막 상태 저장

	if (timer_delay_s(&load_cell_timer, 1)) { // 1초마다 실행
		last_status = load_cell_status(offset);
	}
	return last_status;
	}