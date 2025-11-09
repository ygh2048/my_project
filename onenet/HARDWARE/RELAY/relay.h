/**
 ****************************************************************************************************
 * @file        relay.h
 * @brief       电源 / 照明继电器控制
 ****************************************************************************************************
 */

#ifndef __RELAY_H
#define __RELAY_H

#include "sys.h"

#define RELAY_LIGHT_GPIO_PORT           GPIOA
#define RELAY_LIGHT_GPIO_PIN            GPIO_Pin_11
#define RELAY_LIGHT_GPIO_CLK_ENABLE()   do{ RCC->APB2ENR |= 1 << 2; }while(0)

#define RELAY_POWER_GPIO_PORT           GPIOA
#define RELAY_POWER_GPIO_PIN            GPIO_Pin_12
#define RELAY_POWER_GPIO_CLK_ENABLE()   do{ RCC->APB2ENR |= 1 << 2; }while(0)

#define RELAY_LIGHT(x)  sys_gpio_pin_set(RELAY_LIGHT_GPIO_PORT, RELAY_LIGHT_GPIO_PIN, x)
#define RELAY_POWER(x)  sys_gpio_pin_set(RELAY_POWER_GPIO_PORT, RELAY_POWER_GPIO_PIN, x)

void relay_init(void);
void relay_light_on(void);
void relay_light_off(void);
void relay_power_on(void);
void relay_power_off(void);

#endif /* __RELAY_H */
