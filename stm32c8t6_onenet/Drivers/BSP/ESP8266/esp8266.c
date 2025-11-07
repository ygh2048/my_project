/**
 ****************************************************************************************************
 * @file        esp8266.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       ESP8266 WiFi模块驱动 实现文件
 * @note        使用USART3与ESP8266通信 (TX3->PB10, RX3->PB11)
 *              WiFi SSID: mytask
 *              WiFi Password: 12345678
 ****************************************************************************************************
 */

#include "./BSP/ESP8266/esp8266.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_misc.h"
#include <string.h>
#include <stdio.h>

/* WiFi配置信息 */
#define ESP8266_WIFI_SSID       "mytask"
#define ESP8266_WIFI_PWD        "12345678"
#define ESP8266_WIFI_INFO       "AT+CWJAP=\"" ESP8266_WIFI_SSID "\",\"" ESP8266_WIFI_PWD "\"\r\n"

/* 全局变量 */
uint8_t esp8266_buf[ESP8266_BUF_SIZE];
uint16_t esp8266_cnt = 0;
static uint16_t esp8266_cnt_pre = 0;

/* USART3 GPIO配置 (PB10=TX, PB11=RX) */
#define ESP8266_USART               USART3

/**
 * @brief       ESP8266 RST引脚初始化 (PA1)
 * @param       无
 * @retval      无
 */
static void esp8266_rst_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    /* PA1配置为推挽输出模式 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    /* 默认RST拉高 */
    ESP8266_RST_HIGH();
    
    printf("[ESP8266] RST pin initialized (PA1)\r\n");
}

/**
 * @brief       ESP8266硬件复位
 * @param       无
 * @retval      无
 */
static void esp8266_hw_reset(void)
{
    //printf("[ESP8266] Performing hardware reset...\r\n");
    
    /* RST拉低 */
    ESP8266_RST_LOW();
    delay_ms(100);              /* 保持低电平100ms */
    
    /* RST拉高，使ESP8266复位 */
    ESP8266_RST_HIGH();
    delay_ms(1000);             /* 等待ESP8266启动 */
    
    printf("[ESP8266] Hardware reset complete\r\n");
}

/**
 * @brief       USART3初始化 (用于ESP8266)
 * @param       bound: 波特率 (推荐115200)
 * @retval      无
 */
static void usart3_init(uint32_t bound)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    
    /* GPIO配置: PB10=TX(复用推挽), PB11=RX(浮空输入) */
    /* PB10: TX3 - 复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* PB11: RX3 - 浮空输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* USART3配置 */
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);
    
    /* 使能USART3接收中断 */
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    
    /* 配置NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /* 使能USART3 */
    USART_Cmd(USART3, ENABLE);
}

/**
 * @brief       USART3中断服务函数
 * @param       无
 * @retval      无
 */
void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  /* 接收到数据 */
    {
        if (esp8266_cnt < ESP8266_BUF_SIZE)
        {
            esp8266_buf[esp8266_cnt++] = USART_ReceiveData(USART3);
        }
        else
        {
            USART_ReceiveData(USART3);  /* 读取数据寄存器清除标志 */
            esp8266_cnt = 0;
        }
    }
}

/**
 * @brief       通过USART3发送一个字节
 * @param       data: 要发送的字节
 * @retval      无
 */
static void usart3_send_byte(uint8_t data)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);  /* 等待发送缓冲区空 */
    USART_SendData(USART3, data);
}

/**
 * @brief       通过USART3发送字符串
 * @param       str: 字符串指针
 * @param       len: 字符串长度
 * @retval      无
 */
static void usart3_send_string(uint8_t *str, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        usart3_send_byte(str[i]);
    }
}

