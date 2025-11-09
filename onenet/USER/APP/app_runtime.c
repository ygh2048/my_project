/**
 * ************************************************************
 * @file        app_runtime.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       运行时数据管理模块实现
 * ************************************************************
 */

#include "app_runtime.h"

#include <string.h>

/** @brief 全局系统状态变量，所有系统状态数据都存储在这个结构体中 */
system_status_t sys_status;

/** @brief 远程灯光强制保持的超时计数器 */
static volatile uint32_t remote_light_timeout = 0;

/** @brief 网络命令处理保护标记 */
static volatile uint8_t command_processing = 0;

/**
 * @brief 初始化运行时数据结构
 * 
 * 将系统状态结构体清零，并设置初始状态
 * 
 * @return void
 */
void app_runtime_init(void)
{
    // 清零整个系统状态结构体
    memset(&sys_status, 0, sizeof(system_status_t));

    // 设置初始状态：电源默认开启
    sys_status.power_status = 1;
}

/**
 * @brief 设置远程灯光强制保持的超时时间
 * 
 * 当收到网络命令强制开启灯光时，调用此函数
 * 设置超时计数器为 APP_REMOTE_LIGHT_TIMEOUT_TICKS
 * 在超时前，自动灯光控制将被禁用
 * 
 * @return void
 */
void set_remote_light_timeout(void)
{
    remote_light_timeout = APP_REMOTE_LIGHT_TIMEOUT_TICKS;
}

/**
 * @brief 远程灯光超时计数器递减
 * 
 * 此函数应在定时中断或系统心跳中定期调用
 * 用于实现远程灯光强制保持的自动超时机制
 * 计数器每次调用递减 1，直到归零
 * 
 * @return void
 */
void app_runtime_remote_light_tick(void)
{
    if (remote_light_timeout > 0)
    {
        remote_light_timeout--;  // 递减超时计数器
    }
}

/**
 * @brief 检查远程灯光强制控制是否处于活跃状态
 * 
 * 返回远程灯光强制保持计数器是否大于 0
 * 如果大于 0，表示强制保持仍在生效
 * 如果等于 0，表示强制保持已过期，自动控制可恢复
 * 
 * @return bool
 * @retval true  远程灯光强制保持正在生效（计数器 > 0）
 * @retval false 远程灯光强制保持已过期（计数器 = 0）
 */
bool app_runtime_remote_light_override_active(void)
{
    return (remote_light_timeout > 0);
}

/**
 * @brief 标记网络命令处理开始
 * 
 * 此函数在网络命令接收和处理前调用
 * 设置 command_processing 标记为 1
 * 用于防止在命令处理过程中发生其他任务的干扰
 * 
 * @return void
 */
void app_runtime_begin_command(void)
{
    command_processing = 1;  // 设置命令处理标记
}

/**
 * @brief 标记网络命令处理结束
 * 
 * 此函数在网络命令处理完毕后调用
 * 清除 command_processing 标记
 * 允许其他任务继续处理
 * 
 * @return void
 */
void app_runtime_end_command(void)
{
    command_processing = 0;  // 清除命令处理标记
}

/**
 * @brief 检查网络命令是否正在处理
 * 
 * 返回 command_processing 标记的状态
 * 
 * @return bool
 * @retval true  网络命令正在处理中
 * @retval false 网络命令处理已完成
 */
bool app_runtime_is_command_active(void)
{
    return (command_processing != 0);
}
