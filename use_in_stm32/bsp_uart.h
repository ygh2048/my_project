#ifndef BSP_UART_H_
#define BSP_UART_H_

#include "stdint.h"


#define UART_BUFFER_SIZE        128
#define UART_BUFFER_QUANTITY    16

typedef struct
{
    uint8_t buffer[UART_BUFFER_SIZE];   /* 存放数据的空间 */
    uint16_t size;                      /* 已存放数据的大小 */
}UART_RX_TypeDef;


extern uint8_t uart_buff_ctrl;
extern uint8_t uart_buff_ctrl_last;
extern UART_RX_TypeDef uart_rx_data_t[UART_BUFFER_QUANTITY];
void USART1_Init(void);


void USART1_Send_U8(uint8_t ch);
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);
void Serial_Data_Send(uint8_t *BufferPtr, uint16_t Length);
void USART1_DMAHandler(void);

#endif

