#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "stm32f1xx_hal.h"

/* 蓝牙模块状态定义 */
typedef enum {
    BT_STATE_IDLE = 0,           // 空闲状态
    BT_STATE_INIT = 1,           // 初始化中
    BT_STATE_READY = 2,          // 就绪状态
    BT_STATE_ERROR = 3           // 错误状态
} BT_State_t;

/* 蓝牙AT指令响应结果 */
typedef enum {
    BT_RESP_OK = 0,              // 命令成功
    BT_RESP_ERROR = 1,           // 命令失败
    BT_RESP_TIMEOUT = 2          // 超时
} BT_Response_t;

/* 蓝牙配置参数结构体 */
typedef struct {
    char name[32];               // 蓝牙模块名称
    char pin[6];                 // 配对PIN码
    uint32_t baudrate;           // 波特率
    uint8_t master;              // 1=主机模式, 0=从机模式
    uint8_t role;                // 0=SPP模式, 1=A2DP, 2=HFP
} BT_Config_t;

/* 公开函数声明 */
void BT_Init(UART_HandleTypeDef *huart);              // 蓝牙初始化
void BT_Send_AT_Command(const char *cmd);             // 发送AT指令
BT_Response_t BT_Receive_Response(char *response, uint32_t timeout);  // 接收响应
void BT_Enable_AT_Mode(void);                         // 进入AT模式（按键方式）
void BT_Reset(void);                                  // 软件复位
BT_State_t BT_Get_State(void);                        // 获取蓝牙状态
void BT_Gpio_Init(void);                              // GPIO初始化

/* AT指令宏定义 */
#define BT_CMD_AT               "AT\r\n"              // 测试连接
#define BT_CMD_RESET            "AT+RESET\r\n"        // 软件复位
#define BT_CMD_VERSION          "AT+VERSION\r\n"      // 获取版本
#define BT_CMD_NAME_GET         "AT+NAME?\r\n"        // 查询蓝牙名称
#define BT_CMD_NAME_SET(name)   "AT+NAME="name"\r\n" // 设置蓝牙名称
#define BT_CMD_PIN_GET          "AT+PSWD?\r\n"        // 查询PIN码
#define BT_CMD_PIN_SET(pin)     "AT+PSWD="pin"\r\n"  // 设置PIN码
#define BT_CMD_BAUD_GET         "AT+UART?\r\n"        // 查询波特率
#define BT_CMD_BAUD_SET(baud)   "AT+UART="baud"\r\n" // 设置波特率
#define BT_CMD_ROLE_GET         "AT+ROLE?\r\n"        // 查询角色(主/从)
#define BT_CMD_ROLE_SET_MASTER  "AT+ROLE=1\r\n"       // 设置主机模式
#define BT_CMD_ROLE_SET_SLAVE   "AT+ROLE=0\r\n"       // 设置从机模式
#define BT_CMD_CLASS_GET        "AT+CLASS?\r\n"       // 查询设备类
#define BT_CMD_ADDR             "AT+ADDR?\r\n"        // 获取蓝牙地址
#define BT_CMD_CONNECT          "AT+CCON?\r\n"        // 查询连接状态
#define BT_CMD_BIND(addr)       "AT+BIND="addr"\r\n" // 绑定设备地址

#endif // __BLUETOOTH_H__
