/**
 ****************************************************************************************************
 * @file        hc_sr505.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       HC-SR505人体红外传感器驱动 头文件
 * @note        适配STM32F103C8T6 (48引脚)
 *              HC_SR505_OUT -> PA8
 ****************************************************************************************************
 */

#ifndef __HC_SR505_H
#define __HC_SR505_H

#include "./SYSTEM/sys/sys.h"

/* HC-SR505引脚定义 - 适配STM32F103C8T6 */
#define HC_SR505_GPIO_PORT          GPIOA
#define HC_SR505_GPIO_PIN           SYS_GPIO_PIN8
#define HC_SR505_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA口时钟使能 */

/* IO操作宏定义 */
#define HC_SR505_READ()     ((HC_SR505_GPIO_PORT->IDR & HC_SR505_GPIO_PIN) != 0)

/* 函数声明 */
void hc_sr505_init(void);               /* HC-SR505初始化 */
uint8_t hc_sr505_detect(void);          /* 检测是否有人 */

#endif /* __HC_SR505_H */
