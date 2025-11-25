#include "bluetooth.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

/* 全局变量 */
extern UART_HandleTypeDef huart3;
extern uint8_t bt_rx_buf[64];
extern uint16_t bt_rx_index;

static BT_State_t bt_state = BT_STATE_IDLE;
static uint32_t bt_last_at_time = 0;

/**
 * @brief 蓝牙初始化
 * @param huart: UART句柄（USART3）
 */
void BT_Init(UART_HandleTypeDef *huart)
{
    // 启动UART3接收中断
    HAL_UART_Receive_IT(huart, (uint8_t*)bt_rx_buf, 1);
    
    bt_state = BT_STATE_READY;
    printf("[BT] Bluetooth initialized on USART3 (115200 baud)\r\n");
}

/**
 * @brief 发送AT指令
 * @param cmd: AT指令字符串（必须包含\r\n）
 */
void BT_Send_AT_Command(const char *cmd)
{
    if(cmd == NULL) return;
    
    uint16_t len = strlen(cmd);
    HAL_UART_Transmit(&huart3, (uint8_t*)cmd, len, 100);
    
    printf("[BT_TX] %s", cmd);
}

/**
 * @brief 接收蓝牙响应
 * @param response: 存放响应数据的缓冲区
 * @param timeout: 超时时间(ms)
 * @return 响应结果
 */
BT_Response_t BT_Receive_Response(char *response, uint32_t timeout)
{
    uint32_t start_time = HAL_GetTick();
    uint16_t max_len = 256;
    uint16_t idx = 0;
    
    // 清空缓冲区
    memset(response, 0, max_len);
    
    // 等待响应
    while(HAL_GetTick() - start_time < timeout)
    {
        if(bt_rx_index > 0)
        {
            // 复制数据到响应缓冲区
            if(idx < max_len - 1)
            {
                response[idx++] = bt_rx_buf[bt_rx_index - 1];
            }
            
            // 检查是否收到完整的响应（以\r\n或OK结尾）
            if((idx >= 2 && response[idx-2] == '\r' && response[idx-1] == '\n') ||
               strstr(response, "OK") != NULL)
            {
                printf("[BT_RX] %s", response);
                
                // 清除缓冲区
                bt_rx_index = 0;
                memset(bt_rx_buf, 0, 64);
                
                // 判断响应是否成功
                if(strstr(response, "OK") != NULL)
                    return BT_RESP_OK;
                else if(strstr(response, "ERROR") != NULL)
                    return BT_RESP_ERROR;
            }
        }
        HAL_Delay(10);
    }
    
    return BT_RESP_TIMEOUT;
}

/**
 * @brief 蓝牙模块复位
 */
void BT_Reset(void)
{
    printf("[BT] Sending reset command...\r\n");
    BT_Send_AT_Command(BT_CMD_RESET);
    HAL_Delay(1000);  // 等待模块复位
    printf("[BT] Reset complete\r\n");
}

/**
 * @brief 获取蓝牙模块状态
 */
BT_State_t BT_Get_State(void)
{
    return bt_state;
}

/**
 * @brief 蓝牙模块GPIO初始化（用于进入AT模式）
 * 注：某些蓝牙模块需要在特定状态进入AT模式
 */
void BT_Gpio_Init(void)
{
    // 如果蓝牙模块有专门的AT模式引脚，在此初始化
    // 例如：
    // GPIO_InitTypeDef GPIO_InitStruct = {0};
    // GPIO_InitStruct.Pin = GPIO_PIN_x;
    // GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    // GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOX, &GPIO_InitStruct);
}

/**
 * @brief 进入蓝牙AT模式（通过按键触发）
 * 使用方法：在系统启动时快速按键可进入AT模式
 */
