#ifndef _ONENET_PROTOCOL_H_
#define _ONENET_PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "app_runtime.h"

/* 鍐呭瓨姹犲嚱鏁板０鏄?*/
void mem_pool_init(void);              /* 鍒濆鍖栧唴瀛樻睜 */
void* mem_pool_malloc(size_t size);    /* 浠庡唴瀛樻睜鍒嗛厤鍐呭瓨 */
void mem_pool_free(void* ptr);         /* 閲婃斁鍐呭瓨姹犱腑鐨勫唴瀛?*/

/* OneNET鍑芥暟澹版槑 */
bool OneNet_DevLink(void);              /* 杩炴帴鍒癘neNET骞冲彴 */
void OneNet_SendData(void);             /* 涓婁紶鏁版嵁鍒板钩鍙?*/
void OneNET_Subscribe(void);            /* 璁㈤槄涓婚 */
void OneNet_RevPro(uint8_t *cmd);       /* 澶勭悊骞冲彴涓嬪彂鐨勫懡浠?*/

#endif /* _ONENET_PROTOCOL_H_ */
