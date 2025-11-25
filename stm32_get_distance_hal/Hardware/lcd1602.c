/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : lcd1602.c
  * @brief          : LCD1602 driver implementation
  ******************************************************************************
  * @attention
  *
  * LCD1602 Driver for STM32
  * 4-bit mode operation
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "lcd1602.h"
#include <string.h>
#include <stdio.h>

/* Private function prototypes */
static void LCD1602_Enable(void);
static void LCD1602_Write4Bits(uint8_t data);

/**
  * @brief  Generate enable pulse for LCD
  * @param  None
  * @retval None
  */
static void LCD1602_Enable(void)
{
    LCD_E_HIGH();
    HAL_Delay(1);  // Delay >450ns
    LCD_E_LOW();
    HAL_Delay(1);  // Delay >37us for commands
}

/**
  * @brief  Write 4 bits to LCD data pins
  * @param  data: 4-bit data to write (lower nibble)
  * @retval None
  */
static void LCD1602_Write4Bits(uint8_t data)
{
    // D4 - PB15
    HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // D5 - PA8
    HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // D6 - PA11
    HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    // D7 - PA12
    HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  Write command to LCD
  * @param  cmd: Command byte
  * @retval None
  */
void LCD1602_WriteCommand(uint8_t cmd)
{
    LCD_RS_LOW();   // Command mode
    LCD_RW_LOW();   // Write mode
    
    // Send high nibble
    LCD1602_Write4Bits(cmd >> 4);
    LCD1602_Enable();
    
    // Send low nibble
    LCD1602_Write4Bits(cmd & 0x0F);
    LCD1602_Enable();
    
    if(cmd <= 0x02) {
        HAL_Delay(2);  // Clear and Home commands need more time
    }
}

/**
  * @brief  Write data to LCD
  * @param  data: Data byte
  * @retval None
  */
void LCD1602_WriteData(uint8_t data)
{
    LCD_RS_HIGH();  // Data mode
    LCD_RW_LOW();   // Write mode
    
    // Send high nibble
    LCD1602_Write4Bits(data >> 4);
    LCD1602_Enable();
    
    // Send low nibble
    LCD1602_Write4Bits(data & 0x0F);
    LCD1602_Enable();
}

/**
  * @brief  Initialize LCD1602 in 4-bit mode
  * @param  None
  * @retval None
  */
void LCD1602_Init(void)
{
    // Wait for LCD to power up
    HAL_Delay(50);
    
    LCD_RS_LOW();
    LCD_RW_LOW();
    LCD_E_LOW();
    
    // Initialize in 4-bit mode (special sequence)
    LCD1602_Write4Bits(0x03);
    LCD1602_Enable();
    HAL_Delay(5);
    
    LCD1602_Write4Bits(0x03);
    LCD1602_Enable();
    HAL_Delay(1);
    
    LCD1602_Write4Bits(0x03);
    LCD1602_Enable();
    HAL_Delay(1);
    
    // Switch to 4-bit mode
    LCD1602_Write4Bits(0x02);
    LCD1602_Enable();
    HAL_Delay(1);
    
    // Function set: 4-bit mode, 2 lines, 5x8 font
    LCD1602_WriteCommand(LCD_FUNCTION_SET);
    
    // Display control: display on, cursor off, blink off
    LCD1602_WriteCommand(LCD_DISPLAY_ON);
    
    // Clear display
    LCD1602_Clear();
    
    // Entry mode: increment cursor, no display shift
    LCD1602_WriteCommand(LCD_ENTRY_MODE);
    
    HAL_Delay(10);
}

/**
  * @brief  Clear LCD display
  * @param  None
  * @retval None
  */
void LCD1602_Clear(void)
{
    LCD1602_WriteCommand(LCD_CLEAR);
    HAL_Delay(2);
}

/**
  * @brief  Set cursor position
  * @param  line: Line number (0 or 1)
  * @param  column: Column number (0-15)
  * @retval None
  */
void LCD1602_SetCursor(uint8_t line, uint8_t column)
{
    uint8_t address;
    
    if(line == 0) {
        address = LCD_LINE1 + column;
    } else {
        address = LCD_LINE2 + column;
    }
    
    LCD1602_WriteCommand(address);
}

/**
  * @brief  Write a single character
  * @param  c: Character to write
  * @retval None
  */
void LCD1602_WriteChar(char c)
{
    LCD1602_WriteData((uint8_t)c);
}

/**
  * @brief  Write a string
  * @param  str: Pointer to string
  * @retval None
  */
void LCD1602_WriteString(char *str)
{
    while(*str) {
        LCD1602_WriteChar(*str++);
    }
}

/**
  * @brief  Write an unsigned integer number
  * @param  num: Number to display
  * @retval None
  */
void LCD1602_WriteNumber(uint32_t num)
{
    char buffer[12];
    sprintf(buffer, "%lu", num);
    LCD1602_WriteString(buffer);
}

/**
  * @brief  Write a float number with specified decimal places
  * @param  num: Float number to display
  * @param  decimal_places: Number of decimal places
  * @retval None
  */
void LCD1602_WriteFloat(float num, uint8_t decimal_places)
{
    char buffer[16];
    char format[8];
    sprintf(format, "%%.%df", decimal_places);
    sprintf(buffer, format, num);
    LCD1602_WriteString(buffer);
}
