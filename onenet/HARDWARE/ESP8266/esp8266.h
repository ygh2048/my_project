/**
 ****************************************************************************************************
 * @file        esp8266.h
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-05
 * @brief       ESP8266 WiFi妯″潡椹卞姩 澶存枃浠?
 * @note        浣跨敤USART3涓嶦SP8266閫氫俊
 *              TX3->PB10, RX3->PB11
 ****************************************************************************************************
 */

#ifndef __ESP8266_H
#define __ESP8266_H

#include "sys.h"
#include <stdint.h>

/* ESP8266鎺ユ敹缂撳啿鍖哄ぇ灏?*/
#define ESP8266_BUF_SIZE    512

/* ESP8266 RST引脚定义 (PA1以上) */
#define ESP8266_RST_PORT    GPIOA
#define ESP8266_RST_PIN     1           /* PA1 */
#define ESP8266_RST_HIGH()  (GPIOA->ODR |= (1 << 1))    /* RST拉高 */
#define ESP8266_RST_LOW()   (GPIOA->ODR &= ~(1 << 1))   /* RST拉低 */

/* 返回值定义 */
#define REV_OK      0       /* 接收完成 */
#define REV_WAIT    1       /* 接收未完成 */

/* ESP8266鍏ㄥ眬鍙橀噺 */
extern uint8_t esp8266_buf[ESP8266_BUF_SIZE];
extern uint16_t esp8266_cnt;

/* 鍑芥暟澹版槑 */
uint8_t esp8266_init(void);                                     /* ESP8266鍒濆鍖?*/
void esp8266_clear(void);                                       /* 娓呯┖鎺ユ敹缂撳啿鍖?*/
uint8_t esp8266_wait_receive(void);                             /* 绛夊緟鎺ユ敹瀹屾垚 */
uint8_t esp8266_send_cmd(char *cmd, char *res);                 /* 鍙戦€丄T鍛戒护 */
uint8_t esp8266_send_data(uint8_t *data, uint16_t len);         /* 鍙戦€佹暟鎹?*/
uint8_t *esp8266_get_ipd(uint16_t timeout);                     /* 鑾峰彇骞冲彴杩斿洖鐨勬暟鎹?*/
uint8_t esp8266_check_wifi(void);                               /* 妫€鏌iFi杩炴帴鐘舵€?*/
uint8_t esp8266_check_tcp(void);                                /* 妫€鏌CP杩炴帴鐘舵€?*/
void esp8266_hw_reset_only(void);                               /* 纭欢澶嶄綅 */

#endif /* __ESP8266_H */
