/**
 * @file main.c
 * @brief 主程序文件，智能实验室监控演示系统
 * @details 该程序实现了一个基于STM32F103的智能实验室监控系统，
 *          主要功能包括：
 *          - 电压、电流、功率监测
 *          - 重量检测（物品归还检测）
 *          - 人体红外检测（自动灯光控制）
 *          - OLED显示屏实时显示
 *          - OneNET物联网平台数据上传
 *          - 串口调试命令支持
 *          - 按键控制电源和校准
 * @author ygh2048
 * @date 2025-11-08
 * @version V1.0
 */

#include <stdio.h>
#include <string.h>

#include "sys.h"          // 系统基础函数
#include "delay.h"        // 延时函数
#include "usart.h"        // 串口通信
#include "led.h"          // LED控制
#include "key.h"          // 按键检测
#include "relay.h"        // 继电器控制
#include "adc.h"          // ADC采样
#include "oled.h"         // OLED显示
#include "HX711.h"        // 重量传感器
#include "hc_sr505.h"     // 人体红外传感器
#include "esp8266.h"      // ESP8266 WiFi模块
#include "onenet.h"       // OneNET物联网平台
#include "mem_pool.h"     // 内存池管理

#include "app_config.h"   // 应用配置参数
#include "app_runtime.h"  // 运行时状态管理
#include "app_input.h"    // 输入处理

#define BUF_LEN            32  /**< 发送缓冲区长度 */

/** 发送数据缓冲区 */
static char send_voltage[BUF_LEN];  /**< 电压数据字符串 */
static char send_power[BUF_LEN];    /**< 功率数据字符串 */
static char send_current[BUF_LEN];  /**< 电流数据字符串 */
static char send_return[BUF_LEN];   /**< 归还状态字符串 */

static uint8_t wifi_fail_cnt = 0;   /**< WiFi连接失败计数器 */

/**
 * @brief 将浮点数转换为字符串（避免使用sprintf的%f）
 * @param value 浮点数值
 * @param buffer 输出缓冲区
 * @param decimals 小数位数
 * @note 手动实现，避免HardFault
 */
static void float_to_string(float value, char *buffer, uint8_t decimals)
{
    int int_part = (int)value;
    int dec_part = (int)((value - int_part) * 100);  // 保留2位小数
    if (dec_part < 0) dec_part = -dec_part;
    
    // 整数部分
    if (int_part < 0) {
        *buffer++ = '-';
        int_part = -int_part;
    }
    
    // 转换整数部分
    if (int_part >= 100) {
        *buffer++ = '0' + (int_part / 100);
        *buffer++ = '0' + ((int_part / 10) % 10);
        *buffer++ = '0' + (int_part % 10);
    } else if (int_part >= 10) {
        *buffer++ = '0' + (int_part / 10);
        *buffer++ = '0' + (int_part % 10);
    } else {
        *buffer++ = '0' + int_part;
    }
    
    // 小数部分
    if (decimals > 0) {
        *buffer++ = '.';
        *buffer++ = '0' + (dec_part / 10);
        if (decimals > 1) {
            *buffer++ = '0' + (dec_part % 10);
        }
    }
    
    *buffer = '\0';
}

/**
 * @brief 硬件初始化函数
 * @details 初始化系统各硬件模块：
 *          - 系统时钟：72MHz
 *          - 中断优先级分组
 *          - 串口：115200波特率
 *          - 各外设初始化
 *          - 内存池和运行时初始化
 *          - 默认开启电源
 */
static void Hardware_Init(void)
{
    // 系统时钟初始化为72MHz
    sys_stm32_clock_init(9);
    // 延时函数初始化
    delay_init();
    // 中断优先级分组配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    // 串口初始化：72MHz时钟，115200波特率
    usart_init(72, 115200);

    // 外设初始化
    led_init();         // LED指示灯
    key_init();         // 按键
    relay_init();       // 继电器
    adc_init();         // ADC采样
    
    // OLED初始化
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(1, 1, "Smart Lab");
    OLED_ShowString(2, 1, "System Init...");

    // 软件模块初始化
    mem_pool_init();    // 内存池
    app_runtime_init(); // 运行时状态

    // 默认开启电源
    relay_power_on();
    sys_status.power_status = 1;

    // 启动信息打印
    printf("\r\n");
    printf("====================================================\r\n");
    printf("  Smart Lab Monitoring System - STM32F103C8T6     \r\n");
    printf("         Build: 2025-11-09 v1.2                   \r\n");
    printf("====================================================\r\n");
    printf("\r\n");

    // 传感器初始化
    hx711_init();       // 重量传感器
    hx711_get_maopi();  // 获取重量传感器基准值
    hc_sr505_init();    // 人体红外传感器
    
    OLED_ShowString(3, 1, "HX711: OK");
    OLED_ShowString(4, 1, "Sensors: OK");
    
    printf("System Ready\r\n\r\n");
}

