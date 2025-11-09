/**
 * ************************************************************
 * @file        app_input.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       输入处理模块 - 负责按键和串口命令处理
 * ************************************************************
 */

#include "app_input.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "usart.h"
#include "delay.h"

#include "key.h"
#include "relay.h"
#include "HX711.h"
#include "led.h"
#include "adc.h"

#include "app_runtime.h"

/**
 * @brief 处理按键输入事件
 * 
 * 此函数扫描物理按键并执行相应操作：
 * - KEY0_PRES：切换电源状态
 *   - 按下时切换 power_status，如果开启则驱动继电器，如果关闭则关闭电源和灯光
 * - KEY1_PRES：执行 HX711 秤的去皮（零点校准）
 *   - 按下时读取并保存当前的零点值
 * 
 * @return void
 */
void app_input_process_keys(void)
{
    uint8_t key = key_scan(0);  // 扫描按键，参数 0 表示连续扫描模式

    if (key == KEY0_PRES)  // 按键 0 被按下
    {
        // 切换电源状态
        sys_status.power_status = !sys_status.power_status;

        if (sys_status.power_status)
        {
            // 电源开启：驱动电源继电器
            relay_power_on();
            printf("[KEY0] Power ON\r\n");
        }
        else
        {
            // 电源关闭：关闭电源和灯光继电器
            relay_power_off();
            relay_light_off();
            sys_status.light_status = 0;  // 同步更新灯光状态
            printf("[KEY0] Power OFF\r\n");
        }
    }
    else if (key == KEY1_PRES)  // 按键 1 被按下
    {
        // 执行 HX711 秤的去皮操作（读取零点参考值）
        printf("[KEY1] HX711 Tare: Reading zero point...\r\n");
        hx711_get_maopi();  // 读取当前的零点值
        printf("[KEY1] Tare completed. Zero point = 0x%08lX (%ld)\r\n",
               hx711_maopi, hx711_maopi);
        printf("[KEY1] All subsequent readings will be relative to this zero point\r\n");
    }
}

/**
 * @brief 处理串口输入命令
 * 
 * 此函数从串口接收缓冲区读取命令并执行对应操作。
 * 支持的命令列表：
 * 
 * 1. PON - 开启电源
 * 2. POFF - 关闭电源
 * 3. LON - 开启灯光（需要电源已开启）
 * 4. LOFF - 关闭灯光
 * 5. ZERO - HX711 去皮（当前不可用）
 * 6. INFO - 显示系统状态信息
 * 7. HX711 - 显示 HX711 调试信息
 * 8. ADC - 显示 ADC 调试信息
 * 9. TEST - 运行硬件自检
 * 
 * @return void
 * 
 * @note 需要在主循环中定期调用此函数
 */
