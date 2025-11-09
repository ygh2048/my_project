/**
 ****************************************************************************************************
 * @file        HX711.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       HX711压力传感器驱动 头文件
 * @note        适配STM32F103C8T6 (48引脚)
 *              HX711_SCK  -> PB3 
 *              HX711_DOUT -> PB4
 *              参考: STM32F103_TEST_HX711(可工作的例程)
 ****************************************************************************************************
 */

#ifndef __HX711_H
#define __HX711_H

#include <stdint.h>

/* 全局变量声明 */
extern uint32_t hx711_buffer;           /* HX711原始数据 */
extern uint32_t hx711_maopi;            /* 毛皮(零点)值 */
extern int32_t hx711_weight;            /* 实际重量(克) */
extern uint8_t hx711_error_flag;        /* 错误标志 */

/* 函数声明 */
void hx711_init(void);                  /* HX711初始化 */
uint32_t hx711_read(void);              /* 读取HX711原始数据 */
void hx711_get_maopi(void);             /* 获取毛皮(去皮) */
void hx711_get_weight(void);            /* 获取重量 */

#endif /* __HX711_H */
