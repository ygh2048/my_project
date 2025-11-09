/**
 ****************************************************************************************************
 * @file        led.h
 * @brief       板载 LED 控制
 ****************************************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "sys.h"

#define LED0_GPIO_PORT          GPIOC
#define LED0_GPIO_PIN           GPIO_Pin_13
#define LED0_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 4; }while(0)

#define LED1_GPIO_PORT          GPIOD
#define LED1_GPIO_PIN           GPIO_Pin_2
#define LED1_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 5; }while(0)

#define LED0(x)         sys_gpio_pin_set(LED0_GPIO_PORT, LED0_GPIO_PIN, x)
#define LED1(x)         sys_gpio_pin_set(LED1_GPIO_PORT, LED1_GPIO_PIN, x)

#define LED0_TOGGLE()   do{ LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN; }while(0)
#define LED1_TOGGLE()   do{ LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN; }while(0)

void led_init(void);

#endif /* __LED_H */
