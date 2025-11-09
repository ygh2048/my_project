/**
 * ************************************************************
 * @file        app_config.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       应用层全局配置常量
 * ************************************************************
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/**
 * @defgroup APP_CONFIG 应用配置
 * @{
 */

/**
 * @name 传感器相关阈值
 * @{
 */
/** @brief 物品重量阈值，超过该阈值判断为已取回 */
#define APP_WEIGHT_THRESHOLD_GRAMS       500U
/** @} */

/**
 * @name 调度周期配置（基础节拍 5ms）
 * @{
 */
/** @brief 传感器更新周期：20 * 5ms = 100ms */
#define APP_SENSOR_UPDATE_TICKS          20U

/** @brief OLED 显示更新周期：100 * 5ms = 500ms */
#define APP_DISPLAY_UPDATE_TICKS         100U

/** @brief 状态上报周期：800 * 5ms = 4s */
#define APP_REPORT_UPDATE_TICKS          800U

/** @brief LED 闪烁周期：100 * 5ms = 500ms */
#define APP_LED_TOGGLE_TICKS             100U

/** @brief WiFi 巡检周期：4000 * 5ms = 20s */
#define APP_WIFI_CHECK_TICKS             4000U
/** @} */

/**
 * @name WiFi 自恢复策略
 * @{
 */
/** @brief WiFi 连接失败阈值，连续失败该次数后复位模块 */
#define APP_WIFI_FAIL_THRESHOLD          2U
/** @} */

/**
 * @name 远程灯光强制控制参数
 * @{
 */
/** @brief 远程灯光强制保持时长：40 * 100ms = 4s */
#define APP_REMOTE_LIGHT_TIMEOUT_TICKS   40U
/** @} */

/**
 * @name 测量修正参数
 * @{
 */
/** @brief 电压放大系数，用于修正 ADC 读数 */
#define APP_VOLTAGE_SCALE                1.0f

/** @brief 电流放大系数，用于修正 ADC 读数 */
#define APP_CURRENT_SCALE                1.0f

/** @brief 采样电阻值（欧姆），用于从电压计算电流 */
#define APP_SENSE_RESISTOR_OHMS          1000.0f
/** @} */

/** @} */

#endif /* APP_CONFIG_H 配置文件结束 */
