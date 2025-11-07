/**
 ****************************************************************************************************
 * @file        relay.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       继电器控制驱动 头文件
 * @note        适配STM32F103C8T6 (48引脚)
 *              RELAY_LIGHT -> PA11 (照明继电器)
 *              RELAY_POWER -> PA12 (总电源继电器)
 ****************************************************************************************************
 */

#ifndef __RELAY_H
#define __RELAY_H

#include "./SYSTEM/sys/sys.h"

/* 继电器引脚定义 - 适配STM32F103C8T6 */
#define RELAY_LIGHT_GPIO_PORT           GPIOA
#define RELAY_LIGHT_GPIO_PIN            SYS_GPIO_PIN11
#define RELAY_LIGHT_GPIO_CLK_ENABLE()   do{ RCC->APB2ENR |= 1 << 2; }while(0)

#define RELAY_POWER_GPIO_PORT           GPIOA
#define RELAY_POWER_GPIO_PIN            SYS_GPIO_PIN12
#define RELAY_POWER_GPIO_CLK_ENABLE()   do{ RCC->APB2ENR |= 1 << 2; }while(0)

/* IO操作宏定义 */
#define RELAY_LIGHT(x)  sys_gpio_pin_set(RELAY_LIGHT_GPIO_PORT, RELAY_LIGHT_GPIO_PIN, x)
#define RELAY_POWER(x)  sys_gpio_pin_set(RELAY_POWER_GPIO_PORT, RELAY_POWER_GPIO_PIN, x)

/* 函数声明 */
void relay_init(void);                  /* 继电器初始化 */
void relay_light_on(void);              /* 开启照明 */
void relay_light_off(void);             /* 关闭照明 */
void relay_power_on(void);              /* 开启总电源 */
void relay_power_off(void);             /* 关闭总电源 */

#endif /* __RELAY_H */
