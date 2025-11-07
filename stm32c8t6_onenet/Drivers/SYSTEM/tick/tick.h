/**
 ****************************************************************************************************
 * @file        tick.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       系统计时器 - 1ms 精度的非阻塞延时基础
 * @note        基于 SysTick 中断，提供 get_tick() 和非阻塞延时判断
 *              解决按键响应延迟问题，替代 delay_ms 在关键路径中的使用
 ****************************************************************************************************
 */

#ifndef __TICK_H
#define __TICK_H

#include "./SYSTEM/sys/sys.h"

/**
 * @brief       获取当前系统滴答计数 (单位: ms, 1Hz = 1ms)
 * @param       无
 * @retval      当前滴答计数 (uint32_t, 溢出会自动回卷)
 */
uint32_t get_tick(void);

/**
 * @brief       检查是否满足延时时间
 * @param       start_tick: 起始滴答计数 (调用 get_tick() 返回的值)
 * @param       delay_ms: 所需延时时间 (ms)
 * @retval      1 - 延时已满足, 0 - 延时未满足
 * @note        用法:
 *              uint32_t t0 = get_tick();
 *              if (CHECK_DELAY(t0, 100)) {
 *                  // 100ms 已经过去，执行操作
 *              }
 */
static inline int CHECK_DELAY(uint32_t start_tick, uint32_t delay_ms)
{
    return (get_tick() - start_tick >= delay_ms);
}

/**
 * @brief       初始化系统计时器
 * @param       无
 * @retval      无
 * @note        需要在 sys_stm32_clock_init() 之后调用
 *              该函数配置 SysTick 以生成 1ms 周期中断
 */
void tick_init(void);

#endif

