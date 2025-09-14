#ifndef __TASK_H
#define __TASK_H

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "sram.h"
#include "malloc.h"
#include "ILI93xx.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "adc.h"
#include "timer.h"
#include "touch.h"
#include "GUI.h"
#include "GUIDemo.h"
#include "includes.h"




//任务优先级
#define START_TASK_PRIO				3
//任务堆栈大小	
#define START_STK_SIZE 				1024
//任务控制块
extern OS_TCB StartTaskTCB;
extern CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//TOUCH任务
//设置任务优先级
#define TOUCH_TASK_PRIO				4
//任务堆栈大小
#define TOUCH_STK_SIZE				128
//任务控制块
extern OS_TCB TouchTaskTCB;
extern CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//touch任务
void touch_task(void *p_arg);

//adc任务
//设置任务优先级
#define ADC_TASK_PRIO 				5
//任务堆栈大小
#define ADC_STK_SIZE				128
//任务控制块
extern OS_TCB AdcTaskTCB;
extern CPU_STK ADC_TASK_STK[ADC_STK_SIZE];
//ADC任务
void adc_task(void *p_arg);

//EMWINDEMO任务
//设置任务优先级
#define EMWINDEMO_TASK_PRIO			6
//任务堆栈大小
#define EMWINDEMO_STK_SIZE			2048
//任务控制块
extern OS_TCB EmwindemoTaskTCB;
extern CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];
//emwindemo_task任务
void emwindemo_task(void *p_arg);



//串口任务
//设置任务优先级
#define UART_TASK_PRIO 				7
//任务堆栈大小
#define UART_STK_SIZE				256
//任务控制块
extern OS_TCB UartTaskTCB;
extern CPU_STK UART_TASK_STK[UART_STK_SIZE];
//ADC任务
void uart_task(void *p_arg);

//消息队列
#define KEY_MSG_Q_NUM   1
#define ADC_DATA_Q_NUM  10

#define LIMIT_MIN 0
#define LIMIT_MAX 100


#endif
