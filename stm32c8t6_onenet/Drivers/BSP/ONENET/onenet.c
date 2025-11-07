/**
 ****************************************************************************************************
 * @file        onenet.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       OneNET物联网平台接入 高层封装
 * @note        产品ID: 489VMXhRtU
 *              设备名称: WORK
 ****************************************************************************************************
 */

#include "./BSP/ONENET/onenet.h"
#include "./BSP/ONENET/onenet_protocol.h"
#include "./BSP/ESP8266/esp8266.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include <stdio.h>
#include <string.h>

/* OneNET服务器连接命令 */
#define ONENET_SERVER_CMD   "AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

/* 外部变量声明 */
extern system_status_t sys_status;

/* OneNET MQTT主题 */
#define MQTT_TOPIC_PROP_POST    "$sys/489VMXhRtU/WORK/thing/property/post"
#define MQTT_TOPIC_PROP_SET     "$sys/489VMXhRtU/WORK/thing/property/set"
#define MQTT_TOPIC_PROP_SET_REPLY "$sys/489VMXhRtU/WORK/thing/property/set_reply"

/**
 * @brief       OneNET初始化
 * @param       无
 * @retval      0-成功  1-失败
 */
uint8_t onenet_init(void)
{
    uint16_t timeout;
    
    /* 1. 初始化ESP8266并连接WiFi */
    if (esp8266_init() != 0)
    {
        printf("[OneNET] ERROR: ESP8266 initialization failed!\r\n");
        return 1;  /* ESP8266初始化失败 */
    }
    
    printf("[OneNET] ESP8266 initialized\r\n");
    delay_ms(500);  /* 等待WiFi稳定 */
    
    /* 2. 连接到OneNET MQTT服务器 - 最多尝试10次 */
    timeout = 10;
    while (esp8266_send_cmd(ONENET_SERVER_CMD, "CONNECT"))
    {
        printf("[OneNET] Attempting TCP connection... (timeout=%d)\r\n", timeout);
        if (--timeout == 0)
        {
            printf("[OneNET] ERROR: TCP connection failed!\r\n");
            return 2;
        }
        delay_ms(200);
    }
    
    printf("[OneNET] TCP connected successfully\r\n");
    
    /* 等待TCP连接稳定 */
    delay_ms(500);
    
    /* 清理接收缓冲区,避免残留数据干扰 */
    esp8266_clear();
    delay_ms(200);
    
    return 0;
}

/**
 * @brief       连接到OneNET平台 (MQTT CONNECT)
 * @param       无
 * @retval      0-成功  1-失败
 */
uint8_t onenet_connect(void)
{
    uint16_t timeout = 10;
    uint8_t tcp_retry = 0;
    
    /* 调用onenet_protocol中的连接函数 - 最多尝试10次 */
    while (OneNet_DevLink())
    {
        if (--timeout == 0)
        {
            printf("[OneNET] ERROR: MQTT authentication failed!\r\n");
            
            /* 如果是TCP发送失败,尝试重新建立连接(最多1次) */
            if (tcp_retry == 0)
            {
                tcp_retry++;
                printf("[OneNET] Attempting to reconnect TCP...\r\n");
                
                /* 关闭当前连接 */
                esp8266_send_cmd("AT+CIPCLOSE\r\n", "OK");
                delay_ms(200);
                
                /* 重新建立TCP连接 */
                timeout = 10;
                while (esp8266_send_cmd(ONENET_SERVER_CMD, "CONNECT"))
                {
                    if (--timeout == 0)
                    {
                        printf("[OneNET] ERROR: TCP reconnection failed!\r\n");
                        return 1;
                    }
                    delay_ms(200);
                }
                
                printf("[OneNET] TCP reconnected\r\n");
                delay_ms(200);
                esp8266_clear();
                
                /* 重置超时计数,再次尝试MQTT认证 */
                timeout = 10;
                continue;
            }
            
            return 1;
        }
        delay_ms(200);
    }
    
    return 0;
}

/**
 * @brief       订阅OneNET主题
 * @param       无
 * @retval      无
 */
void onenet_subscribe(void)
{
    /* 调用onenet_protocol中的订阅函数 */
    OneNET_Subscribe();
}

/**
 * @brief       上传数据到OneNET平台
 * @param       无
 * @retval      无
 */
void onenet_send_data(void)
{
    /* 调用onenet_protocol中的数据上传函数 */
    OneNet_SendData();
}

/**
 * @brief       处理OneNET平台下发的命令
 * @param       data: 接收到的数据指针
 * @retval      无
 */
void onenet_recv_process(uint8_t *data)
{
    /* 调用onenet_protocol中的命令处理函数 */
    extern uint8_t cmd_processing;
    cmd_processing = 1;  /* 标记命令处理中 */
    
    OneNet_RevPro(data);
    
    cmd_processing = 0;  /* 标记命令处理完成 */
}
