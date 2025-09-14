#ifndef __MINE_H
#define __MINE_H

#include "stm32f4xx.h"

#define BYTE0(dwTemp)  (*((char *)(&dwTemp)))
#define BYTE1(dwTemp)  (*((char *)(&dwTemp)+1))
#define BYTE2(dwTemp)  (*((char *)(&dwTemp)+2))
#define BYTE3(dwTemp)  (*((char *)(&dwTemp)+3))

void u16_to_u8(unsigned short *u16Value,unsigned char Subscript,unsigned char *Byte);
void u8_to_u16(unsigned char *Byte,unsigned char Subscript,unsigned short *u16Value);


#define ADC_BUF_LENGTH 512


volatile extern u16 data_count ;//统计写入数据长度
volatile extern u16 write_index ; // 当前写入位置索引（volatile防止编译器优化）
extern u16 adc_record[ADC_BUF_LENGTH];

void adc_record_write(u16);
u16 adc_record_read(u16);
void adc_record_print(void);
u16 adc_record_length(void);


typedef struct limit_tem
{
	u8 value;
	int decade;//十位
	int theuint;//个位
    char p_value;
}LIMIT_TEM;


#define TEMP_AREA_LEFT   50
#define TEMP_AREA_TOP    50
#define TEMP_AREA_RIGHT  750
#define TEMP_AREA_BOTTOM 250
#define MAX_TEMP_VALUE   120  // 假设温度范围0~100.0℃（放大10倍处理）


void display_init(void);
void display_temp_line(LIMIT_TEM mylimit);
void display_information(LIMIT_TEM mylimit,int warning_state);

#endif
