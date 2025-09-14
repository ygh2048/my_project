/*
 * @Author: ygh && ??ygh2048009576@outlook.com??
 * @Date: 2025-03-26 16:46:41
 * @LastEditors: ygh && ??ygh2048009576@outlook.com??
 * @LastEditTime: 2025-03-29 20:08:35
 * @FilePath: \project\FreeRTOS\BSP\bsp_task.c
 * @Description: 
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
/*
 * bsp_task.c
 *
 *  Created on: Mar 5, 2022
 *      Author: Administrator
 */
#include "bsp.h"

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern osMessageQId uart_queueHandle;
int enable_beep = 0;
int enable_led=0;
extern uint8_t keynum;
extern uint8_t uart_buff_ctrl;
extern uint8_t uart_buff_ctrl_last;
// LED light task entity function  LED
void defaulttask_uart(void)
{
    UART_RX_TypeDef *pRecvUartData; 
    printf("Uart Thread init. \r\n");
    while (1)
    {
        if (xQueueReceive(uart_queueHandle, &pRecvUartData, portMAX_DELAY) == pdTRUE)
        {
        	unpack_process_gh(pRecvUartData);
        	osDelay(10);
//        	printf("ok\r\n");
        }
    }
}

// Buzzer task entity function  
void Task_Entity_Beep(void)
{
    while (1)
    {
        if (enable_beep)
        {
            // The buzzer goes off every 200 milliseconds  
            BEEP_ON();
            osDelay(300);
            BEEP_OFF();
            osDelay(100);
            enable_beep=0;
        }
        else
        {
            BEEP_OFF();
            osDelay(100);
        }
        if (enable_led)
        {
            // The buzzer goes off every 200 milliseconds  
        	LED1_OFF();
        	osDelay(1000);
            
        	enable_led=0;
        	enable_beep=1;
        }
        else
        {
        	LED1_ON();
        	osDelay(100);
        }

    }
}

// Key task entity function  串口测试
void Task_Entity_Key(void)
{
    
	int i=0;
	int keyangle=0;
    while (1)
    {
        if (Key1_State(1) == KEY_PRESS)
        {
            // Button controls the buzzer switch  
            enable_beep = !enable_beep;
//        	refresh(1, 2, 3);
            i++;
          //speaker_gh(0, 0, 0, 1, 1, 0, 0);//FF FC 00 00 40 41 CD CC 8C 3F 9A 99 E9 40 00 01 00 18 00 15 00 15 A1 A2
            
        }
        if (Key2_State(1) == KEY_PRESS)
        {
            // Button controls the buzzer switch           //enable_beep = !enable_beep;
            keynum++;
        	keyangle+=45;
        	keyangle%=180;
//        	print3dpos();
        	refresh_new(3,2,1);
//        	drawpoints((float)keynum, (float)keynum);
        }
//        dj_set(dj_1, keyangle, bdj180);
        osDelay(10);
    }
}
