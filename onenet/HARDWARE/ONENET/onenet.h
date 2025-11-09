/**
 ****************************************************************************************************
 * @file        onenet.h
 * @author      Smart Lab System  
 * @version     V1.0
 * @date        2025-11-05
 * @brief       OneNET鐗╄仈缃戝钩鍙版帴鍏?澶存枃浠?
 * @note        浜у搧ID: 489VMXhRtU
 *              璁惧鍚嶇О: WORK
 ****************************************************************************************************
 */

#ifndef __ONENET_H
#define __ONENET_H

#include "sys.h"
#include <stdint.h>

/* OneNET骞冲彴閰嶇疆淇℃伅 */
#define ONENET_PRODUCT_ID       "489VMXhRtU"
#define ONENET_ACCESS_KEY       "DZ2LW8WLi195ITVn6dMjwZ8lS4cvdQfxaas/ww1vBWU="
#define ONENET_DEVICE_NAME      "WORK"

/* OneNET鏈嶅姟鍣ㄥ湴鍧€鍜岀鍙?*/
#define ONENET_SERVER_IP        "mqtts.heclouds.com"
#define ONENET_SERVER_PORT      1883

/* 鍔熻兘鐐规爣璇嗙 */
#define ONENET_PROP_LIGHT       "light"         /* 鐓ф槑寮€鍏?*/
#define ONENET_PROP_OPEN_CLOSE  "open_close"    /* 鎬荤數婧愬紑鍏?*/
#define ONENET_PROP_VOLTAGE     "mv"            /* 鐢靛帇(mV) */
#define ONENET_PROP_CURRENT     "ma"            /* 鐢垫祦(mA) */
#define ONENET_PROP_POWER       "mp"            /* 鍔熺巼(mW) */
#define ONENET_PROP_WEIGHT      "weight"        /* 璐ㄩ噺(g) */

/* 鍑芥暟澹版槑 */
uint8_t onenet_init(void);                              /* OneNET鍒濆鍖?*/
uint8_t onenet_connect(void);                           /* 杩炴帴鍒癘neNET骞冲彴 */
void onenet_send_data(void);                            /* 涓婁紶鏁版嵁鍒板钩鍙?*/
void onenet_recv_process(uint8_t *data);                /* 澶勭悊骞冲彴涓嬪彂鐨勫懡浠?*/
void onenet_subscribe(void);                            /* 璁㈤槄涓婚 */

#endif /* __ONENET_H */
