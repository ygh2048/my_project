/**
 * ************************************************************
 * @file        app_display.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       显示管理模块头文件
 * ************************************************************
 */

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

/**
 * @brief 更新并显示系统状态到 OLED 屏幕
 * 
 * 此函数会在 OLED 屏幕上显示以下信息：
 * - 第1行：电压值和物品返回状态
 * - 第2行：电流值和人体检测状态
 * - 第3行：重量和灯光状态
 * - 第4行：功率和电源状态
 * 
 * @return void
 * 
 * @note 需要定期调用此函数以刷新显示内容
 * @see app_display_right_align()
 */
void app_display_status(void);

#endif /* APP_DISPLAY_H 头文件结束 */
