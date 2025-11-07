/**
 ****************************************************************************************************
 * @file        relay.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       继电器控制驱动 实现文件
 * @note        适配STM32F103C8T6, 引脚: LIGHT->PA11, POWER->PA12
 ****************************************************************************************************
 */

#include "./BSP/RELAY/relay.h"

/**
 * @brief       继电器初始化
 * @param       无
 * @retval      无
 */
void relay_init(void)
{
    RELAY_LIGHT_GPIO_CLK_ENABLE();
    RELAY_POWER_GPIO_CLK_ENABLE();
    
    /* 配置为推挽输出 */
    sys_gpio_set(RELAY_LIGHT_GPIO_PORT, RELAY_LIGHT_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);
    
    sys_gpio_set(RELAY_POWER_GPIO_PORT, RELAY_POWER_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PD);
    
    /* 初始状态:关闭 */
    RELAY_LIGHT(0);
    RELAY_POWER(0);
}

/**
 * @brief       开启照明
 * @param       无
 * @retval      无
 */
void relay_light_on(void)
{
    RELAY_LIGHT(1);
}

/**
 * @brief       关闭照明
 * @param       无
 * @retval      无
 */
void relay_light_off(void)
{
    RELAY_LIGHT(0);
}

/**
 * @brief       开启总电源
 * @param       无
 * @retval      无
 */
void relay_power_on(void)
{
    RELAY_POWER(1);
}

/**
 * @brief       关闭总电源
 * @param       无
 * @retval      无
 */
void relay_power_off(void)
{
    RELAY_POWER(0);
}