/**
 * @brief 网络初始化函数
 * @details 初始化WiFi和物联网平台连接：
 *          - ESP8266 WiFi模块初始化
 *          - OneNET平台连接和订阅
 *          - 失败时运行离线模式
 * @note 增加超时保护，防止网络初始化阻塞主程序
 */
static void Network_Init(void)
{
    uint8_t ret;

    OLED_ShowString(2, 1, "WiFi Init...  ");
    
    ret = onenet_init();
    if (ret != 0)
    {
        printf("ERROR: OneNET init failed (code=%d)\r\n", ret);
        printf("System will continue without network...\r\n");
        
        OLED_Clear();
        OLED_ShowString(1, 1, "ERROR: WiFi");
        OLED_ShowString(2, 1, "Init Failed");
        OLED_ShowString(3, 1, "Continue in");
        OLED_ShowString(4, 1, "5 seconds...");
        
        /* 显示错误5秒后继续 */
        for (int i = 0; i < 10; i++)
        {
            LED0(0);
            delay_ms(250);
            LED0(1);
            delay_ms(250);
        }
        return;
    }
    
    OLED_ShowString(2, 1, "WiFi: OK      ");
    OLED_ShowString(3, 1, "OneNET Conn...");
    
    ret = onenet_connect();
    if (ret != 0)
    {
        printf("ERROR: OneNET connect failed\r\n");
        printf("System will continue without network...\r\n");
        
        OLED_Clear();
        OLED_ShowString(1, 1, "ERROR: OneNET");
        OLED_ShowString(2, 1, "Connect Failed");
        OLED_ShowString(3, 1, "Continue in");
        OLED_ShowString(4, 1, "5 seconds...");
        
        /* 显示错误5秒后继续 */
        for (int i = 0; i < 10; i++)
        {
            LED0(0);
            delay_ms(250);
            LED0(1);
            delay_ms(250);
        }
        return;
    }
    
    onenet_subscribe();     /* 订阅控制主题 */
    
    OLED_ShowString(3, 1, "OneNET: OK    ");
    OLED_ShowString(4, 1, "System Ready! ");
    delay_ms(1000);  /* 显示1秒后进入正常工作 */
    
    printf("System Ready\r\n\r\n");
}

/**
 * @brief 主函数
 * @details 系统主循环，实现以下功能：
 *          - 传感器数据采集和处理
 *          - 自动控制逻辑（灯光、人体检测）
 *          - 按键响应处理
 *          - OLED显示更新
 *          - 数据上传到OneNET
 *          - 网络命令处理
 *          - 串口调试命令处理
 * @return int 程序返回值（理论上不会返回）
 */
