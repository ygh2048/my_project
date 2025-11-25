/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : key.h
  * @brief          : Key scanning module header file
  ******************************************************************************
  * @attention
  *
  * Key Scanning Driver for STM32
  * Supports debounce and key state detection
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __KEY_H
#define __KEY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* Key Definitions */
#define KEY_NONE        0x00
#define KEY_OK          0x01
#define KEY_DOWN        0x02
#define KEY_UP          0x04
#define KEY_ONOFF       0x08

/* Key State */
#define KEY_RELEASED    0
#define KEY_PRESSED     1

/* Function Prototypes */
void KEY_Init(void);
uint8_t KEY_Scan(void);
uint8_t KEY_GetState(uint8_t key);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H */
