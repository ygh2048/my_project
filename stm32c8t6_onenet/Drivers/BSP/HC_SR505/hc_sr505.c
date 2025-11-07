/**
 ****************************************************************************************************
 * @file        hc_sr505.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       HC-SR505人体红外传感器驱动 实现文件
 * @note        适配STM32F103C8T6, 引脚: OUT->PA8
 ****************************************************************************************************
 */

#include "./BSP/HC_SR505/hc_sr505.h"

/**
 * @brief       HC-SR505初始化
 * @param       无
 * @retval      无
 */
void hc_sr505_init(void)
{
    HC_SR505_GPIO_CLK_ENABLE();     /* 使能时钟 */
    
    /* 配置为浮空输入模式(HC-SR501推挽输出,无需上下拉) */
    sys_gpio_set(HC_SR505_GPIO_PORT, HC_SR505_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_NONE);
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
