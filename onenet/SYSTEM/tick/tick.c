/**
 ****************************************************************************************************
 * @file        tick.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       System Timer - 1ms precision non-blocking delay base implementation file
 ****************************************************************************************************
 */

#include "tick.h"
#include "stm32f10x.h"

/* Global 1ms tick counter */
static volatile uint32_t g_system_tick = 0;

/**
 * @brief       SysTick interrupt handler
 * @param       None
 * @retval      None
 * @note        Automatically called by STM32 interrupt mechanism, once per 1ms
 */
void SysTick_Handler(void)
{
    g_system_tick++;
}

/**
 * @brief       Get current system tick count
 * @param       None
 * @retval      Current tick count (Unit: ms)
 */
uint32_t get_tick(void)
{
    return g_system_tick;
}

/**
 * @brief       初始化系统计时器
 * @param       无
 * @retval      无
 * @note        配置 SysTick 以生成 1ms 周期中断
 *              SysTick 直接使用处理器时钟 (72MHz)
 *              计数值 = 72MHz * 1ms = 72000
 */
void tick_init(void)
{
    /* 禁用 SysTick */
    SysTick->CTRL = 0;
    
    /* 清除计数值 */
    SysTick->VAL = 0;
    
    /* Set clock source to HCLK/8 (same as delay.c) */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    
    /* Set reload value: (72MHz / 8) * 1ms = 9000, so reload = 8999 */
    SysTick->LOAD = 8999;
    
    /* 设置优先级: SysTick 在最低优先级 (15 = 最低) */
    NVIC_SetPriority(SysTick_IRQn, 15);
    
    /* 启用 SysTick 中断并启动计时器 */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |    /* 使用处理器时钟 (HCLK / 8) */
                    SysTick_CTRL_TICKINT_Msk |       /* 启用 SysTick 异常请求 */
                    SysTick_CTRL_ENABLE_Msk;         /* 启用计时器 */
    
    /* 重置全局计数器 */
    g_system_tick = 0;
}

