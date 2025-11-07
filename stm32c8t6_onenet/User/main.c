/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-30
 * @brief       串口通信 实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/tick/tick.h"
#include "./BSP/LED/led.h"
#include "./BSP/HX711/HX711.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/HC_SR505/hc_sr505.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/RELAY/relay.h"
#include "./BSP/KEY/key.h"
#include "./BSP/ESP8266/esp8266.h"
#include "./BSP/ONENET/onenet.h"
#include "./BSP/ONENET/mem_pool.h"
#include <stdio.h>
#include <string.h>

/* 系统参数定义 */
#define WEIGHT_THRESHOLD    500     /* 质量阈值(克) */

/* ADC采样参数 */
#define ADC_VREF            3.3f    /* ADC参考电压 (V) */
#define R_SENSE             1000    /* 采样电阻(1k, Ω) */

/* 实际电路配置:
 * PA0: 直接采样3.3V电源
 * PA2: 采样1k电阻上的压降(用于计算电流)
 */
#define VOLTAGE_MULTIPLY    1.0f    /* PA0直接采样，无分压 */
#define CURRENT_RATIO       1.0f    /* 电流计算系数 */

/* 系统状态结构体 */
typedef struct
{
    float voltage;              /* 电压(V) */
    float current;              /* 电流(mA, 毫安) */
    float power;                /* 功率(W) */
    int32_t weight;             /* 质量(克) */
    uint8_t is_returned;        /* 物品归还状态: 0-未归还, 1-已归还 */
    uint8_t human_detected;     /* 人体检测: 0-无人, 1-有人 */
    uint8_t light_status;       /* 照明状态: 0-关, 1-开 */
    uint8_t power_status;       /* 总电源状态: 0-关, 1-开 */
} system_status_t;

system_status_t sys_status;

/* WiFi连接状态监测变量 */
uint32_t wifi_check_cnt = 0;         /* WiFi检查计数器 */
uint8_t wifi_fail_cnt = 0;           /* WiFi失败计数 */
uint8_t cmd_processing = 0;          /* 命令处理标志 */

/* 远程控制优先级变量 */
static uint32_t remote_light_timeout = 0;  /* 远程light命令有效期计数器 */
#define REMOTE_LIGHT_TIMEOUT    40  /* 远程命令有效期：400ms (40*10ms) */

#define WIFI_CHECK_INTERVAL     400         /* 4秒检查一次 (400 * 10ms) */
#define WIFI_FAIL_THRESHOLD     3           /* 失败3次后执行硬件复位 */

/* 函数声明 */
void system_init(void);
void system_update(void);
void oled_display_status(void);
void process_key(void);
void process_uart_cmd(void);

/**
 * @brief       设置远程光控制的优先级超时
 * @param       无
 * @retval      无
 */
void set_remote_light_timeout(void)
{
    remote_light_timeout = REMOTE_LIGHT_TIMEOUT;
}

/**
 * @brief       系统初始化
 * @param       无
 * @retval      无
 */
