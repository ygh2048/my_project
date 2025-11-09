/**
 ****************************************************************************************************
 * @file        hc_sr505.h
 * @brief       HC-SR505 被动红外传感器接口
 ****************************************************************************************************
 */

#ifndef __HC_SR505_H
#define __HC_SR505_H

#include "sys.h"

#define HC_SR505_GPIO_PORT          GPIOA
#define HC_SR505_GPIO_PIN           GPIO_Pin_8
#define HC_SR505_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 2; }while(0)

#define HC_SR505_READ()     ((HC_SR505_GPIO_PORT->IDR & HC_SR505_GPIO_PIN) != 0)

void hc_sr505_init(void);
uint8_t hc_sr505_detect(void);

#endif /* __HC_SR505_H */
