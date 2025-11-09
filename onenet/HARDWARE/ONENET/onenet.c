/**
 * @file onenet.c
 * @author Smart Lab System
 * @version V1.0
 * @date 2025-11-05
 * @brief OneNET物联网平台驱动实现文件
 * @note 设备ID: 489VMXhRtU
 *       产品名称: WORK
 */

#include "onenet.h"
#include "onenet_protocol.h"
#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "app_runtime.h"
#include <stdio.h>
#include <string.h>

/* OneNET服务器连接命令 */
#define ONENET_SERVER_CMD   "AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

/* OneNET MQTT主题 */
#define MQTT_TOPIC_PROP_POST    "$sys/489VMXhRtU/WORK/thing/property/post"
#define MQTT_TOPIC_PROP_SET     "$sys/489VMXhRtU/WORK/thing/property/set"
#define MQTT_TOPIC_PROP_SET_REPLY "$sys/489VMXhRtU/WORK/thing/property/set_reply"

/**
 * @brief OneNET初始化
 * @param 无
 * @retval 0-成功 1-失败
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
    delay_ms(500);  /* 等待WiFi连接完成 */

    /* 2. 连接OneNET MQTT服务器 - 最多重试10次 */
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

    /* 清空接收缓冲区 */
    esp8266_clear();
    delay_ms(200);

    return 0;
}

/**
 * @brief 连接OneNET平台(MQTT CONNECT)
 * @param 无
 * @retval 0-成功 1-失败
 */
uint8_t onenet_connect(void)
{
    uint16_t timeout = 10;
    uint8_t tcp_retry = 0;

    /* 调用onenet_protocol中的设备连接函数 - 最多重试10次 */
    while (OneNet_DevLink())
    {
        if (--timeout == 0)
        {
            printf("[OneNET] ERROR: MQTT authentication failed!\r\n");

            /* 如果MQTT认证失败，尝试重新连接TCP */
            if (tcp_retry == 0)
            {
                tcp_retry++;
                printf("[OneNET] Attempting to reconnect TCP...\r\n");

                /* 关闭TCP连接 */
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

                /* 重新尝试MQTT连接 */
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
 * @brief 订阅OneNET主题
 * @param 无
 * @retval 无
 */
void onenet_subscribe(void)
{
    /* 调用onenet_protocol中的订阅函数 */
    OneNET_Subscribe();
}

/**
 * @brief 发送数据到OneNET平台
 * @param 无
 * @retval 无
 */
void onenet_send_data(void)
{
    /* 调用onenet_protocol中的发送数据函数 */
    OneNet_SendData();
}

/**
 * @brief 处理OneNET平台接收到的命令
 * @param data: 接收到的数据
 * @retval 无
 */
void onenet_recv_process(uint8_t *data)
{
    /* 调用onenet_protocol中的数据处理函数 */
    app_runtime_begin_command();

    OneNet_RevPro(data);

    app_runtime_end_command();
}


