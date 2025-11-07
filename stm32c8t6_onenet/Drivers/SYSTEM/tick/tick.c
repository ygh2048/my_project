/**
 ****************************************************************************************************
 * @file        tick.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       系统计时器 - 1ms 精度的非阻塞延时基础 实现文件
 ****************************************************************************************************
 */

#include "./SYSTEM/tick/tick.h"
#include "stm32f10x.h"

/* 全局 1ms 计数器 */
static volatile uint32_t g_system_tick = 0;

/**
 * @brief       SysTick 中断处理函数
 * @param       无
 * @retval      无
 * @note        自动被 STM32 中断机制调用，每 1ms 一次
 */
void SysTick_Handler(void)
{
    g_system_tick++;
}

/**
 * @brief       获取当前系统滴答计数
 * @param       无
 * @retval      当前滴答计数 (单位: ms)
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
    
    /* 设置重载值: 72MHz * 1ms = 72000 */
    SysTick->LOAD = 72000 - 1;  /* 计数从 71999 到 0，共 72000 个周期 */
    
    /* 设置优先级: SysTick 在最低优先级 (15 = 最低) */
    NVIC_SetPriority(SysTick_IRQn, 15);
    
    /* 启用 SysTick 中断并启动计时器 */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |    /* 使用处理器时钟 (HCLK / 8) */
                    SysTick_CTRL_TICKINT_Msk |       /* 启用 SysTick 异常请求 */
                    SysTick_CTRL_ENABLE_Msk;         /* 启用计时器 */
    
    /* 重置全局计数器 */
    g_system_tick = 0;
}

