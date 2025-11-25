/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#if 1               
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
////重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0); //使用串口1   
	USART1->DR = (uint8_t) ch;      
	return ch;
}
extern u16 receive_cnt;
extern u16 distance;
static const uint8_t CrcTable[256] =
{
0x00, 0x4d, 0x9a, 0xd7, 0x79, 0x34, 0xe3,
0xae, 0xf2, 0xbf, 0x68, 0x25, 0x8b, 0xc6, 0x11, 0x5c, 0xa9, 0xe4, 0x33,
0x7e, 0xd0, 0x9d, 0x4a, 0x07, 0x5b, 0x16, 0xc1, 0x8c, 0x22, 0x6f, 0xb8,
0xf5, 0x1f, 0x52, 0x85, 0xc8, 0x66, 0x2b, 0xfc, 0xb1, 0xed, 0xa0, 0x77,
0x3a, 0x94, 0xd9, 0x0e, 0x43, 0xb6, 0xfb, 0x2c, 0x61, 0xcf, 0x82, 0x55,
0x18, 0x44, 0x09, 0xde, 0x93, 0x3d, 0x70, 0xa7, 0xea, 0x3e, 0x73, 0xa4,
0xe9, 0x47, 0x0a, 0xdd, 0x90, 0xcc, 0x81, 0x56, 0x1b, 0xb5, 0xf8, 0x2f,
0x62, 0x97, 0xda, 0x0d, 0x40, 0xee, 0xa3, 0x74, 0x39, 0x65, 0x28, 0xff,
0xb2, 0x1c, 0x51, 0x86, 0xcb, 0x21, 0x6c, 0xbb, 0xf6, 0x58, 0x15, 0xc2,
0x8f, 0xd3, 0x9e, 0x49, 0x04, 0xaa, 0xe7, 0x30, 0x7d, 0x88, 0xc5, 0x12,
0x5f, 0xf1, 0xbc, 0x6b, 0x26, 0x7a, 0x37, 0xe0, 0xad, 0x03, 0x4e, 0x99,
0xd4, 0x7c, 0x31, 0xe6, 0xab, 0x05, 0x48, 0x9f, 0xd2, 0x8e, 0xc3, 0x14,
0x59, 0xf7, 0xba, 0x6d, 0x20, 0xd5, 0x98, 0x4f, 0x02, 0xac, 0xe1, 0x36,
0x7b, 0x27, 0x6a, 0xbd, 0xf0, 0x5e, 0x13, 0xc4, 0x89, 0x63, 0x2e, 0xf9,
0xb4, 0x1a, 0x57, 0x80, 0xcd, 0x91, 0xdc, 0x0b, 0x46, 0xe8, 0xa5, 0x72,
0x3f, 0xca, 0x87, 0x50, 0x1d, 0xb3, 0xfe, 0x29, 0x64, 0x38, 0x75, 0xa2,
0xef, 0x41, 0x0c, 0xdb, 0x96, 0x42, 0x0f, 0xd8, 0x95, 0x3b, 0x76, 0xa1,
0xec, 0xb0, 0xfd, 0x2a, 0x67, 0xc9, 0x84, 0x53, 0x1e, 0xeb, 0xa6, 0x71,
0x3c, 0x92, 0xdf, 0x08, 0x45, 0x19, 0x54, 0x83, 0xce, 0x60, 0x2d, 0xfa,
0xb7, 0x5d, 0x10, 0xc7, 0x8a, 0x24, 0x69, 0xbe, 0xf3, 0xaf, 0xe2, 0x35,
0x78, 0xd6, 0x9b, 0x4c, 0x01, 0xf4, 0xb9, 0x6e, 0x23, 0x8d, 0xc0, 0x17,
0x5a, 0x06, 0x4b, 0x9c, 0xd1, 0x7f, 0x32, 0xe5, 0xa8
};//用于crc校验的数组

uint8_t Usart1_Receive_buf[1];          // 串口1接收中断数据存放的缓冲区
uint8_t Uart2_Receive_buf[1];           // 串口2(激光测距)接收中断数据存放的缓冲区
uint8_t Uart3_Receive_buf[1];           // 串口3(蓝牙)接收中断缓冲区

// 蓝牙接收缓冲区和变量
#define BT_RX_BUF_SIZE 64
uint8_t bt_rx_buf[BT_RX_BUF_SIZE];     // 蓝牙接收缓冲区
uint16_t bt_rx_index = 0;              // 蓝牙接收索引
uint8_t bt_connected = 0;              // 蓝牙连接状态

// 激光接收诊断变量
uint32_t usart2_byte_count = 0;        // USART2接收到的字节总数
uint32_t usart2_interrupt_count = 0;   // USART2中断触发次数