void app_input_process_uart(void)
{
    uint8_t len;

    // 检查是否接收到完整的命令（0x8000 标志位表示接收完毕）
    if (g_usart_rx_sta & 0x8000)
    {
        len = g_usart_rx_sta & 0x3FFF;      // 提取接收数据长度
        g_usart_rx_buf[len] = '\0';         // 添加字符串结束符

        printf("[CMD] Received: %s\r\n", g_usart_rx_buf);

        // ===== PON 命令：开启电源 =====
        if (strcmp((char *)g_usart_rx_buf, "PON\r\n") == 0 ||
            strcmp((char *)g_usart_rx_buf, "PON") == 0)
        {
            relay_power_on();
            sys_status.power_status = 1;
            printf("PON\r\n");
        }
        // ===== POFF 命令：关闭电源 =====
        else if (strcmp((char *)g_usart_rx_buf, "POFF\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "POFF") == 0)
        {
            relay_power_off();
            relay_light_off();
            sys_status.power_status = 0;
            sys_status.light_status = 0;
            printf("POFF\r\n");
        }
        // ===== LON 命令：开启灯光 =====
        else if (strcmp((char *)g_usart_rx_buf, "LON\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "LON") == 0)
        {
            if (sys_status.power_status)
            {
                // 仅在电源开启的情况下允许打开灯光
                relay_light_on();
                sys_status.light_status = 1;
                printf("LON\r\n");
            }
            else
            {
                // 电源关闭时报错
                printf("[ERR] Power is OFF, cannot turn on light\r\n\r\n");
            }
        }
        // ===== LOFF 命令：关闭灯光 =====
        else if (strcmp((char *)g_usart_rx_buf, "LOFF\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "LOFF") == 0)
        {
            relay_light_off();
            sys_status.light_status = 0;
            printf("[OK] Light OFF\r\n\r\n");
        }
        // ===== ZERO 命令：HX711 去皮 =====
        else if (strcmp((char *)g_usart_rx_buf, "ZERO\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "ZERO") == 0)
        {
            // 此功能当前暂未实现
            printf("[INFO] HX711 not initialized, ZERO command skipped\r\n\r\n");
        }
        // ===== INFO 命令：显示系统状态信息 =====
        else if (strcmp((char *)g_usart_rx_buf, "INFO\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "INFO") == 0)
        {
            // 格式化系统状态信息
            int v_val = (int)(sys_status.voltage * 100);      // 电压（精度 0.01V）
            int i_val = (int)(sys_status.current * 10);       // 电流（精度 0.1mA）
            float power_w = sys_status.power / 1000.0f;       // 功率转换为瓦特
            int p_val = (int)(power_w * 100);                 // 功率（精度 0.01W）

            printf("[INFO] System Status:\r\n");
            printf("  Voltage:     %d.%02d V\r\n", v_val / 100, v_val % 100);
            printf("  Current:     %d.%d mA\r\n", i_val / 10, i_val % 10);
            printf("  Power:       %d.%02d W\r\n", p_val / 100, p_val % 100);
            printf("  Weight:      %d g\r\n", sys_status.weight);
            printf("  Returned:    %s\r\n", sys_status.is_returned ? "YES" : "NO");
            printf("  Human:       %s\r\n", sys_status.human_detected ? "DETECTED" : "NONE");
            printf("  Light:       %s\r\n", sys_status.light_status ? "ON" : "OFF");
            printf("  Power:       %s\r\n\r\n", sys_status.power_status ? "ON" : "OFF");
        }
        // ===== HX711 命令：显示 HX711 调试信息 =====
        else if (strcmp((char *)g_usart_rx_buf, "HX711\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "HX711") == 0)
        {
            printf("[HX711] Debug Info:\r\n");
            printf("  Raw Buffer:  0x%08lX (%ld)\r\n", hx711_buffer, hx711_buffer);
            printf("  Zero Point:  0x%08lX (%ld)\r\n", hx711_maopi, hx711_maopi);
            printf("  Weight:      %ld g\r\n", hx711_weight);
            printf("  Delta:       %ld\r\n\r\n", (int32_t)(hx711_buffer - hx711_maopi));
        }
        // ===== ADC 命令：显示 ADC 调试信息 =====
        else if (strcmp((char *)g_usart_rx_buf, "ADC\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "ADC") == 0)
        {
            // 读取原始 ADC 值
            float v_raw = adc_get_voltage(ADC_CH0);   // 通道 0：电压采样
            float i_raw = adc_get_voltage(ADC_CH1);   // 通道 1：电流采样

            int v_raw_int = (int)(v_raw * 1000);      // 转换为整数显示
            int i_raw_int = (int)(i_raw * 1000);
            int v_display = (int)(sys_status.voltage * 1000);
            int i_display = (int)(sys_status.current * 1000);
            int p_display = (int)(sys_status.power * 1000);

            printf("[ADC] Debug Info:\r\n");
            printf("  V_Raw:       %d.%03d V (actual)\r\n", v_raw_int / 1000, v_raw_int % 1000);
            printf("  I_Raw:       %d.%03d V (actual)\r\n", i_raw_int / 1000, i_raw_int % 1000);
            printf("  V_Display:   %d.%03d V\r\n", v_display / 1000, v_display % 1000);
            printf("  I_Display:   %d.%03d mA\r\n", i_display / 1000, i_display % 1000);
            printf("  P_Display:   %d.%03d mW\r\n", p_display / 1000, p_display % 1000);
            printf("  P_Calc:      V*I/1000 = %d*%d/1000 = %d\r\n\r\n",
                   v_display, i_display, (v_display * i_display) / 1000);
        }
        // ===== TEST 命令：运行硬件自检 =====
        else if (strcmp((char *)g_usart_rx_buf, "TEST\r\n") == 0 ||
                 strcmp((char *)g_usart_rx_buf, "TEST") == 0)
        {
            printf("[TEST] Running Hardware Self-Test...\r\n");

            // 测试灯光继电器
            printf("  [TEST] Relay Light: ON...");
            relay_light_on();
            delay_ms(500);
            printf("  OFF\r\n");
            relay_light_off();

            // 测试电源继电器
            printf("  [TEST] Relay Power: ON...");
            relay_power_on();
            delay_ms(500);
            printf("  OFF\r\n");
            relay_power_off();
            delay_ms(500);
            relay_power_on();
            sys_status.power_status = 1;

            // 测试 LED0 闪烁
            printf("  [TEST] LED0: Blink x3\r\n");
            for (int i = 0; i < 3; i++)
            {
                LED0(0);                // LED 开
                delay_ms(200);
                LED0(1);                // LED 关
                delay_ms(200);
            }

            printf("[OK] Self-Test Complete\r\n\r\n");
        }
        // ===== 未知命令 =====
        else
        {
            printf("[ERR] Unknown Command. Try: PON, POFF, LON, LOFF, ZERO, INFO, TEST, HX711, ADC\r\n\r\n");
        }

        // 清除串口接收状态标志，准备接收下一条命令
        g_usart_rx_sta = 0;
    }
}

