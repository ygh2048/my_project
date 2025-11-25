/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_OK_Pin GPIO_PIN_13
#define KEY_OK_GPIO_Port GPIOC
#define KEY_DOWN_Pin GPIO_PIN_14
#define KEY_DOWN_GPIO_Port GPIOC
#define KEY_UP_Pin GPIO_PIN_15
#define KEY_UP_GPIO_Port GPIOC
#define KEY_ONOFF_Pin GPIO_PIN_1
#define KEY_ONOFF_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_0
#define BUZZER_GPIO_Port GPIOB
#define LCD_RS_Pin GPIO_PIN_12
#define LCD_RS_GPIO_Port GPIOB
#define LCD_RW_Pin GPIO_PIN_13
#define LCD_RW_GPIO_Port GPIOB
#define LCD_E_Pin GPIO_PIN_14
#define LCD_E_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_15
#define LCD_D4_GPIO_Port GPIOB
#define LCD_D5_Pin GPIO_PIN_8
#define LCD_D5_GPIO_Port GPIOA
#define LCD_D6_Pin GPIO_PIN_11
#define LCD_D6_GPIO_Port GPIOA
#define LCD_D7_Pin GPIO_PIN_12
#define LCD_D7_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

// ==================== 预留引脚定义 ====================
// 预留输入引脚 (与CubeMX配置同步)
#define RESERVE_IN1_Pin GPIO_PIN_1
#define RESERVE_IN1_GPIO_Port GPIOB
#define RESERVE_IN2_Pin GPIO_PIN_2
#define RESERVE_IN2_GPIO_Port GPIOB
// ======================================================

// ==================== 矩阵键盘引脚定义 (4x4) ====================
// 行线 (输出，推挽输出，扫描时轮流拉低)
#define MATRIX_ROW1_Pin GPIO_PIN_5   // PB5 -> R1
#define MATRIX_ROW1_GPIO_Port GPIOB
#define MATRIX_ROW2_Pin GPIO_PIN_4   // PB4 -> R2
#define MATRIX_ROW2_GPIO_Port GPIOB
#define MATRIX_ROW3_Pin GPIO_PIN_3   // PB3 -> R3
#define MATRIX_ROW3_GPIO_Port GPIOB
#define MATRIX_ROW4_Pin GPIO_PIN_15  // PA15 -> R4
#define MATRIX_ROW4_GPIO_Port GPIOA

// 列线 (输入，上拉输入，检测按键)
#define MATRIX_COL1_Pin GPIO_PIN_6   // PB6 -> C1
#define MATRIX_COL1_GPIO_Port GPIOB
#define MATRIX_COL2_Pin GPIO_PIN_7   // PB7 -> C2
#define MATRIX_COL2_GPIO_Port GPIOB
#define MATRIX_COL3_Pin GPIO_PIN_8   // PB8 -> C3
#define MATRIX_COL3_GPIO_Port GPIOB
#define MATRIX_COL4_Pin GPIO_PIN_9   // PB9 -> C4
#define MATRIX_COL4_GPIO_Port GPIOB
// ==============================================================
////JTAGģʽ���ö���
#define JTAG_SWD_DISABLE   0X02
#define SWD_ENABLE         0X01
#define JTAG_SWD_ENABLE    0X00	

//ӳ���ַ��ʽ
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO������Ĵ�����ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 

//IO������Ĵ�����ַӳ��
#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 


	
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //����

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //����
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
