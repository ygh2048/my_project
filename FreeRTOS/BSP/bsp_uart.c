/*
 * bsp_uart.c
 *
 *  Created on: Jul 17, 2024
 *      Author: ygh20
 */
#include "bsp_uart.h"
#include "bsp.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern osMessageQId uart_queueHandle;

#define ENABLE_UART_DMA    0

UART_RX_TypeDef uart_rx_data_t[UART_BUFFER_QUANTITY];
uint8_t uart_buff_ctrl = 0;
uint8_t uart_buff_ctrl_last=0;

// Initialize USART1  初始化串口1
void USART1_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, uart_rx_data_t[uart_buff_ctrl].buffer, UART_BUFFER_SIZE);
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_ERR);
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_PE);
}
// Initialize USART2  初始化串口2
void USART2_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, uart_rx_data_t[uart_buff_ctrl].buffer, UART_BUFFER_SIZE);
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_ERR);
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_PE);
}
// Initialize USART3  初始化串口3
void USART3_Init(void)
{
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart3, uart_rx_data_t[uart_buff_ctrl].buffer, UART_BUFFER_SIZE);
	__HAL_UART_DISABLE_IT(&huart3, UART_IT_ERR);
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_PE);
}

// The serial port sends one byte  串口发送一个字节
void USART1_Send_U8(uint8_t ch)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
}

// The serial port sends a string of data  串口发送一串数据
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
    #if ENABLE_UART_DMA
    while((&huart1)->gState != HAL_UART_STATE_READY);
    HAL_UART_Transmit_DMA(&huart1, BufferPtr, Length);
    #else
    HAL_UART_Transmit(&huart1, BufferPtr, Length, 0xfff);
    #endif
}
/***************************************************************************************
@函数名：Serial_Data_Send(uint8_t *buf, uint32_t cnt)
@入口参数：buf:待发送数据
			     cnt:待发送字长
@出口参数：无
功能描述：串口发送函数
@作者：无名小哥
@日期：2020年01月17日
****************************************************************************************/
void Serial_Data_Send(uint8_t *BufferPtr, uint16_t Length)
{
    while (Length--)
    {
        USART1_Send_U8(*BufferPtr);
        BufferPtr++;
    }
}
void USART1_DMAHandler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) // 判断是否是空闲中断
    {
        UART_RX_TypeDef *pUartData; /* 定义指向创建串口数据的指针 */
        __HAL_UART_CLEAR_IDLEFLAG(&huart1); // 清除空闲中断标志
        HAL_UART_DMAStop(&huart1);          // 停止本次DMA传输

        /* 计算接收到的数据长度，放进串口数据结构体中 */
        uart_rx_data_t[uart_buff_ctrl].size = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
        /* 将这个串口数据的结构体地址给指针 */
        pUartData = &uart_rx_data_t[uart_buff_ctrl];
        /* 把指向串口接收数据的指针放入消息队列。注意，这里传的是指针的地址，也就是指针串口数据结构体的指针的地址不能传pUartData本身 */
        xQueueSendFromISR(uart_queueHandle, &pUartData, NULL);
        /* 这里进行加一，使其下一次DMA传输时将接收的数据放到下一个串口缓冲区中 */
        uart_buff_ctrl_last=uart_buff_ctrl;
        uart_buff_ctrl++;
        /* 取余操作防止越界 */
        uart_buff_ctrl_last%= UART_BUFFER_QUANTITY;
        uart_buff_ctrl %= UART_BUFFER_QUANTITY;
        /* 重启开始DMA传输 */
        HAL_UART_Receive_DMA(&huart1, uart_rx_data_t[uart_buff_ctrl].buffer, UART_BUFFER_SIZE);
    }
}

void USART2_DMAHandler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE)) // 判断是否是空闲中断
    {
        UART_RX_TypeDef *pUartData; /* 定义指向创建串口数据的指针 */
        __HAL_UART_CLEAR_IDLEFLAG(&huart2); // 清除空闲中断标志
        HAL_UART_DMAStop(&huart2);          // 停止本次DMA传输

        /* 计算接收到的数据长度，放进串口数据结构体中 */
        uart_rx_data_t[uart_buff_ctrl].size = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
        /* 将这个串口数据的结构体地址给指针 */
        pUartData = &uart_rx_data_t[uart_buff_ctrl];
        /* 把指向串口接收数据的指针放入消息队列。注意，这里传的是指针的地址，也就是指针串口数据结构体的指针的地址不能传pUartData本身 */
        xQueueSendFromISR(uart_queueHandle, &pUartData, NULL);
        /* 这里进行加一，使其下一次DMA传输时将接收的数据放到下一个串口缓冲区中 */
        uart_buff_ctrl_last=uart_buff_ctrl;
        uart_buff_ctrl++;
        /* 取余操作防止越界 */
        uart_buff_ctrl_last%= UART_BUFFER_QUANTITY;
        uart_buff_ctrl %= UART_BUFFER_QUANTITY;
        /* 重启开始DMA传输 */
        HAL_UART_Receive_DMA(&huart2, uart_rx_data_t[uart_buff_ctrl].buffer, UART_BUFFER_SIZE);
    }
}

void USART3_DMAHandler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE)) // 判断是否是空闲中断
    {
        UART_RX_TypeDef *pUartData; /* 定义指向创建串口数据的指针 */
        __HAL_UART_CLEAR_IDLEFLAG(&huart3); // 清除空闲中断标志
        HAL_UART_DMAStop(&huart3);          // 停止本次DMA传输

        /* 计算接收到的数据长度，放进串口数据结构体中 */
        uart_rx_data_t[uart_buff_ctrl].size = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
        /* 将这个串口数据的结构体地址给指针 */
        pUartData = &uart_rx_data_t[uart_buff_ctrl];
        /* 把指向串口接收数据的指针放入消息队列。注意，这里传的是指针的地址，也就是指针串口数据结构体的指针的地址不能传pUartData本身 */
        xQueueSendFromISR(uart_queueHandle, &pUartData, NULL);
        /* 这里进行加一，使其下一次DMA传输时将接收的数据放到下一个串口缓冲区中 */
        uart_buff_ctrl_last=uart_buff_ctrl;
        uart_buff_ctrl++;
        /* 取余操作防止越界 */
        uart_buff_ctrl_last%= UART_BUFFER_QUANTITY;
        uart_buff_ctrl %= UART_BUFFER_QUANTITY;
        /* 重启开始DMA传输 */
        HAL_UART_Receive_DMA(&huart3, uart_rx_data_t[uart_buff_ctrl].buffer, UART_BUFFER_SIZE);
    }
}


#ifdef __GNUC__//串口重定向
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
