/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : key.c
  * @brief          : Key scanning module implementation
  ******************************************************************************
  * @attention
  *
  * Key Scanning Driver for STM32
  * Supports debounce and key state detection
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "key.h"

/* Private variables */
static uint8_t key_state[4] = {0};  // Key state buffer
static uint8_t key_count[4] = {0};  // Debounce counter

/**
  * @brief  Initialize key module
  * @param  None
  * @retval None
  */
void KEY_Init(void)
{
    // Keys are already initialized in MX_GPIO_Init()
    // This function is for future expansion
}

/**
  * @brief  Get key state
  * @param  key: Key identifier (KEY_OK, KEY_DOWN, KEY_UP, KEY_ONOFF)
  * @retval Key state (KEY_PRESSED or KEY_RELEASED)
  */
uint8_t KEY_GetState(uint8_t key)
{
    GPIO_PinState pin_state;
    
    switch(key)
    {
        case KEY_OK:
            pin_state = HAL_GPIO_ReadPin(KEY_OK_GPIO_Port, KEY_OK_Pin);
            break;
        case KEY_DOWN:
            pin_state = HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin);
            break;
        case KEY_UP:
            pin_state = HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
            break;
        case KEY_ONOFF:
            pin_state = HAL_GPIO_ReadPin(KEY_ONOFF_GPIO_Port, KEY_ONOFF_Pin);
            break;
        default:
            return KEY_RELEASED;
    }
    
    // Keys are active low (pressed = LOW)
    return (pin_state == GPIO_PIN_RESET) ? KEY_PRESSED : KEY_RELEASED;
}

/**
  * @brief  Scan all keys with debounce
  * @param  None
  * @retval Pressed key value (KEY_NONE if no key pressed)
  */
uint8_t KEY_Scan(void)
{
    static uint8_t key_pressed = KEY_NONE;
    static uint8_t key_released = 1;
    uint8_t key_value = KEY_NONE;
    
    // Check each key
    if(KEY_GetState(KEY_OK) == KEY_PRESSED) {
        key_value = KEY_OK;
    }
    else if(KEY_GetState(KEY_DOWN) == KEY_PRESSED) {
        key_value = KEY_DOWN;
    }
    else if(KEY_GetState(KEY_UP) == KEY_PRESSED) {
        key_value = KEY_UP;
    }
    else if(KEY_GetState(KEY_ONOFF) == KEY_PRESSED) {
        key_value = KEY_ONOFF;
    }
    
    // Key press detection with debounce
    if(key_value != KEY_NONE && key_released == 1) {
        HAL_Delay(10);  // Debounce delay
        if(KEY_GetState(key_value) == KEY_PRESSED) {
            key_released = 0;
            key_pressed = key_value;
            return key_value;
        }
    }
    else if(key_value == KEY_NONE) {
        key_released = 1;
        key_pressed = KEY_NONE;
    }
    
    return KEY_NONE;
}
