/**
 ****************************************************************************************************
 * @file        esp8266.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       ESP8266 WiFi模块驱动 头文件
 * @note        使用USART3与ESP8266通信
 *              TX3->PB10, RX3->PB11
 ****************************************************************************************************
 */

#ifndef __ESP8266_H
#define __ESP8266_H

#include "./SYSTEM/sys/sys.h"
#include <stdint.h>

/* ESP8266接收缓冲区大小 */
#define ESP8266_BUF_SIZE    512

/* ESP8266 RST引脚定义 (PA1以上) */
#define ESP8266_RST_PORT    GPIOA
#define ESP8266_RST_PIN     1           /* PA1 */
#define ESP8266_RST_HIGH()  (GPIOA->ODR |= (1 << 1))    /* RST拉高 */
#define ESP8266_RST_LOW()   (GPIOA->ODR &= ~(1 << 1))   /* RST拉低 */

/* 返回值定义 */
#define REV_OK      0       /* 接收完成 */
#define REV_WAIT    1       /* 接收未完成 */

/* ESP8266全局变量 */
extern uint8_t esp8266_buf[ESP8266_BUF_SIZE];
extern uint16_t esp8266_cnt;

/* 函数声明 */
uint8_t esp8266_init(void);                                     /* ESP8266初始化 */
void esp8266_clear(void);                                       /* 清空接收缓冲区 */
uint8_t esp8266_wait_receive(void);                             /* 等待接收完成 */
uint8_t esp8266_send_cmd(char *cmd, char *res);                 /* 发送AT命令 */
uint8_t esp8266_send_data(uint8_t *data, uint16_t len);         /* 发送数据 */
uint8_t *esp8266_get_ipd(uint16_t timeout);                     /* 获取平台返回的数据 */
uint8_t esp8266_check_wifi(void);                               /* 检查WiFi连接状态 */
uint8_t esp8266_check_tcp(void);                                /* 检查TCP连接状态 */
void esp8266_hw_reset_only(void);                               /* 硬件复位 */

#endif /* __ESP8266_H */
