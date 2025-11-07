/**
 ****************************************************************************************************
 * @file        onenet.h
 * @author      Smart Lab System  
 * @version     V1.0
 * @date        2025-11-05
 * @brief       OneNET物联网平台接入 头文件
 * @note        产品ID: 489VMXhRtU
 *              设备名称: WORK
 ****************************************************************************************************
 */

#ifndef __ONENET_H
#define __ONENET_H

#include "./SYSTEM/sys/sys.h"
#include <stdint.h>

/* OneNET平台配置信息 */
#define ONENET_PRODUCT_ID       "489VMXhRtU"
#define ONENET_ACCESS_KEY       "DZ2LW8WLi195ITVn6dMjwZ8lS4cvdQfxaas/ww1vBWU="
#define ONENET_DEVICE_NAME      "WORK"

/* OneNET服务器地址和端口 */
#define ONENET_SERVER_IP        "mqtts.heclouds.com"
#define ONENET_SERVER_PORT      1883

/* 功能点标识符 */
#define ONENET_PROP_LIGHT       "light"         /* 照明开关 */
#define ONENET_PROP_OPEN_CLOSE  "open_close"    /* 总电源开关 */
#define ONENET_PROP_VOLTAGE     "mv"            /* 电压(mV) */
#define ONENET_PROP_CURRENT     "ma"            /* 电流(mA) */
#define ONENET_PROP_POWER       "mp"            /* 功率(mW) */
#define ONENET_PROP_WEIGHT      "weight"        /* 质量(g) */

/* 函数声明 */
uint8_t onenet_init(void);                              /* OneNET初始化 */
uint8_t onenet_connect(void);                           /* 连接到OneNET平台 */
void onenet_send_data(void);                            /* 上传数据到平台 */
void onenet_recv_process(uint8_t *data);                /* 处理平台下发的命令 */
void onenet_subscribe(void);                            /* 订阅主题 */

#endif /* __ONENET_H */