void BT_Enable_AT_Mode(void)
{
    printf("\n[BT] Entering AT mode...\r\n");
    printf("[BT] You can now send AT commands\r\n");
    printf("[BT] Common commands:\r\n");
    printf("  AT                  - Test connection\r\n");
    printf("  AT+VERSION          - Get module version\r\n");
    printf("  AT+NAME?            - Get module name\r\n");
    printf("  AT+NAME=MyDevice    - Set module name\r\n");
    printf("  AT+PSWD?            - Get PIN code\r\n");
    printf("  AT+PSWD=1234        - Set PIN code\r\n");
    printf("  AT+ROLE?            - Get role (0=slave, 1=master)\r\n");
    printf("  AT+ROLE=0           - Set as slave (recommended)\r\n");
    printf("  AT+UART?            - Get baud rate\r\n");
    printf("  AT+ADDR?            - Get Bluetooth address\r\n");
    printf("  AT+RESET            - Reset module\r\n");
}

/**
 * @brief 蓝牙心跳检测（定期发送AT命令测试连接）
 */
void BT_Heartbeat(void)
{
    static uint32_t last_at_time = 0;
    
    // 每10秒发送一次AT测试
    if(HAL_GetTick() - last_at_time >= 10000)
    {
        last_at_time = HAL_GetTick();
        
        // 只在蓝牙未连接时发送
        extern uint8_t bt_connected;
        if(!bt_connected)
        {
            BT_Send_AT_Command(BT_CMD_AT);
        }
    }
}

/**
 * @brief 配置蓝牙模块（推荐设置）
 * 此函数执行一系列AT指令来配置蓝牙模块
 * 需要模块处于AT模式
 */
void BT_Configure_Device(const char *device_name, const char *pin_code)
{
    char cmd[64];
    char response[256];
    
    printf("\n[BT] Starting configuration...\r\n");
    
    // 1. 测试连接
    printf("[BT] Step 1: Testing connection\r\n");
    BT_Send_AT_Command(BT_CMD_AT);
    BT_Receive_Response(response, 500);
    HAL_Delay(200);
    
    // 2. 获取版本
    printf("[BT] Step 2: Getting version\r\n");
    BT_Send_AT_Command(BT_CMD_VERSION);
    BT_Receive_Response(response, 500);
    HAL_Delay(200);
    
    // 3. 设置设备名称
    if(device_name != NULL)
    {
        printf("[BT] Step 3: Setting device name to '%s'\r\n", device_name);
        snprintf(cmd, sizeof(cmd), "AT+NAME=%s\r\n", device_name);
        BT_Send_AT_Command(cmd);
        BT_Receive_Response(response, 500);
        HAL_Delay(200);
    }
    
    // 4. 设置PIN码
    if(pin_code != NULL)
    {
        printf("[BT] Step 4: Setting PIN code to '%s'\r\n", pin_code);
        snprintf(cmd, sizeof(cmd), "AT+PSWD=%s\r\n", pin_code);
        BT_Send_AT_Command(cmd);
        BT_Receive_Response(response, 500);
        HAL_Delay(200);
    }
    
    // 5. 设置为从机模式（推荐作为被控设备）
    printf("[BT] Step 5: Setting as slave mode\r\n");
    BT_Send_AT_Command(BT_CMD_ROLE_SET_SLAVE);
    BT_Receive_Response(response, 500);
    HAL_Delay(200);
    
    // 6. 获取蓝牙地址
    printf("[BT] Step 6: Getting Bluetooth address\r\n");
    BT_Send_AT_Command(BT_CMD_ADDR);
    BT_Receive_Response(response, 500);
    HAL_Delay(200);
    
    // 7. 获取当前设置确认
    printf("[BT] Step 7: Verifying configuration\r\n");
    BT_Send_AT_Command(BT_CMD_NAME_GET);
    BT_Receive_Response(response, 500);
    
    printf("[BT] Configuration complete!\r\n");
    printf("[BT] Module will restart and enter normal mode\r\n");
    
    // 复位模块以使配置生效
    HAL_Delay(1000);
    BT_Reset();
}