void system_init(void)
{
    sys_stm32_clock_init(9);    /* 设置时钟, 72Mhz */
    delay_init(72);             /* 延时初始化 */
    usart_init(72, 115200);     /* 串口初始化 */
    led_init();                 /* LED初始化 */

    adc_init();                 /* ADC初始化 */
    relay_init();               /* 继电器初始化 */
    key_init();                 /* 按键初始化 */
    
    /* OLED初始化 (软件I2C无法检测设备是否存在) */
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(1, 1, "Smart Lab");
    OLED_ShowString(2, 1, "System Init...");
    
    /* 初始化内存池 (用于MQTT) */
    mem_pool_init();
    
    /* 初始化系统状态 */
    memset(&sys_status, 0, sizeof(system_status_t));
    sys_status.power_status = 1;    /* 默认总电源开启 */
    relay_power_on();                /* 开启总电源 */
    
    printf("\r\n");
    printf("====================================================\r\n");
    printf("  Smart Lab Monitoring System - STM32F103C8T6     \r\n");
    printf("         Build: 2025-11-05 v1.1                   \r\n");
    printf("====================================================\r\n");
    printf("\r\n");
    
    /* 初始化外设模块 */
    hx711_init();
    hx711_get_maopi();      /* 启动时设置零点值 */
    hc_sr505_init();
    
    OLED_ShowString(3, 1, "HX711: OK");
    OLED_ShowString(4, 1, "Sensors: OK");
    
    printf("System Ready\r\n\r\n");
    
    OLED_ShowString(2, 1, "WiFi Init...  ");
    
    uint8_t ret = onenet_init();
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
    }
    else
    {
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
        }
        else
        {
            onenet_subscribe();     /* 订阅控制主题 */
            
            OLED_ShowString(3, 1, "OneNET: OK    ");
            OLED_ShowString(4, 1, "System Ready! ");
            delay_ms(1000);  /* 显示1秒后进入正常工作 */
        }
    }
    
    printf("System Ready\r\n\r\n");
}

/**
 * @brief       系统状态更新
 * @param       无
 * @retval      无
 */
void system_update(void)
{
    float voltage_raw, voltage_drop;
    
    /* 1. 读取ADC电压电流 */
    voltage_raw = adc_get_voltage(ADC_CH0);  /* PA0: 采样上方电压(电源侧) */
    voltage_drop = adc_get_voltage(ADC_CH1); /* PA2: 采样下方电压(接地侧) */
    
    /* 转换为实际电压和电流 */
    /* 电源电压 */
    sys_status.voltage = voltage_raw * VOLTAGE_MULTIPLY;
    
    /* 1k电阻两端压降 = 上方电压 - 下方电压 */
    float v_drop = voltage_raw - voltage_drop;
    
    /* 根据欧姆定律计算电流
     * I(A) = V_drop(V) / R(1000Ω)
     * I(mA) = V_drop / 1 = V_drop (单位已是mA)
     */
    sys_status.current = v_drop;  /* 电流(mA) = 压降(V) */
    
    /* 功率计算: P(mW) = V(V) * I(mA) = voltage * current
     * 这里的voltage是电源电压，current是通过1k电阻的电流
     */
    sys_status.power = sys_status.voltage * sys_status.current;  /* 单位是mW */
    
    
    /* 2. 读取HX711重量 */
    hx711_get_weight();                     /* 获取重量 */
    sys_status.weight = (uint16_t)hx711_weight;  /* 更新到系统状态 */
    
    /* 判断工具是否归还 */
    if (sys_status.weight >= WEIGHT_THRESHOLD)
    {
        sys_status.is_returned = 1;         /* 重量超过阈值,判定为已归还 */
    }
    else
    {
        sys_status.is_returned = 0;         /* 重量低于阈值,判定为未归还 */
    }
    
    /* 3. 检测人体红外 */
    sys_status.human_detected = hc_sr505_detect();
    
    /* 4. 自动控制照明 (远程控制优先级更高) */
    /* 如果有远程命令有效期，则禁用自动控制 */
    if (remote_light_timeout > 0)
    {
        remote_light_timeout--;  /* 倒计时 */
    }
    else
    {
        /* 远程控制超时，恢复自动控制 */
        if (sys_status.human_detected && sys_status.power_status)
        {
            if (!sys_status.light_status)
            {
                relay_light_on();
                sys_status.light_status = 1;
                printf("[AUTO] Human detected -> Light ON\r\n");
            }
        }
        else
        {
            if (sys_status.light_status)
            {
                relay_light_off();
                sys_status.light_status = 0;
                printf("[AUTO] No human detected -> Light OFF\r\n");
            }
        }
    }
}

/**
 * @brief       OLED显示系统状态
 * @param       无
 * @retval      无
 */
