/**
 * ************************************************************
 * @file        app_display.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       显示管理模块实现
 * ************************************************************
 */

#include "app_display.h"

#include <stdio.h>
#include <string.h>

#include "oled.h"
#include "app_runtime.h"

/**
 * @brief 右对齐显示字符串在 OLED 屏幕上
 * 
 * 根据文本长度自动计算列位置，使文本在屏幕上右对齐显示
 * 屏幕宽度为 16 个字符
 * 
 * @param[in] row  显示行号 (1-4)
 * @param[in] text 要显示的字符串
 * 
 * @return void
 * 
 * @note 最长字符串应不超过 16 个字符
 */
static void app_display_right_align(uint8_t row, const char *text)
{
    uint8_t col = 1;                    // 默认列位置
    size_t len = strlen(text);          // 计算文本长度

    // 如果文本长度小于 16，计算右对齐的起始列
    if (len < 16)
    {
        col = (uint8_t)(16 - len + 1);  // 屏幕宽度 16 - 文本长度 + 1
    }

    OLED_ShowString(row, col, text);    // 在计算好的位置显示文本
}

/**
 * @brief 更新并显示系统状态到 OLED 屏幕
 * 
 * 此函数将 sys_status 结构体中的数据格式化并显示到 OLED 屏幕：
 * - 第1行左侧：电压值 (V)，右侧：物品返回状态 (RET)
 * - 第2行左侧：电流值 (I)，右侧：人体检测状态 (H)
 * - 第3行左侧：重量 (W)，右侧：灯光状态 (L)
 * - 第4行左侧：功率 (P)，右侧：电源状态 (PWR)
 * 
 * @return void
 * 
 * @note 调用此函数前需确保 OLED 已初始化
 * @see app_runtime.h (sys_status 定义)
 */
void app_display_status(void)
{
    char str[20];  // 临时字符串缓冲区

    // 清空 OLED 屏幕
    OLED_Clear();

    // ===== 第1行：电压和物品返回状态 =====
    int v_int = (int)sys_status.voltage;                    // 电压整数部分
    int v_dec = (int)((sys_status.voltage - v_int) * 10);  // 电压小数部分（一位）
    sprintf(str, "V:%d.%dV", v_int, v_dec);                // 格式：V:X.YV
    OLED_ShowString(1, 1, str);                             // 左对齐显示

    sprintf(str, "RET:%s", sys_status.is_returned ? "OK" : "NO");  // RET: OK/NO
    app_display_right_align(1, str);                        // 右对齐显示

    // ===== 第2行：电流和人体检测状态 =====
    int i_int = (int)sys_status.current;                    // 电流整数部分
    int i_dec = (int)((sys_status.current - i_int) * 10);  // 电流小数部分（一位）
    sprintf(str, "I:%d.%dmA", i_int, i_dec);               // 格式：I:X.YmA
    OLED_ShowString(2, 1, str);                             // 左对齐显示

    sprintf(str, "H:%s", sys_status.human_detected ? "YES" : "NO");  // H: YES/NO
    app_display_right_align(2, str);                        // 右对齐显示

    // ===== 第3行：重量和灯光状态 =====
    sprintf(str, "W:%dg", sys_status.weight);               // 格式：W:XXXg
    OLED_ShowString(3, 1, str);                             // 左对齐显示

    sprintf(str, "L:%s", sys_status.light_status ? "ON" : "OFF");  // L: ON/OFF
    app_display_right_align(3, str);                        // 右对齐显示

    // ===== 第4行：功率和电源状态 =====
    // 功率的处理：功率值单位为 mW，需要先转换为整数以便显示
    int p_val = (int)(sys_status.power * 10);              // 放大 10 倍避免精度损失
    int p_display_int = p_val / 10;                        // 功率整数部分
    int p_display_dec = p_val % 10;                        // 功率小数部分（一位）

    // 如果功率超过 999mW，则只显示整数部分
    if (p_display_int > 999)
    {
        sprintf(str, "P:%dmW", p_display_int);             // 格式：P:XXXXmW
    }
    else
    {
        sprintf(str, "P:%d.%dmW", p_display_int, p_display_dec);  // 格式：P:X.YmW
    }
    OLED_ShowString(4, 1, str);                             // 左对齐显示

    sprintf(str, "PWR:%s", sys_status.power_status ? "ON" : "OFF");  // PWR: ON/OFF
    app_display_right_align(4, str);                        // 右对齐显示
}


