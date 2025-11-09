/**
 ****************************************************************************************************
 * @file        hc_sr505.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       HC-SR505人体红外传感器驱动实现文件
 * @note        适配STM32F103C8T6, 引脚: OUT->PA8
 ****************************************************************************************************
 */

#include "hc_sr505.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

/**
 * @brief       HC-SR505初始化
 * @param       无
 * @retval      无
 */
void hc_sr505_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     /* ???? */

    GPIO_InitStructure.GPIO_Pin = HC_SR505_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HC_SR505_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief       检测是否有人
 * @param       无
 * @retval      0: 无人, 1: 有人
 */
uint8_t hc_sr505_detect(void)
{
    return HC_SR505_READ() ? 1 : 0;
}

