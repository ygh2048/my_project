#ifndef _ONENET_PROTOCOL_H_
#define _ONENET_PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 系统状态结构体声明 */
typedef struct
{
    float voltage;
    float current;
    float power;
    int32_t weight;
    uint8_t is_returned;
    uint8_t human_detected;
    uint8_t light_status;
    uint8_t power_status;
} system_status_t;

/* 内存池函数声明 */
void mem_pool_init(void);              /* 初始化内存池 */
void* mem_pool_malloc(size_t size);    /* 从内存池分配内存 */
void mem_pool_free(void* ptr);         /* 释放内存池中的内存 */

/* OneNET函数声明 */
bool OneNet_DevLink(void);              /* 连接到OneNET平台 */
void OneNet_SendData(void);             /* 上传数据到平台 */
void OneNET_Subscribe(void);            /* 订阅主题 */
void OneNet_RevPro(uint8_t *cmd);       /* 处理平台下发的命令 */

#endif /* _ONENET_PROTOCOL_H_ */
