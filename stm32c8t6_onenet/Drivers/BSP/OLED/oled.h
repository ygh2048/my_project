#ifndef __OLED_H
#define __OLED_H

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include <stdint.h>

//-----------------OLED Port Definition - STM32F103C8T6---------------- 
// I2C-only mode: SCL -> PB6, SDA -> PB7 (NO RES PIN)
// Using direct register access for GPIO ODR (output data register)

#define OLED_GPIO_ODR            (*(volatile uint32_t *)0x40010C0C)
#define OLED_SCL_PIN             (1 << 6)   // PB6
#define OLED_SDA_PIN             (1 << 7)   // PB7

#define OLED_SCL_Clr()           (OLED_GPIO_ODR &= ~OLED_SCL_PIN)
#define OLED_SCL_Set()           (OLED_GPIO_ODR |= OLED_SCL_PIN)

#define OLED_SDA_Clr()           (OLED_GPIO_ODR &= ~OLED_SDA_PIN)
#define OLED_SDA_Set()           (OLED_GPIO_ODR |= OLED_SDA_PIN)

void OLED_I2C_Init(void);
uint8_t OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#endif
