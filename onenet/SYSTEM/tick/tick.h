/**
 ****************************************************************************************************
 * @file        tick.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       System Timer - 1ms precision non-blocking delay base
 * @note        Based on SysTick interrupt, provides get_tick() and non-blocking delay check
 *              Solves button response delay problem, replaces delay_ms in key paths
 ****************************************************************************************************
 */

#ifndef __TICK_H
#define __TICK_H

#include "sys.h"

/**
 * @brief       Get current system tick count (Unit: ms, 1Hz = 1ms)
 * @param       None
 * @retval      Current tick count (uint32_t, overflow will auto wrap around)
 */
uint32_t get_tick(void);

/**
 * @brief       Check if delay time has been satisfied
 * @param       start_tick: Starting tick count (value returned by get_tick() call)
 * @param       delay_ms: Required delay time (ms)
 * @retval      1 - Delay satisfied, 0 - Delay not satisfied
 * @note        Usage:
 *              uint32_t t0 = get_tick();
 *              if (CHECK_DELAY(t0, 100)) {
 *                  // 100ms has passed, execute action
 *              }
 */
static inline int CHECK_DELAY(uint32_t start_tick, uint32_t delay_ms)
{
    return (get_tick() - start_tick >= delay_ms);
}

/**
 * @brief       Initialize system timer
 * @param       None
 * @retval      None
 * @note        Must be called after sys_stm32_clock_init()
 *              Configures SysTick to generate 1ms period interrupt
 */
void tick_init(void);

#endif

