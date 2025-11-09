/**
 ****************************************************************************************************
 * @file        adc.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       ADC 采样接口：CH0 为电压，CH1 为电流检测
 ****************************************************************************************************
 */

#ifndef __ADC_H
#define __ADC_H

#include "sys.h"

#define ADC_CH0_GPIO_PORT       GPIOA
#define ADC_CH0_GPIO_PIN        GPIO_Pin_0
#define ADC_CH1_GPIO_PORT       GPIOA
#define ADC_CH1_GPIO_PIN        GPIO_Pin_2

#define ADC_CH0                 0U      /* PA0 -> ADC1_IN0  : 电压采样 */
#define ADC_CH1                 2U      /* PA2 -> ADC1_IN2  : 电流采样 */

void adc_init(void);
uint16_t adc_get_value(uint8_t channel);
float adc_get_voltage(uint8_t channel);

#endif /* __ADC_H */
