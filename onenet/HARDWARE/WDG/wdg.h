/**
 ****************************************************************************************************
 * @file        wdg.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-06
 * @brief       独立看门狗驱动 头文件
 * @note        防止程序卡死
 ****************************************************************************************************
 */

#ifndef __WDG_H
#define __WDG_H

#include <stdint.h>

/* 函数声明 */
void iwdg_init(uint8_t prer, uint16_t rlr);
void iwdg_feed(void);

#endif
