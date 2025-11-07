/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       ADC驱动 头文件
 * @note        适配STM32F103C8T6 (48引脚)
 *              ADC_CH0 -> PA0 (电压采样)
 *              ADC_CH1 -> PA2 (电流采样 - PA1被ESP8266复位占用)
 ****************************************************************************************************
 */

#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"

/* ADC通道定义 */
#define ADC_CH0_GPIO_PORT       GPIOA
#define ADC_CH0_GPIO_PIN        SYS_GPIO_PIN0
#define ADC_CH1_GPIO_PORT       GPIOA
#define ADC_CH1_GPIO_PIN        SYS_GPIO_PIN2

#define ADC_CH0                 0       /* PA0 - ADC1_IN0 (电压采样) */
#define ADC_CH1                 2       /* PA2 - ADC1_IN2 (电流采样) */

/* 函数声明 */
void adc_init(void);                    /* ADC初始化 */
uint16_t adc_get_value(uint8_t channel); /* 获取ADC值 */
float adc_get_voltage(uint8_t channel);  /* 获取电压值(V) */

#endif /* __ADC_H */
