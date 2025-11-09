/**
 ****************************************************************************************************
 * @file        wdg.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-06
 * @brief       看门狗驱动实现文件
 * @note        IWDG timeout = (4 * 2^prer * rlr) / 40 (ms)
 *              例如: prer=6, rlr=625 -> timeout = 4000ms (4秒)
 ****************************************************************************************************
 */

#include "wdg.h"
#include "stm32f10x_iwdg.h"

/**
 * @brief       初始化独立看门狗
 * @param       prer: 预分频数 (0~7, 对应分频数: 4,8,16,32,64,128,256)
 *              rlr: 重装载值 (0~0xFFF, 即1~4095)
 * @retval      无
 * @note        超时时间计算: Tout = ((4 * 2^prer) * rlr) / 40 (ms)
 *              建议: prer=6, rlr=625 -> 4秒超时
 */
void iwdg_init(uint8_t prer, uint16_t rlr)
{
    /* 取消寄存器写保护 */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    
    /* 设置预分频系数 */
    IWDG_SetPrescaler(prer);
    
    /* 设置重装载值 */
    IWDG_SetReload(rlr);
    
    /* 重载并启动看门狗 */
    IWDG_ReloadCounter();
    IWDG_Enable();
}

/**
 * @brief       喂狗函数
 * @param       无
 * @retval      无
 * @note        在主循环中定时调用
 */
void iwdg_feed(void)
{
    IWDG_ReloadCounter();
}

