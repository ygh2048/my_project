/**
 ****************************************************************************************************
 * @file        key.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       按键驱动（KEY0：电源键，KEY1：去皮键）
 ****************************************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "sys.h"

/* 引脚映射 */
#define KEY0_GPIO_PORT          GPIOB
#define KEY0_GPIO_PIN           GPIO_Pin_12
#define KEY0_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 3; }while(0)

#define KEY1_GPIO_PORT          GPIOB
#define KEY1_GPIO_PIN           GPIO_Pin_13
#define KEY1_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 3; }while(0)

/* 原始电平读取 */
#define KEY0        ((KEY0_GPIO_PORT->IDR & KEY0_GPIO_PIN) != 0)
#define KEY1        ((KEY1_GPIO_PORT->IDR & KEY1_GPIO_PIN) != 0)

/* 事件编码 */
#define KEY0_PRES   1U        /* KEY0 被按下 */
#define KEY1_PRES   2U        /* KEY1 被按下 */

void key_init(void);          /* 初始化 GPIO、EXTI 和 NVIC */
uint8_t key_scan(uint8_t mode); /* 读取事件（mode 预留） */

#endif /* __KEY_H */