void oled_display_status(void)
{
    char str[20];
    
    OLED_Clear();
    
    /* Line 1: Voltage (左) | Return Status (右) */
    int v_int = (int)sys_status.voltage;
    int v_dec = (int)((sys_status.voltage - v_int) * 10);
    sprintf(str, "V:%d.%dV", v_int, v_dec);
    OLED_ShowString(1, 1, str);
    
    /* Right of Line 1: Return Status (right-aligned) */
    if (sys_status.is_returned)
    {
        sprintf(str, "RET:OK");
    }
    else
    {
        sprintf(str, "RET:NO");
    }
    {
        uint8_t col = 1;
        size_t len = strlen(str);
        if (len < 16) col = (uint8_t)(16 - len + 1);
        OLED_ShowString(1, col, str);
    }
    
    /* Line 2: Current (左) | Human Status (右) */
    int i_int = (int)sys_status.current;
    int i_dec = (int)((sys_status.current - i_int) * 10);
    sprintf(str, "I:%d.%dmA", i_int, i_dec);
    OLED_ShowString(2, 1, str);
    
    /* Right of Line 2: Human Status (right-aligned) */
    if (sys_status.human_detected)
    {
        sprintf(str, "H:YES");
    }
    else
    {
        sprintf(str, "H:NO");
    }
    {
        uint8_t col = 1;
        size_t len = strlen(str);
        if (len < 16) col = (uint8_t)(16 - len + 1);
        OLED_ShowString(2, col, str);
    }
    
    /* Line 3: Weight (左) | Light Status (右) */
    sprintf(str, "W:%dg", sys_status.weight);
    OLED_ShowString(3, 1, str);
    
    /* Right of Line 3: Light Status (right-aligned) */
    if (sys_status.light_status)
    {
        sprintf(str, "L:ON");
    }
    else
    {
        sprintf(str, "L:OFF");
    }
    {
        uint8_t col = 1;
        size_t len = strlen(str);
        if (len < 16) col = (uint8_t)(16 - len + 1);
        OLED_ShowString(3, col, str);
    }
    
    /* Line 4: Power (左) | Power Status (右) */
    int p_val = (int)(sys_status.power * 10);
    int p_display_int = p_val / 10;
    int p_display_dec = p_val % 10;
    
    if (p_display_int > 999)
    {
        sprintf(str, "P:%dmW", p_display_int);
    }
    else
    {
        sprintf(str, "P:%d.%dmW", p_display_int, p_display_dec);
    }
    OLED_ShowString(4, 1, str);
    
    /* Right of Line 4: Power Status (right-aligned) */
    if (sys_status.power_status)
    {
        sprintf(str, "PWR:ON");
    }
    else
    {
        sprintf(str, "PWR:OFF");
    }
    {
        uint8_t col = 1;
        size_t len = strlen(str);
        if (len < 16) col = (uint8_t)(16 - len + 1);
        OLED_ShowString(4, col, str);
    }
}

/**
 * @brief       处理按键
 * @param       无
 * @retval      无
 */
void process_key(void)
{
    uint8_t key;
    
    key = key_scan(0);
    
    if (key == KEY0_PRES)
    {
        /* Toggle Power */
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
    }
    else if (key == KEY1_PRES)
    {
        /* HX711 去皮(清零) */
        printf("[KEY1] HX711 Tare: Reading zero point...\r\n");
        hx711_get_maopi();  /* 获取当前值作为零点 */
        printf("[KEY1] Tare completed. Zero point = 0x%08lX (%ld)\r\n", 
               hx711_maopi, hx711_maopi);
        printf("[KEY1] All subsequent readings will be relative to this zero point\r\n");
    }
}

/**
 * @brief       处理串口命令
 * @param       无
 * @retval      无
 */
