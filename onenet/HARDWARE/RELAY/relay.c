/**
 * @file relay.c
 * @brief 继电器驱动实现文件
 * @author Smart Lab System
 * @version V1.0
 * @date 2025-11-04
 * @note 适配STM32F103C8T6, 引脚: LIGHT->PA11, POWER->PA12
 */

#include "relay.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

/**
 * @brief 继电器初始化
 * @param 无
 * @retval 无
 */
void relay_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = RELAY_LIGHT_GPIO_PIN | RELAY_POWER_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RELAY_LIGHT_GPIO_PORT, &GPIO_InitStructure);

    RELAY_LIGHT(0);
    RELAY_POWER(0);
}

/**
 * @brief 开启灯光
 * @param 无
 * @retval 无
 */
void relay_light_on(void)
{
    RELAY_LIGHT(1);
}

/**
 * @brief 关闭灯光
 * @param 无
 * @retval 无
 */
void relay_light_off(void)
{
    RELAY_LIGHT(0);
}

/**
 * @brief 开启电源
 * @param 无
 * @retval 无
 */
void relay_power_on(void)
{
    RELAY_POWER(1);
}

/**
 * @brief 关闭电源
 * @param 无
 * @retval 无
 */
void relay_power_off(void)
{
    RELAY_POWER(0);
}

