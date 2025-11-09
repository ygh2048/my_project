/**
 * ************************************************************
 * @file        app_system.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-09
 * @brief       系统管理模块实现
 * ************************************************************
 */

#include "app_system.h"

#include <stdio.h>

#include "sys.h"
#include "usart.h"
#include "delay.h"

#include "led.h"
#include "HX711.h"
#include "oled.h"
#include "hc_sr505.h"
#include "adc.h"
#include "relay.h"
#include "key.h"
#include "esp8266.h"
#include "onenet.h"
#include "mem_pool.h"

#include "app_config.h"
#include "app_runtime.h"

/**
 * @brief 显示系统启动欢迎信息
 * 
 * 在串口打印系统标题、构建信息等
 * 
 * @return void
 */
static void app_system_show_banner(void)
{
    printf("\r\n");
    printf("====================================================\r\n");
    printf("  Smart Lab Monitoring System - STM32F103C8T6     \r\n");
    printf("         Build: 2025-11-05 v1.1                   \r\n");
    printf("====================================================\r\n");
    printf("\r\n");
}

/**
 * @brief 在 OLED 上显示消息，同时 LED 闪烁等待
 * 
 * 用于在系统初始化期间显示状态信息，并通过 LED 闪烁提示用户
 * 闪烁 10 次（每次闪烁 500ms），共约 5 秒
 * 
 * @param[in] line1 第 1 行显示的文本
 * @param[in] line2 第 2 行显示的文本
 * 
 * @return void
 */
static void app_system_wait_with_message(const char *line1,
                                         const char *line2)
{
    OLED_Clear();                           // 清空屏幕
    OLED_ShowString(1, 1, line1);           // 显示第 1 行消息
    OLED_ShowString(2, 1, line2);           // 显示第 2 行消息
    OLED_ShowString(3, 1, "Continue in");   // 显示提示文本
    OLED_ShowString(4, 1, "5 seconds...");

    // LED 闪烁 10 次（每次 250ms，共 5 秒）
    for (int i = 0; i < 10; i++)
    {
        LED0(0);                // LED 亮
        delay_ms(250);
        LED0(1);                // LED 灭
        delay_ms(250);
    }
}

/**
 * @brief 初始化网络模块
 * 
 * 按顺序初始化 WiFi 和 OneNET 连接
 * 如果任何步骤失败，显示错误信息并继续运行
 * 
 * @return void
 */
static void app_system_init_network(void)
{
    uint8_t ret;

    // ===== 初始化 WiFi =====
    OLED_ShowString(2, 1, "WiFi Init...  ");

    ret = onenet_init();  // 初始化 OneNET（包括 WiFi 初始化）
    if (ret != 0)
    {
        printf("ERROR: OneNET init failed (code=%d)\r\n", ret);
        printf("System will continue without network...\r\n");
        app_system_wait_with_message("ERROR: WiFi", "Init Failed");
        return;
    }

    OLED_ShowString(2, 1, "WiFi: OK      ");

    // ===== 连接 OneNET =====
    OLED_ShowString(3, 1, "OneNET Conn...");

    ret = onenet_connect();  // 连接到 OneNET 云平台
    if (ret != 0)
    {
        printf("ERROR: OneNET connect failed\r\n");
        printf("System will continue without network...\r\n");
        app_system_wait_with_message("ERROR: OneNET", "Connect Failed");
        return;
    }

    // ===== 订阅 OneNET 消息 =====
    onenet_subscribe();
    OLED_ShowString(3, 1, "OneNET: OK    ");
    OLED_ShowString(4, 1, "System Ready! ");
    delay_ms(1000);
}

/**
 * @brief 系统初始化函数
 * 
 * 在系统启动时调用一次，用于初始化所有硬件外设和软件模块
 * 初始化顺序很重要，因为某些模块可能依赖于其他模块
 * 
 * @return void
 */
void app_system_init(void)
{
    // ===== 系统基础初始化 =====
    sys_stm32_clock_init(9);           // 初始化系统时钟 (72MHz)
    delay_init();                      // 初始化延时函数
    usart_init(72, 115200);            // 初始化串口 (115200 波特率)
    led_init();                        // 初始化 LED

    // ===== 硬件外设初始化 =====
    adc_init();                        // 初始化 ADC（电压/电流采样）
    relay_init();                      // 初始化继电器（电源和灯光控制）
    key_init();                        // 初始化按键输入

    // ===== OLED 初始化和欢迎信息 =====
    OLED_Init();                       // 初始化 OLED 显示屏
    OLED_Clear();
    OLED_ShowString(1, 1, "Smart Lab");
    OLED_ShowString(2, 1, "System Init...");

    // ===== 应用层初始化 =====
    mem_pool_init();                   // 初始化内存池（用于网络通信）
    app_runtime_init();                // 初始化应用运行时数据

    relay_power_on();                  // 启动电源继电器

    app_system_show_banner();          // 显示启动欢迎信息

    // ===== 传感器初始化 =====
    hx711_init();                      // 初始化 HX711（重量传感器）
    hx711_get_maopi();                 // 读取 HX711 零点（去皮）
    hc_sr505_init();                   // 初始化 HC_SR505（人体传感器）

    OLED_ShowString(3, 1, "HX711: OK");
    OLED_ShowString(4, 1, "Sensors: OK");

    printf("System Ready\r\n\r\n");

    // ===== 网络初始化 =====
    app_system_init_network();
    printf("System Ready\r\n\r\n");
}