int main(void)
{
    // 本地变量定义
    float voltage;           /**< 电压值(V) */
    float current;           /**< 电流值(mA) */
    float power;             /**< 功率值(mW) */
    uint8_t key_value;       /**< 按键值 */
    uint16_t upload_divider = 0;  /**< 上传计数器，用于控制上传频率 */
    uint16_t display_divider = 0; /**< 显示更新计数器，防止刷新过快 */
    static char line[32];    /**< OLED显示缓冲区，使用static避免栈溢出 */

    // 系统初始化
    Hardware_Init();
    
    // OLED初始化显示
    OLED_ShowString(1, 1, "Smart Lab Demo  ");
    OLED_ShowString(2, 1, "Initializing... ");
    delay_ms(500);
    
    // 网络初始化（带超时保护）
    Network_Init();
    
    // 清屏准备显示数据
    OLED_Clear();

    // 主循环
    while (1)
    {
        /* ==================== 传感器数据采集 ==================== */
        // 电压采样：ADC_CH0通道，乘以电压校准系数
        voltage = adc_get_voltage(ADC_CH0) * APP_VOLTAGE_SCALE;
        // 电流采样：通过分流电阻计算，单位转换为mA
        current = (adc_get_voltage(ADC_CH1) / APP_SENSE_RESISTOR_OHMS) * 1000.0f * APP_CURRENT_SCALE;
        // 功率计算：P = V * I
        power = voltage * current;

        // 重量检测：获取重量值并判断是否归还
        hx711_get_weight();
        sys_status.weight = (int32_t)hx711_weight;
        sys_status.is_returned = (sys_status.weight >= APP_WEIGHT_THRESHOLD_GRAMS);
        // 人体检测：检测是否有人员存在
        sys_status.human_detected = hc_sr505_detect();

        // 更新系统状态
        sys_status.voltage = voltage;
        sys_status.current = current;
        sys_status.power = power;

        /* ==================== 自动灯光控制逻辑 ==================== */
        // 电源关闭时，强制关闭灯光
        if (!sys_status.power_status)
        {
            relay_light_off();
            sys_status.light_status = 0;
        }
        // 电源开启且无远程控制覆盖时，根据人体检测自动控制灯光
        else if (!app_runtime_remote_light_override_active())
        {
            if (sys_status.human_detected)
            {
                relay_light_on();
                sys_status.light_status = 1;
            }
            else
            {
                relay_light_off();
                sys_status.light_status = 0;
            }
        }
        // 处理远程灯光控制
        app_runtime_remote_light_tick();

        /* ==================== 按键控制处理 ==================== */
        key_value = key_scan(0);
        if (key_value == KEY0_PRES)
        {
            // KEY0：电源开关切换
            sys_status.power_status = !sys_status.power_status;
            if (sys_status.power_status)
            {
                relay_power_on();
                printf("[KEY0] Power ON\r\n");
            }
            else
            {
                relay_power_off();
                relay_light_off();
                sys_status.light_status = 0;
                printf("[KEY0] Power OFF\r\n");
            }
            delay_ms(150); // 按键去抖延时
        }
        else if (key_value == KEY1_PRES)
        {
            // KEY1：重量传感器校准（去皮）
            printf("[KEY1] HX711 tare start...\r\n");
            hx711_get_maopi();
            printf("[KEY1] HX711 tare done.\r\n");
            delay_ms(150); // 按键去抖延时
        }

        /* ==================== OLED显示更新（每5次循环更新一次，约500ms） ==================== */
        if (++display_divider >= 5)
        {
            display_divider = 0;
            uint8_t col;
            size_t len;
            
            // 清屏
            OLED_Clear();
            
            /* Line 1: Voltage (左) | Return Status (右) */
            int v_int = (int)voltage;
            int v_dec = (int)((voltage - v_int) * 10);
            // 使用手动格式化：V:X.XV 格式
            line[0] = 'V'; line[1] = ':';
            line[2] = '0' + (v_int / 10);
            line[3] = '0' + (v_int % 10);
            line[4] = '.';
            line[5] = '0' + (v_dec % 10);
            line[6] = 'V';
            line[7] = '\0';
            OLED_ShowString(1, 1, line);
            
            // Right of Line 1: Return Status (right-aligned)
            if (sys_status.is_returned) {
                memcpy(line, "RET:OK", 7);
            } else {
                memcpy(line, "RET:NO", 7);
            }
            len = strlen(line);
            col = (len < 16) ? (uint8_t)(16 - len + 1) : 1;
            OLED_ShowString(1, col, line);
            
            /* Line 2: Current (左) | Human Status (右) */
            int i_int = (int)current;
            int i_dec = (int)((current - i_int) * 10);
            // 格式化：I:X.XmA
            line[0] = 'I'; line[1] = ':';
            line[2] = '0' + (i_int / 10);
            line[3] = '0' + (i_int % 10);
            line[4] = '.';
            line[5] = '0' + (i_dec % 10);
            line[6] = 'm'; line[7] = 'A'; line[8] = '\0';
            OLED_ShowString(2, 1, line);
            
            // Right of Line 2: Human Status (right-aligned)
            if (sys_status.human_detected) {
                memcpy(line, "H:YES", 6);
            } else {
                memcpy(line, "H:NO", 5);
            }
            len = strlen(line);
            col = (len < 16) ? (uint8_t)(16 - len + 1) : 1;
            OLED_ShowString(2, col, line);
            
            /* Line 3: Weight (左) | Light Status (右) */
            int weight = (int)sys_status.weight;
            line[0] = 'W'; line[1] = ':';
            // 格式化：W:XXXXg
            if (weight >= 1000) {
                line[2] = '0' + (weight / 1000);
                line[3] = '0' + ((weight / 100) % 10);
                line[4] = '0' + ((weight / 10) % 10);
                line[5] = '0' + (weight % 10);
                line[6] = 'g'; line[7] = '\0';
            } else if (weight >= 100) {
                line[2] = '0' + (weight / 100);
                line[3] = '0' + ((weight / 10) % 10);
                line[4] = '0' + (weight % 10);
                line[5] = 'g'; line[6] = '\0';
            } else if (weight >= 10) {
                line[2] = '0' + (weight / 10);
                line[3] = '0' + (weight % 10);
                line[4] = 'g'; line[5] = '\0';
            } else {
                line[2] = '0' + weight;
                line[3] = 'g'; line[4] = '\0';
            }
            OLED_ShowString(3, 1, line);
            
            // Right of Line 3: Light Status (right-aligned)
            if (sys_status.light_status) {
                memcpy(line, "L:ON", 5);
            } else {
                memcpy(line, "L:OFF", 6);
            }
            len = strlen(line);
            col = (len < 16) ? (uint8_t)(16 - len + 1) : 1;
            OLED_ShowString(3, col, line);
            
            /* Line 4: Power (左) | Power Status (右) */
            int p_val = (int)(power * 10);
            int p_display_int = p_val / 10;
            int p_display_dec = p_val % 10;
            
            line[0] = 'P'; line[1] = ':';
            if (p_display_int > 999) {
                // P:XXXXmW 格式（无小数点）
                line[2] = '0' + (p_display_int / 1000);
                line[3] = '0' + ((p_display_int / 100) % 10);
                line[4] = '0' + ((p_display_int / 10) % 10);
                line[5] = '0' + (p_display_int % 10);
                line[6] = 'm'; line[7] = 'W'; line[8] = '\0';
            } else if (p_display_int > 99) {
                // P:XXX.XmW 格式
                line[2] = '0' + (p_display_int / 100);
                line[3] = '0' + ((p_display_int / 10) % 10);
                line[4] = '0' + (p_display_int % 10);
                line[5] = '.';
                line[6] = '0' + p_display_dec;
                line[7] = 'm'; line[8] = 'W'; line[9] = '\0';
            } else if (p_display_int > 9) {
                // P:XX.XmW 格式
                line[2] = '0' + (p_display_int / 10);
                line[3] = '0' + (p_display_int % 10);
                line[4] = '.';
                line[5] = '0' + p_display_dec;
                line[6] = 'm'; line[7] = 'W'; line[8] = '\0';
            } else {
                line[2] = '0' + p_display_int;
                line[3] = '.';
                line[4] = '0' + p_display_dec;
                line[5] = 'm'; line[6] = 'W'; line[7] = '\0';
            }
            OLED_ShowString(4, 1, line);
            
            // Right of Line 4: Power Status (right-aligned)
            if (sys_status.power_status) {
                memcpy(line, "PWR:ON", 7);
            } else {
                memcpy(line, "PWR:OFF", 8);
            }
            len = strlen(line);
            col = (len < 16) ? (uint8_t)(16 - len + 1) : 1;
            OLED_ShowString(4, col, line);
        } // 结束OLED显示更新块

        /* ==================== 数据上传准备 ==================== */
        // 格式化数据为字符串，避免使用浮点sprintf（防止HardFault）
        float_to_string(voltage, send_voltage, 2);
        float_to_string(current, send_current, 2);
        float_to_string(power / 1000.0f, send_power, 2); // 转换为W
        
        // 整数转字符串
        if (sys_status.is_returned) {
            send_return[0] = '1';
            send_return[1] = '\0';
        } else {
            send_return[0] = '0';
            send_return[1] = '\0';
        }

        // 每20次循环上传一次数据（约2秒）
        if (++upload_divider >= 20)
        {
            upload_divider = 0;
            printf("[UPLOAD] V=%sV  I=%smA  P=%sW  Back=%s\r\n",
                   send_voltage, send_current, send_power, send_return);
            onenet_send_data(); // 上传到OneNET
        }

        /* ==================== 网络和命令处理 ==================== */
        {
            // 处理OneNET下发的命令
            uint8_t *onenet_cmd = esp8266_get_ipd(0);
            if (onenet_cmd != NULL)
            {
                onenet_recv_process(onenet_cmd);
            }

            // WiFi连接状态检查和重连
            if (esp8266_check_wifi() != 0)
            {
                if (++wifi_fail_cnt >= APP_WIFI_FAIL_THRESHOLD)
                {
                    printf("[WiFi] lost, reset module!\r\n");
                    esp8266_hw_reset_only();
                    wifi_fail_cnt = 0;
                }
            }
            else
            {
                wifi_fail_cnt = 0; // 连接正常，重置计数器
            }
        }

        // 处理串口调试命令
        app_input_process_uart();

        // LED0闪烁指示系统运行
        LED0_TOGGLE();
        // 主循环延时100ms
        delay_ms(100);
    }
}
