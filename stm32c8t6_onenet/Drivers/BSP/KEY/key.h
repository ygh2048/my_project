/**
 ****************************************************************************************************
 * @file        key.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       按键驱动 头文件
 * @note        适配STM32F103C8T6 (48引脚)
 *              KEY0 -> PB12 (控制总电源开关)
 *              KEY1 -> PB13 (预留功能)
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"

/* 按键引脚定义 - 适配STM32F103C8T6 */
#define KEY0_GPIO_PORT          GPIOB
#define KEY0_GPIO_PIN           SYS_GPIO_PIN12
#define KEY0_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 3; }while(0)

#define KEY1_GPIO_PORT          GPIOB
#define KEY1_GPIO_PIN           SYS_GPIO_PIN13
#define KEY1_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 3; }while(0)

/* IO操作宏定义 */
#define KEY0        ((KEY0_GPIO_PORT->IDR & KEY0_GPIO_PIN) != 0)
#define KEY1        ((KEY1_GPIO_PORT->IDR & KEY1_GPIO_PIN) != 0)

/* 按键值定义 */
#define KEY0_PRES   1       /* KEY0按下 */
#define KEY1_PRES   2       /* KEY1按下 */

/* 函数声明 */
void key_init(void);        /* 按键初始化 */
uint8_t key_scan(uint8_t mode); /* 按键扫描 */

#endif /* __KEY_H */
