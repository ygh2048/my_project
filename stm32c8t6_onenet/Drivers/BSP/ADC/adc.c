/**
 ****************************************************************************************************
 * @file        adc.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       ADC驱动 实现文件
 * @note        适配STM32F103C8T6, 使用ADC1的通道0和通道1
 ****************************************************************************************************
 */

#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"

/**
 * @brief       ADC初始化
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  /* ADC时钟为72MHz/6=12MHz */
    
    /* 配置GPIO为模拟输入 */
    GPIO_InitStructure.GPIO_Pin = ADC_CH0_GPIO_PIN | ADC_CH1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(ADC_CH0_GPIO_PORT, &GPIO_InitStructure);
    
    /* ADC配置 */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                    /* 独立模式 */
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                        /* 非扫描模式 */
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  /* 单次转换 */
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  /* 软件触发 */
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               /* 右对齐 */
    ADC_InitStructure.ADC_NbrOfChannel = 1;                              /* 1个转换通道 */
    ADC_Init(ADC1, &ADC_InitStructure);
    
    /* 使能ADC */
    ADC_Cmd(ADC1, ENABLE);
    
    /* 复位校准 */
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    
    /* 开始校准 */
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    /* 设置采样时间 (针对10k分压电路优化)
     * 对于10k等效内阻，采样时间41.5周期(ADC_CLK=12MHz)≈3.5µs
     * 足以充分采样，避免欠采样导致的跳变
     */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_41Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_41Cycles5);
}

/**
 * @brief       获取ADC值
 * @param       channel: ADC通道 (0或2)
 * @retval      ADC值 (0~4095)
 */
uint16_t adc_get_value(uint8_t channel)
{
    uint16_t adc_value;
    uint8_t i;
    uint32_t sum = 0;
    
    /* 多次采样取平均 */
    for (i = 0; i < 10; i++)
    {
        /* 配置通道 */
        ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_41Cycles5);
        
        /* 启动转换 */
        ADC_SoftwareStartConvCmd(ADC1, ENABLE);
        
        /* 等待转换完成 */
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        
        /* 读取ADC值 */
        sum += ADC_GetConversionValue(ADC1);
        delay_ms(1);
    }
    
    adc_value = sum / 10;
    return adc_value;
}

/**
 * @brief       获取电压值
 * @param       channel: ADC通道
 * @retval      电压值(V)
 */
float adc_get_voltage(uint8_t channel)
{
    uint16_t adc_value;
    float voltage;
    
    adc_value = adc_get_value(channel);
    voltage = (float)adc_value * (3.3f / 4096.0f);  /* 转换为实际电压 */
    
    return voltage;
}
