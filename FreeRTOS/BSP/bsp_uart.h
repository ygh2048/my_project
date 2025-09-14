/*
 * bsp_uart.h
 *
 *  Created on: Jul 17, 2024
 *      Author: ygh20
 */

#ifndef BSP_UART_H_
#define BSP_UART_H_

#include "stdint.h"

#define LENGTH 255

#define UART_BUFFER_SIZE        256
#define UART_BUFFER_QUANTITY    16





typedef struct
{
    uint8_t buffer[UART_BUFFER_SIZE];   /* 存放数据的空间 */
    uint16_t size;                      /* 已存放数据的大小 */
}UART_RX_TypeDef;

void USART1_Init(void);
void USART2_Init(void);
void USART3_Init(void);
void USART1_Send_U8(uint8_t ch);
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);
void Serial_Data_Send(uint8_t *BufferPtr, uint16_t Length);
void USART1_DMAHandler(void);
void USART2_DMAHandler(void);
void USART3_DMAHandler(void);

#endif /* BSP_UART_H_ */
