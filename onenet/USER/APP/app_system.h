/**
 * ************************************************************
 * @file        app_system.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       系统管理模块头文件
 * ************************************************************
 */

#ifndef APP_SYSTEM_H
#define APP_SYSTEM_H

#include <stdint.h>

/**
 * @brief 系统初始化
 * 
 * 此函数在系统启动时调用，用于初始化所有硬件外设和软件模块：
 * 1. 系统时钟初始化
 * 2. 延时模块初始化
 * 3. 串口初始化
 * 4. LED、ADC、继电器、按键等外设初始化
 * 5. OLED 屏幕初始化
 * 6. 内存池初始化
 * 7. 应用运行时数据初始化
 * 8. 传感器初始化（HX711、HC_SR505）
 * 9. 网络模块初始化（WiFi、OneNET）
 * 
 * @return void
 * 
 * @note 此函数必须在主函数开始时调用一次
 * @see app_system_update()
 */
void app_system_init(void);

/**
 * @brief 系统运行时更新
 * 
 * 此函数应在主循环中定期调用，用于更新系统运行时状态：
 * 1. 从 ADC 读取电压和电流值
 * 2. 计算功率 (P = V * I)
 * 3. 从 HX711 读取重量值
 * 4. 判断物品是否已返回（基于重量阈值）
 * 5. 检测人体存在（HC_SR505）
 * 6. 更新远程灯光超时计数器
 * 7. 根据人体检测结果自动控制灯光（如果未被远程强制）
 * 
 * @return void
 * 
 * @note 建议以 100ms 左右的周期调用此函数
 * @see APP_SENSOR_UPDATE_TICKS
 */
void app_system_update(void);

/**
 * @brief 上报系统状态到网络
 * 
 * 此函数将当前系统状态信息格式化并上报到 OneNET 云平台
 * 包括电压、电流、功率、重量等实时数据
 * 
 * @return void
 * 
 * @note 建议以 4s 左右的周期调用此函数
 * @see APP_REPORT_UPDATE_TICKS
 */
void app_system_report_status(void);

/**
 * @brief 检查网络连接状态
 * 
 * 此函数检查 WiFi 和 TCP 连接状态，如果连接丢失则进行恢复：
 * 1. 检查 WiFi 连接是否正常
 * 2. 如果 WiFi 丢失，增加失败计数器
 * 3. 当失败次数达到阈值时，复位 ESP8266 模块
 * 4. 检查 TCP 连接是否正常
 * 5. 如果 TCP 丢失，复位 ESP8266 模块
 * 
 * @param[in] wifi_fail_cnt 指向 WiFi 连续失败计数器的指针
 *                          如果为 NULL，则不进行计数统计
 * 
 * @return void
 * 
 * @note 建议以 20s 左右的周期调用此函数
 * @see APP_WIFI_CHECK_TICKS
 * @see APP_WIFI_FAIL_THRESHOLD
 */
void app_system_check_network(uint8_t *wifi_fail_cnt);

#endif /* APP_SYSTEM_H 头文件结束 */
