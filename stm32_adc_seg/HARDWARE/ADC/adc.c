#include "adc.h"
#include "delay.h"

// ��ʼ��ADC1��ʹ��PB0(ͨ��8)�����룬ADCCLK=12MHz��239.5T����ʱ
void ADC1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); // 72 MHz / 6 = 12 MHz

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1))
        ;
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1))
        ;
}

// ��ͨ���������һ�η���
uint16_t ADC1_ReadOnce(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SAMPLE_TIME_CYCLES);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;
    return ADC_GetConversionValue(ADC1);
}

// ƽ���ۼ򣬼Ӷ������ȶ���
uint16_t ADC1_ReadAverage(uint8_t channel, uint8_t times)
{
    uint32_t acc = 0;
    uint8_t i;
    for (i = 0; i < times; i++)
    {
        acc += ADC1_ReadOnce(channel);
        delay_ms(2);
    }
    return (uint16_t)(acc / times);
}

// ��ADCֵת�ɹ�mV��������
uint32_t ADC1_ToMilliVolts(uint16_t adc_value)
{
    // Round by adding half divisor.
    return (adc_value * ADC_REF_MV + (ADC_MAX_VALUE / 2)) / ADC_MAX_VALUE;
}
