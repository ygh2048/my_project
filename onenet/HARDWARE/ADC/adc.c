/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       ADC 采样实现：提供初始化、原始值和电压值读取
 ****************************************************************************************************
 */

#include "adc.h"
#include "delay.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"

void adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 1. 使能 GPIOA 与 ADC1 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  /* ADCCLK = 72MHz / 6 = 12MHz */

    /* 2. PA0 / PA2 设置为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = ADC_CH0_GPIO_PIN | ADC_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC_CH0_GPIO_PORT, &GPIO_InitStructure);

    /* 3. ADC1 基本参数 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);

    /* 4. 标定流程 */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

uint16_t adc_get_value(uint8_t channel)
{
    uint32_t sum = 0;

    for (uint8_t i = 0; i < 10; i++)
    {
        ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_41Cycles5);
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        sum += ADC_GetConversionValue(ADC1);
        delay_ms(1);
    }

    return (uint16_t)(sum / 10);
}

float adc_get_voltage(uint8_t channel)
{
    uint16_t raw = adc_get_value(channel);
    return (float)raw * (3.3f / 4096.0f);
}
