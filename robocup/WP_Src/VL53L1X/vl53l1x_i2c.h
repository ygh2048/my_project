#ifndef __VL53L1_I2C_H
#define __VL53L1_I2C_H

#include "Headfile.h"


uint8_t VL53L1_write_byte(uint8_t address,uint16_t index,uint8_t data);              //IIC写一个8位数据
uint8_t VL53L1_write_word(uint8_t address,uint16_t index,uint16_t data);             //IIC写一个16位数据
uint8_t VL53L1_write_dword(uint8_t address,uint16_t index,uint32_t data);            //IIC写一个32位数据
uint8_t VL53L1_write_multi(uint8_t address, uint16_t index,uint8_t *pdata,uint16_t count);//IIC连续写

uint8_t VL53L1_read_byte(uint8_t address,uint16_t index,uint8_t *pdata);             //IIC读一个8位数据
uint8_t VL53L1_read_word(uint8_t address,uint16_t index,uint16_t *pdata);            //IIC读一个16位数据
uint8_t VL53L1_read_dword(uint8_t address,uint16_t index,uint32_t *pdata);           //IIC读一个32位数据
uint8_t VL53L1_read_multi(uint8_t address,uint16_t index,uint8_t *pdata,uint16_t count);  //IIC连续读

void DelayUs(uint16_t n);		//need tested arrcoding to system clock, for 30Mhz a slight slow 
void VL53L1_I2C_Init(void);
#endif 