static uint8_t esp8266_response_has_error(const char *resp)
{
    if (resp == NULL)
    {
        return 1;
    }
    
    if (strstr(resp, "ERROR") != NULL ||
        strstr(resp, "FAIL") != NULL ||
        strstr(resp, "ALREADY CONNECT") != NULL)
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief       清空ESP8266接收缓冲区
 * @param       无
 * @retval      无
 */
void esp8266_clear(void)
{
    memset(esp8266_buf, 0, sizeof(esp8266_buf));
    esp8266_cnt = 0;
    esp8266_cnt_pre = 0;
}

/**
 * @brief       等待ESP8266接收完成
 * @param       无
 * @retval      REV_OK-接收完成  REV_WAIT-等待中
 */
uint8_t esp8266_wait_receive(void)
{
    if (esp8266_cnt == 0)
        return REV_WAIT;

    if (esp8266_cnt == esp8266_cnt_pre)
    {
        esp8266_cnt = 0;
        return REV_OK;
    }

    esp8266_cnt_pre = esp8266_cnt;
    return REV_WAIT;
}

/**
 * @brief       发送AT命令到ESP8266
 * @param       cmd: 命令字符串
 *              res: 期望的响应字符串
 * @retval      0-成功  1-失败
 */
uint8_t esp8266_send_cmd(char *cmd, char *res)
{
    uint8_t timeout = 200;
    uint8_t ret;

    esp8266_clear();  /* 发送前先清一次缓冲 */
    usart3_send_string((uint8_t *)cmd, strlen(cmd));

    while (timeout--)
    {
        ret = esp8266_wait_receive();
        if (ret == REV_OK)
        {
            const char *resp = (const char *)esp8266_buf;
            
            if (esp8266_response_has_error(resp))
            {
                return 1;  /* 收到了失败响应 */
            }
            
            if (strstr(resp, res) != NULL)
            {
                return 0;  /* 成功收到响应 */
            }
        }
        delay_ms(10);
    }

    return 1;  /* 超时失败 */
}

/**
 * @brief       向ESP8266发送数据
 * @param       data: 数据指针
 *              len: 数据长度
 * @retval      0-成功  1-失败
 */
uint8_t esp8266_send_data(uint8_t *data, uint16_t len)
{
    char cmd_buf[32];
    uint8_t retry = 3;  /* 最多重试3次 */
    
    /* 发送前先清理缓冲区，等待ESP8266准备好 */
    delay_ms(100);
    esp8266_clear();
    delay_ms(50);
    
    while (retry--)
    {
        memset(cmd_buf, 0, sizeof(cmd_buf));
        sprintf(cmd_buf, "AT+CIPSEND=%d\r\n", len);
        
        if (!esp8266_send_cmd(cmd_buf, ">"))
        {
            /* 收到 > 提示符，可以发送数据 */
            delay_ms(50);  /* 确保提示符已完全处理 */
            usart3_send_string(data, len);
            delay_ms(100);  /* 等待数据发送完成 */
            printf("[ESP8266] Sent %d bytes\r\n", len);
            return 0;  /* 发送成功 */
        }
        else
        {
            /* 如果还有重试机会，清理状态并重试 */
            if (retry > 0)
            {
                delay_ms(300);
                esp8266_clear();
                delay_ms(100);
                /* 尝试发送Escape序列退出发送模式 */
                usart3_send_string((uint8_t *)"\x1B", 1);  /* ESC键 */
                delay_ms(150);
                esp8266_clear();
                delay_ms(300);
            }
        }
    }
    
    printf("[ESP8266] ERROR: Send failed after 3 retries - recovery and returning!\r\n");
    printf("[DEBUG] [ESP8266_SendData] Clearing buffer and recovering system state\r\n");
    /* 最后清理一次缓冲区，让系统能够恢复 */
    esp8266_clear();
    delay_ms(200);
    return 1;  /* 发送失败但不卡死 */
}

/**
 * @brief       获取平台返回的数据
 * @param       timeout: 等待超时时间(单位10ms)
 * @retval      平台返回的原始数据指针
 */
uint8_t *esp8266_get_ipd(uint16_t timeout)
{
    char *ptr_ipd = NULL;

    do
    {
        if (esp8266_wait_receive() == REV_OK)
        {
            ptr_ipd = strstr((char *)esp8266_buf, "+IPD,");
            if (ptr_ipd)
            {
                ptr_ipd = strchr(ptr_ipd, ':');
                if (ptr_ipd)
                {
                    ptr_ipd++;
                    return (uint8_t *)ptr_ipd;
                }
            }
        }
        delay_ms(10);
    } while (timeout--);
    
    return NULL;
}

/**
 * @brief       ESP8266初始化
 * @param       无
 * @retval      无
 */
uint8_t esp8266_init(void)
{
    uint16_t timeout;
    
    /* 初始化RST引脚 */
    esp8266_rst_init();
    delay_ms(100);
    
    /* 执行硬件复位 */
    esp8266_hw_reset();
    
    /* 初始化USART3 */
    usart3_init(115200);
    esp8266_clear();

    /* 退出透传模式 */
    delay_ms(500);
    usart3_send_string((uint8_t *)"+++", 3);
    delay_ms(500);

    /* 测试AT命令 - 最多尝试10次 */
    timeout = 10;
    while (esp8266_send_cmd("AT\r\n", "OK"))
    {
        if (--timeout == 0)
        {
            printf("[ESP8266] Init failed\r\n");
            return 1;
        }
        delay_ms(500);
    }

    /* 关闭回显 */
    esp8266_send_cmd("ATE0\r\n", "OK");

    /* 设置为Station模式 - 最多尝试5次 */
    timeout = 5;
    while (esp8266_send_cmd("AT+CWMODE=1\r\n", "OK"))
    {
        if (--timeout == 0)
        {
            printf("[ESP8266] ERROR: Failed to set station mode!\r\n");
            return 2;
        }
        delay_ms(500);
    }

    /* 连接WiFi - 最多尝试10次 */
    timeout = 10;
    while (esp8266_send_cmd(ESP8266_WIFI_INFO, "GOT IP"))
    {
        if (--timeout == 0)
        {
            printf("[ESP8266] ERROR: WiFi connection failed!\r\n");
            return 3;
        }
        delay_ms(2000);
    }
    printf("[ESP8266] WiFi connected!\r\n");

    /* 设置单连接模式 */
    esp8266_send_cmd("AT+CIPMUX=0\r\n", "OK");
    
    return 0;  /* 初始化成功 */
}

/**
 * @brief       仅执行硬件复位(不重新初始化USART)
 * @param       无
 * @retval      无
 */
void esp8266_hw_reset_only(void)
{
    printf("[ESP8266] Performing hardware reset only (PA1)...\r\n");
    ESP8266_RST_LOW();
    delay_ms(100);
    ESP8266_RST_HIGH();
    delay_ms(2000);
    printf("[ESP8266] Hardware reset complete\r\n");
    esp8266_clear();
}

/**
 * @brief       检查WiFi连接状态
 * @param       无
 * @retval      0-已连接, 1-未连接
 */
uint8_t esp8266_check_wifi(void)
{
    /* 检查缓冲区中是否有"WIFI DISCONNECT"消息 */
    char *p = (char *)esp8266_buf;
    if (strstr(p, "WIFI DISCONNECT") != NULL || strstr(p, "CLOSED") != NULL)
    {
        printf("WiFi lost\r\n");
        return 1;  /* WiFi已断开 */
    }
    
    return 0;  /* WiFi仍然连接 */
}

/**
 * @brief       检查TCP连接状态
 * @param       无
 * @retval      0-已连接, 1-未连接
 */
uint8_t esp8266_check_tcp(void)
{
    /* 检查缓冲区中是否有"CLOSED"或"ERROR"消息 */
    char *p = (char *)esp8266_buf;
    if (strstr(p, "CLOSED") != NULL || strstr(p, "CONNECT FAIL") != NULL)
    {
        printf("TCP lost\r\n");
        return 1;  /* TCP已断开 */
    }
    
    return 0;  /* TCP仍然连接 */
}
