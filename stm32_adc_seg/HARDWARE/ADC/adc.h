#ifndef __MY_ADC_H
#define __MY_ADC_H

#include "stm32f10x.h"

// ADC1ͨ��PB0 -> ADC_Channel_8
#define ADC_CHANNEL_VSENSE      ADC_Channel_8
#define ADC_MAX_VALUE           4095U
#define ADC_REF_MV              3300U
#define ADC_SAMPLE_TIME_CYCLES  ADC_SampleTime_239Cycles5

void ADC1_Init(void);
uint16_t ADC1_ReadOnce(uint8_t channel);
uint16_t ADC1_ReadAverage(uint8_t channel, uint8_t times);
uint32_t ADC1_ToMilliVolts(uint16_t adc_value);

#endif