void process_uart_cmd(void)
{
    uint8_t len;
    
    if (g_usart_rx_sta & 0x8000)        /* Data Received */
    {
        len = g_usart_rx_sta & 0x3FFF;
        g_usart_rx_buf[len] = '\0';
        
        printf("[CMD] Received: %s\r\n", g_usart_rx_buf);
        
        /* Command Parse */
        if (strcmp((char *)g_usart_rx_buf, "PON\r\n") == 0 || 
            strcmp((char *)g_usart_rx_buf, "PON") == 0)
        {
            relay_power_on();
            sys_status.power_status = 1;
            printf("PON\r\n");
        }
        else if (strcmp((char *)g_usart_rx_buf, "POFF\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "POFF") == 0)
        {
            relay_power_off();
            relay_light_off();
            sys_status.power_status = 0;
            sys_status.light_status = 0;
            printf("POFF\r\n");
        }
        else if (strcmp((char *)g_usart_rx_buf, "LON\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "LON") == 0)
        {
            if (sys_status.power_status)
            {
                relay_light_on();
                sys_status.light_status = 1;
                printf("LON\r\n");
            }
            else
            {
                printf("[ERR] Power is OFF, cannot turn on light\r\n\r\n");
            }
        }
        else if (strcmp((char *)g_usart_rx_buf, "LOFF\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "LOFF") == 0)
        {
            relay_light_off();
            sys_status.light_status = 0;
            printf("[OK] Light OFF\r\n\r\n");
        }
        else if (strcmp((char *)g_usart_rx_buf, "ZERO\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "ZERO") == 0)
        {
            printf("[INFO] HX711 not initialized, ZERO command skipped\r\n\r\n");
        }
        else if (strcmp((char *)g_usart_rx_buf, "INFO\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "INFO") == 0)
        {
            /* 转换为整数避免浮点格式化 */
            int v_val = (int)(sys_status.voltage * 100);
            int i_val = (int)(sys_status.current * 10);
            float power_w = sys_status.power / 1000.0f;
            int p_val = (int)(power_w * 100);
            
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
        else if (strcmp((char *)g_usart_rx_buf, "HX711\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "HX711") == 0)
        {
            printf("[HX711] Debug Info:\r\n");
            printf("  Raw Buffer:  0x%08lX (%ld)\r\n", hx711_buffer, hx711_buffer);
            printf("  Zero Point:  0x%08lX (%ld)\r\n", hx711_maopi, hx711_maopi);
            printf("  Weight:      %ld g\r\n", hx711_weight);
            printf("  Delta:       %ld\r\n\r\n", (int32_t)(hx711_buffer - hx711_maopi));
        }
        else if (strcmp((char *)g_usart_rx_buf, "ADC\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "ADC") == 0)
        {
            /* 获取原始ADC读数 */
            float v_raw = adc_get_voltage(ADC_CH0);
            float i_raw = adc_get_voltage(ADC_CH1);
            
            int v_raw_int = (int)(v_raw * 1000);
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
        else if (strcmp((char *)g_usart_rx_buf, "TEST\r\n") == 0 || 
                 strcmp((char *)g_usart_rx_buf, "TEST") == 0)
        {
            printf("[TEST] Running Hardware Self-Test...\r\n");
            
            /* Test Relay */
            printf("  [TEST] Relay Light: ON...");
            relay_light_on();
            delay_ms(500);
            printf("  OFF\r\n");
            relay_light_off();
            
            printf("  [TEST] Relay Power: ON...");
            relay_power_on();
            delay_ms(500);
            printf("  OFF\r\n");
            relay_power_off();
            delay_ms(500);
            relay_power_on();
            sys_status.power_status = 1;
            
            /* Test LED */
            printf("  [TEST] LED0: Blink x3\r\n");
            for (int i = 0; i < 3; i++)
            {
                LED0(0);
                delay_ms(200);
                LED0(1);
                delay_ms(200);
            }
            
            printf("[OK] Self-Test Complete\r\n\r\n");
        }
        else
        {
            printf("[ERR] Unknown Command. Try: PON, POFF, LON, LOFF, ZERO, INFO, TEST, HX711, ADC\r\n\r\n");
        }
        
        g_usart_rx_sta = 0;
    }
}

/**
 * @brief       主函数
 * @param       无
 * @retval      无
 */
int main(void)
{
    uint16_t update_cnt = 0;
    uint16_t display_cnt = 0;
    uint16_t report_cnt = 0;
    uint16_t led_cnt = 0;
    
    system_init();                  /* System Init */
    
    /* 初始化独立看门狗 - 暂时禁用，调试重启问题 */
    /* IWDG 预分频值为64, 重载值为625 */
    /* 超时时间 = (4 * 2^prer * rlr) / 40000 = (4 * 64 * 625) / 40000 = 4000ms */
    // IWDG->KR = 0x5555;              /* 使能写入 */
    // IWDG->PR = 6;                   /* 预分频64 */
    // IWDG->RLR = 625;                /* 重载值625 */
    // IWDG->KR = 0xCCCC;              /* 启动IWDG */
    // IWDG->KR = 0xAAAA;              /* 喂狗 */
    printf("[IWDG] Watchdog DISABLED for debugging restart issue\r\n");
    
    while (1)
    {
        /* 喂狗 - 防止复位 (暂时禁用) */
        // IWDG->KR = 0xAAAA;
        
        /* 每次循环都处理按键 - 提高响应速度 */
        process_key();
        
        /* Update sensor data every 100ms */
        update_cnt++;
        if (update_cnt >= 20)  /* 20*5ms = 100ms */
        {
            update_cnt = 0;
            system_update();
        }
        
        /* Update OLED display every 500ms */
        display_cnt++;
        if (display_cnt >= 100)  /* 100*5ms = 500ms */
        {
            display_cnt = 0;
            oled_display_status();
        }
        
        /* Report status and upload data every 4s */
        report_cnt++;
        if (report_cnt >= 800)  /* 800*5ms = 4000ms */
        {
            report_cnt = 0;
            /* 转换为整数避免浮点格式化 */
            int v_val = (int)(sys_status.voltage * 10);
            int i_val = (int)(sys_status.current * 10);
            int p_val = (int)(sys_status.power * 10);  /* mW单位，精确到0.1mW */
            
            printf("V:%d.%dV I:%d.%dmA W:%dg P:%d.%dmW\r\n",
                   v_val / 10, v_val % 10,
                   i_val / 10, i_val % 10,
                   sys_status.weight,
                   p_val / 10, p_val % 10);
            
            /* 上传数据到OneNET平台 */
            onenet_send_data();
        }
        
        /* Check WiFi/TCP connection every 20s (减少检查频率以避免干扰数据传输) */
        wifi_check_cnt++;
        if (wifi_check_cnt >= 4000 && !cmd_processing)  /* 4000*5ms = 20秒检查一次 */
        {
            wifi_check_cnt = 0;
            
            /* 仅检查缓冲区中的状态标志，不发送AT命令 */
            if (esp8266_check_wifi() != 0)
            {
                printf("WiFi lost!\r\n");
                wifi_fail_cnt++;
                
                if (wifi_fail_cnt >= 2)
                {
                    printf("WiFi reset\r\n");
                    esp8266_hw_reset_only();
                    wifi_fail_cnt = 0;
                }
            }
            else
            {
                wifi_fail_cnt = 0;
                
                /* 仅在WiFi连接时检查TCP */
                if (esp8266_check_tcp() != 0)
                {
                    printf("TCP lost!\r\n");
                    esp8266_hw_reset_only();
                }
            }
        }
        
        /* 处理OneNET平台下发的命令 */
        uint8_t *onenet_data = esp8266_get_ipd(0);
        if (onenet_data != NULL)
        {
            onenet_recv_process(onenet_data);
        }
        
        /* Process UART */
        process_uart_cmd();
        
        /* LED Blink indicator - 每500ms闪烁 */
        led_cnt++;
        if (led_cnt >= 100)  /* 100*5ms = 500ms */
        {
            led_cnt = 0;
            LED0_TOGGLE();
        }
        
        /* 主循环固定延时5ms - 实现非阻塞计数，提高按键响应 */
        delay_ms(5);
    }
}





















