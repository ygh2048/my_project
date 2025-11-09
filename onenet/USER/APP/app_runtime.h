/**
 * ************************************************************
 * @file        app_runtime.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       运行时数据管理模块头文件
 * ************************************************************
 */

#ifndef APP_RUNTIME_H
#define APP_RUNTIME_H

#include <stdbool.h>
#include <stdint.h>

#include "app_config.h"

/**
 * @defgroup RUNTIME_DATA 运行时数据
 * @{
 */

/**
 * @struct system_status_t
 * @brief 系统状态结构体，用于存储所有运行时状态数据
 */
typedef struct
{
    /** @brief 电压值（单位：V） */
    float voltage;

    /** @brief 电流值（单位：mA） */
    float current;

    /** @brief 功率值（单位：mW） */
    float power;

    /** @brief 重量值（单位：g） */
    int32_t weight;

    /** @brief 物品是否已返回（1=已返回，0=未返回），基于重量阈值判断 */
    uint8_t is_returned;

    /** @brief 是否检测到人体（1=检测到，0=未检测到） */
    uint8_t human_detected;

    /** @brief 灯光状态（1=开启，0=关闭） */
    uint8_t light_status;

    /** @brief 电源状态（1=开启，0=关闭） */
    uint8_t power_status;
} system_status_t;

/** @brief 全局系统状态变量 */
extern system_status_t sys_status;

/** @} */

/**
 * @brief 初始化运行时数据结构
 * 
 * 将系统状态结构体清零，并设置初始状态（电源默认开启）
 * 
 * @return void
 */
void app_runtime_init(void);

/**
 * @defgroup REMOTE_LIGHT 远程灯光控制
 * @{
 */

/**
 * @brief 设置远程灯光强制保持的超时时间
 * 
 * 当收到网络命令强制开启灯光时，调用此函数设置超时计数器
 * 在超时前，自动灯光控制将被禁用
 * 
 * @return void
 * 
 * @see app_runtime_remote_light_tick()
 * @see app_runtime_remote_light_override_active()
 */
void set_remote_light_timeout(void);

/**
 * @brief 远程灯光超时计数器递减
 * 
 * 此函数应在定时器中断或系统心跳中定期调用
 * 用于实现远程灯光强制保持的自动超时机制
 * 
 * @return void
 * 
 * @see set_remote_light_timeout()
 */
void app_runtime_remote_light_tick(void);

/**
 * @brief 检查远程灯光强制控制是否处于活跃状态
 * 
 * @return bool
 * @retval true  远程灯光强制保持正在生效
 * @retval false 远程灯光强制保持已过期，自动控制可恢复
 * 
 * @see set_remote_light_timeout()
 */
bool app_runtime_remote_light_override_active(void);

/** @} */

/**
 * @defgroup COMMAND_PROTECTION 命令处理保护
 * @{
 */

/**
 * @brief 标记网络命令处理开始
 * 
 * 此函数在网络命令接收和处理前调用，用于设置保护标记
 * 防止在命令处理过程中发生其他任务的干扰
 * 
 * @return void
 * 
 * @see app_runtime_end_command()
 * @see app_runtime_is_command_active()
 */
void app_runtime_begin_command(void);

/**
 * @brief 标记网络命令处理结束
 * 
 * 此函数在网络命令处理完毕后调用，用于清除保护标记
 * 允许其他任务继续处理
 * 
 * @return void
 * 
 * @see app_runtime_begin_command()
 */
void app_runtime_end_command(void);

/**
 * @brief 检查网络命令是否正在处理
 * 
 * @return bool
 * @retval true  网络命令正在处理中
 * @retval false 网络命令处理已完成
 * 
 * @see app_runtime_begin_command()
 */
bool app_runtime_is_command_active(void);

/** @} */

#endif /* APP_RUNTIME_H 头文件结束 */
