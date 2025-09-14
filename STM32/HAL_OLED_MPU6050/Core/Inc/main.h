/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define PWMB_Pin GPIO_PIN_0
#define PWMB_GPIO_Port GPIOA
#define SERVOS1_PWM_Pin GPIO_PIN_1
#define SERVOS1_PWM_GPIO_Port GPIOA
#define AIN2_Pin GPIO_PIN_4
#define AIN2_GPIO_Port GPIOA
#define AIN1_Pin GPIO_PIN_5
#define AIN1_GPIO_Port GPIOA
#define SERVOS2_PWM_Pin GPIO_PIN_6
#define SERVOS2_PWM_GPIO_Port GPIOA
#define PWMA_Pin GPIO_PIN_7
#define PWMA_GPIO_Port GPIOA
#define ADC_Pin GPIO_PIN_0
#define ADC_GPIO_Port GPIOB
#define BIN1_Pin GPIO_PIN_10
#define BIN1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_11
#define BIN2_GPIO_Port GPIOB
#define STATE_Pin GPIO_PIN_12
#define STATE_GPIO_Port GPIOB
#define KEY_START_Pin GPIO_PIN_13
#define KEY_START_GPIO_Port GPIOB
#define KEY_CHOOSE_Pin GPIO_PIN_14
#define KEY_CHOOSE_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOB
#define R3_Pin GPIO_PIN_8
#define R3_GPIO_Port GPIOA
#define R2_Pin GPIO_PIN_11
#define R2_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_12
#define R1_GPIO_Port GPIOA
#define M_Pin GPIO_PIN_15
#define M_GPIO_Port GPIOA
#define L1_Pin GPIO_PIN_3
#define L1_GPIO_Port GPIOB
#define L2_Pin GPIO_PIN_4
#define L2_GPIO_Port GPIOB
#define L3_Pin GPIO_PIN_5
#define L3_GPIO_Port GPIOB
#define MPU6050_SCL_Pin GPIO_PIN_6
#define MPU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_7
#define MPU6050_SDA_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SCLB9_Pin GPIO_PIN_9
#define OLED_SCLB9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