/**
 * @brief 系统运行时更新函数
 * 
 * 此函数应在主循环中定期调用（建议 100ms 周期）
 * 用于更新所有传感器数据并进行自动控制
 * 
 * @return void
 */
void app_system_update(void)
{
    // ===== 更新电压和电流 =====
    // 从 ADC 通道 0 和 1 读取原始电压值
    float voltage_raw = adc_get_voltage(ADC_CH0);      // 总电压采样
    float voltage_drop = adc_get_voltage(ADC_CH1);     // 采样电阻上的压降
    
    // 计算通过采样电阻的实际电压（欧姆定律：V = I * R）
    const float sense_voltage = voltage_raw - voltage_drop;
    const float sense_current_ma = (sense_voltage / APP_SENSE_RESISTOR_OHMS) * 1000.0f * APP_CURRENT_SCALE;

    // 应用修正系数
    sys_status.voltage = voltage_raw * APP_VOLTAGE_SCALE;
    sys_status.current = sense_current_ma;
    
    // 计算功率（P = V * I）
    sys_status.power = sys_status.voltage * sys_status.current;

    // ===== 更新重量信息 =====
    hx711_get_weight();                // 从 HX711 读取重量
    sys_status.weight = (uint16_t)hx711_weight;
    
    // 判断物品是否已返回（基于重量阈值）
    sys_status.is_returned = (sys_status.weight >= APP_WEIGHT_THRESHOLD_GRAMS);

    // ===== 更新人体检测状态 =====
    sys_status.human_detected = hc_sr505_detect();

    // ===== 更新远程灯光超时计数器 =====
    app_runtime_remote_light_tick();

    // ===== 自动灯光控制（如果未被远程强制） =====
    // 如果远程灯光强制保持不活跃，则执行自动灯光逻辑
    if (!app_runtime_remote_light_override_active())
    {
        // 如果检测到人体且电源开启，则自动打开灯光
        if (sys_status.human_detected && sys_status.power_status)
        {
            if (!sys_status.light_status)
            {
                relay_light_on();               // 打开灯光
                sys_status.light_status = 1;
                printf("[AUTO] Human detected -> Light ON\r\n");
            }
        }
        // 否则关闭灯光
        else
        {
            if (sys_status.light_status)
            {
                relay_light_off();              // 关闭灯光
                sys_status.light_status = 0;
                printf("[AUTO] No human detected -> Light OFF\r\n");
            }
        }
    }
}

/**
 * @brief 上报系统状态到网络
 * 
 * 此函数将当前系统状态信息格式化并上报到 OneNET 云平台
 * 建议以 4s 左右的周期调用此函数
 * 
 * @return void
 */
void app_system_report_status(void)
{
    // 格式化系统状态数据
    int v_val = (int)(sys_status.voltage * 10);        // 电压（精度 0.1V）
    int i_val = (int)(sys_status.current * 10);        // 电流（精度 0.1mA）
    int p_val = (int)(sys_status.power * 10);          // 功率（精度 0.1mW）

    // 在串口打印状态信息
    printf("V:%d.%dV I:%d.%dmA W:%dg P:%d.%dmW\r\n",
           v_val / 10, v_val % 10,
           i_val / 10, i_val % 10,
           sys_status.weight,
           p_val / 10, p_val % 10);

    // 通过 OneNET 上报数据到云平台
    onenet_send_data();
}

/**
 * @brief 检查网络连接状态
 * 
 * 此函数检查 WiFi 和 TCP 连接状态，如果连接丢失则进行恢复
 * 当 WiFi 连续失败达到阈值时，自动复位 ESP8266 模块
 * 
 * @param[in] wifi_fail_cnt 指向 WiFi 连续失败计数器的指针
 *                          如果为 NULL，则不进行计数统计
 * 
 * @return void
 */
void app_system_check_network(uint8_t *wifi_fail_cnt)
{
    // ===== 检查 WiFi 连接 =====
    if (esp8266_check_wifi() != 0)
    {
        printf("WiFi lost!\r\n");

        if (wifi_fail_cnt != NULL)
        {
            // 增加失败计数器
            (*wifi_fail_cnt)++;
            
            // 如果连续失败次数达到阈值，复位 ESP8266 模块
            if (*wifi_fail_cnt >= APP_WIFI_FAIL_THRESHOLD)
            {
                printf("WiFi reset\r\n");
                esp8266_hw_reset_only();        // 执行硬件复位
                *wifi_fail_cnt = 0;             // 重置计数器
            }
        }
    }
    else
    {
        // WiFi 连接正常，重置失败计数器
        if (wifi_fail_cnt != NULL)
        {
            *wifi_fail_cnt = 0;
        }

        // ===== 检查 TCP 连接 =====
        // 当 WiFi 正常时，进一步检查 TCP 连接
        if (esp8266_check_tcp() != 0)
        {
            printf("TCP lost!\r\n");
            esp8266_hw_reset_only();            // 复位 ESP8266 模块以重建连接
        }
    }
}

