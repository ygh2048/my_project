/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : lcd1602.h
  * @brief          : LCD1602 driver header file
  ******************************************************************************
  * @attention
  *
  * LCD1602 Driver for STM32
  * 4-bit mode operation
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __LCD1602_H
#define __LCD1602_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* LCD1602 Pin Definitions (from main.h) */
// RS - PB12
// RW - PB13
// E  - PB14
// D4 - PB15
// D5 - PA8
// D6 - PA11
// D7 - PA12

/* LCD1602 Control Macros */
#define LCD_RS_HIGH()   HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET)
#define LCD_RS_LOW()    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET)

#define LCD_RW_HIGH()   HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_SET)
#define LCD_RW_LOW()    HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET)

#define LCD_E_HIGH()    HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_SET)
#define LCD_E_LOW()     HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET)

/* LCD Commands */
#define LCD_CLEAR           0x01    // Clear display
#define LCD_HOME            0x02    // Return home
#define LCD_ENTRY_MODE      0x06    // Entry mode: increment cursor, no shift
#define LCD_DISPLAY_ON      0x0C    // Display on, cursor off, blink off
#define LCD_DISPLAY_OFF     0x08    // Display off
#define LCD_CURSOR_ON       0x0E    // Display on, cursor on, blink off
#define LCD_CURSOR_BLINK    0x0F    // Display on, cursor on, blink on
#define LCD_CURSOR_LEFT     0x10    // Move cursor left
#define LCD_CURSOR_RIGHT    0x14    // Move cursor right
#define LCD_DISPLAY_LEFT    0x18    // Shift display left
#define LCD_DISPLAY_RIGHT   0x1C    // Shift display right
#define LCD_FUNCTION_SET    0x28    // 4-bit mode, 2 lines, 5x8 dots

/* LCD Position */
#define LCD_LINE1           0x80    // First line address
#define LCD_LINE2           0xC0    // Second line address

/* Function Prototypes */
void LCD1602_Init(void);
void LCD1602_Clear(void);
void LCD1602_SetCursor(uint8_t line, uint8_t column);
void LCD1602_WriteChar(char c);
void LCD1602_WriteString(char *str);
void LCD1602_WriteNumber(uint32_t num);
void LCD1602_WriteFloat(float num, uint8_t decimal_places);
void LCD1602_WriteCommand(uint8_t cmd);
void LCD1602_WriteData(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif /* __LCD1602_H */