#endif

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 921600;  // STP-23激光传感器标准波特率
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  HAL_UART_Receive_IT(&huart2, Uart2_Receive_buf, sizeof(Uart2_Receive_buf)); // 打开串口2接收中断用于激光测距
  /* USER CODE END USART2_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;  // 蓝牙模块波特率 9600
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
 HAL_UART_Receive_IT(&huart3,Uart3_Receive_buf,sizeof(Uart3_Receive_buf)); //打开串口3接收中断(用于蓝牙单字节接收)
 // 同时启动蓝牙的DMA接收或缓冲区接收
 memset(bt_rx_buf, 0, BT_RX_BUF_SIZE);
 bt_rx_index = 0;
  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

char USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint16_t point1 ;
LiDARFrameTypeDef Pack_Data;//雷达接收的数据储存在这个变量之中
extern u16 receive_cnt;
extern u16 distance;


static void STP23_ParseByte(uint8_t temp_data)
{
    static uint8_t state = 0;
    static uint8_t crc = 0;
    static uint8_t cnt = 0;

    if (state > 5)
    {
        if (state < 42)
        {
            switch (state % 3)
            {
                case 0:
                    Pack_Data.point[cnt].distance = (u16)temp_data;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
                case 1:
                    Pack_Data.point[cnt].distance = ((u16)temp_data << 8) + Pack_Data.point[cnt].distance;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
                default:
                    Pack_Data.point[cnt].intensity = temp_data;
                    cnt++;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
            }
        }
        else
        {
            switch (state)
            {
                case 42:
                    Pack_Data.end_angle = (u16)temp_data;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
                case 43:
                    Pack_Data.end_angle = ((u16)temp_data << 8) + Pack_Data.end_angle;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
                case 44:
                    Pack_Data.timestamp = (u16)temp_data;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
                case 45:
                    Pack_Data.timestamp = ((u16)temp_data << 8) + Pack_Data.timestamp;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                    break;
                case 46:
                    Pack_Data.crc8 = temp_data;
                    if (Pack_Data.crc8 == crc)
                    {
                        data_process();
                        receive_cnt++;
                    }
                    crc = 0;
                    state = 0;
                    cnt = 0;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        switch (state)
        {
            case 0:
                if (temp_data == HEADER)
                {
                    Pack_Data.header = temp_data;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                }
                else
                {
                    state = 0;
                    crc = 0;
                }
                break;
            case 1:
                if (temp_data == VERLEN)
                {
                    Pack_Data.ver_len = temp_data;
                    state++;
                    crc = CrcTable[(crc ^ temp_data) & 0xff];
                }
                else
                {
                    state = 0;
                    crc = 0;
                }
                break;
            case 2:
                Pack_Data.temperature = (u16)temp_data;
                state++;
                crc = CrcTable[(crc ^ temp_data) & 0xff];
                break;
            case 3:
                Pack_Data.temperature = ((u16)temp_data << 8) + Pack_Data.temperature;
                state++;
                crc = CrcTable[(crc ^ temp_data) & 0xff];
                break;
            case 4:
                Pack_Data.start_angle = (u16)temp_data;
                state++;
                crc = CrcTable[(crc ^ temp_data) & 0xff];
                break;
            case 5:
                Pack_Data.start_angle = ((u16)temp_data << 8) + Pack_Data.start_angle;
                state++;
                crc = CrcTable[(crc ^ temp_data) & 0xff];
                break;
            default:
                break;
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        usart2_interrupt_count++;  // 诊断：记录中断触发次数
        usart2_byte_count++;       // 诊断：记录接收字节数
        
        STP23_ParseByte(Uart2_Receive_buf[0]);
        HAL_UART_Receive_IT(&huart2, Uart2_Receive_buf, sizeof(Uart2_Receive_buf));
    }
    else if (huart->Instance == USART3)
    {
        uint8_t temp_data = Uart3_Receive_buf[0];

        if (bt_rx_index < BT_RX_BUF_SIZE - 1)
        {
            bt_rx_buf[bt_rx_index++] = temp_data;
            if (temp_data == '\n' && bt_rx_index > 1 && bt_rx_buf[bt_rx_index - 2] == '\r')
            {
                bt_rx_buf[bt_rx_index - 2] = '\0';
                bt_rx_index--;
            }
        }
        else
        {
            bt_rx_index = 0;
            memset(bt_rx_buf, 0, BT_RX_BUF_SIZE);
        }

        HAL_UART_Receive_IT(&huart3, Uart3_Receive_buf, sizeof(Uart3_Receive_buf));
    }
    else if (huart->Instance == USART1)
    {
        HAL_UART_Receive_IT(&huart1, Usart1_Receive_buf, sizeof(Usart1_Receive_buf));
    }
}
uint8_t Hand(char *a)                   // 串口命令识别函数
{ 
    if(strstr(USART_RX_BUF,a)!=NULL)
	    return 1;
	else
		return 0;
}

void CLR_Buf(void)                           // 串口缓存清理
{
	memset(USART_RX_BUF, 0, USART_REC_LEN);      //清空
  point1 = 0;                    
}

void data_process(void)//数据处理函数，完成一帧之后可进行数据处理
{
	//计算距离 - 改为实时计算而不是每100帧
	static u8 cnt = 0;
	u8 i;
	static u16 count = 0;
	static u32 sum = 0;
	
	// 当前帧内计算有效点
	for(i=0;i<12;i++)//12个点取平均
	{
		if(Pack_Data.point[i].distance != 0)//去除0的点
		{
			count++;
			sum += Pack_Data.point[i].distance;
		}
	}
	
	// 改进：每3帧更新一次距离（更频繁），而不是等100帧
	if(++cnt >= 3)
	{
		if(count > 0)
		{
			distance = sum/count;
		}
		sum = 0;
		count = 0;
		cnt = 0;
	}
}

// 诊断函数：输出激光接收状态（不输出字节列举）
void Lidar_Debug_Info(void)
{
	static u32 last_frame_cnt = 0;
	static u32 last_byte_cnt = 0;
	extern uint32_t usart2_byte_count;
	extern uint32_t usart2_interrupt_count;
	
	uint32_t frame_increase = receive_cnt - last_frame_cnt;
	uint32_t byte_increase = usart2_byte_count - last_byte_cnt;
	
	printf("[LIDAR] Frames: %u (+%u), Distance: %dmm, Bytes: %u (+%u), Interrupts: %u\r\n", 
		   receive_cnt, frame_increase,
		   distance, 
		   usart2_byte_count, byte_increase,
		   usart2_interrupt_count);
	
	last_frame_cnt = receive_cnt;
	last_byte_cnt = usart2_byte_count;
}

/* USER CODE END 1 */
