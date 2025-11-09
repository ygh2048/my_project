/**
 * ************************************************************
 * @file        app_input.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       输入处理模块头文件
 * ************************************************************
 */

#ifndef APP_INPUT_H
#define APP_INPUT_H

/**
 * @brief 处理按键输入事件
 * 
 * 此函数扫描物理按键（KEY0、KEY1）并执行相应操作：
 * - KEY0：切换电源状态
 * - KEY1：执行 HX711 秤的去皮操作
 * 
 * @return void
 * 
 * @note 需要定期调用此函数以响应用户按键输入
 * @see app_input_process_uart()
 */
void app_input_process_keys(void);

/**
 * @brief 处理串口命令输入
 * 
 * 此函数从串口接收缓冲区读取命令并执行对应操作：
 * - PON：开启电源
 * - POFF：关闭电源
 * - LON：开启灯光（需要电源已开启）
 * - LOFF：关闭灯光
 * - ZERO：HX711 去皮（当前不可用）
 * - INFO：显示系统状态信息
 * - HX711：显示 HX711 调试信息
 * - ADC：显示 ADC 调试信息
 * - TEST：运行硬件自检
 * 
 * @return void
 * 
 * @note 需要定期调用此函数以处理接收到的串口命令
 * @see app_input_process_keys()
 */
void app_input_process_uart(void);

#endif /* APP_INPUT_H 头文件结束 */
