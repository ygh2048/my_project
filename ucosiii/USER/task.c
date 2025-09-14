/*
 * @Author: ygh && “ygh2048009576@outlook.com”
 * @Date: 2025-05-03 16:24:36
 * @LastEditors: ygh && “ygh2048009576@outlook.com”
 * @LastEditTime: 2025-05-08 21:22:36
 * @FilePath: \experient4\USER\task.c
 * @Description: 实验四的任务具体实现，包括GUI和任务函数
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
#include "task.h"

#include <stdlib.h>  // 用于atoi函数
#include <string.h>  // 用于字符串处理

#include "mine.h"

//任务控制块
OS_TCB StartTaskTCB;
CPU_STK START_TASK_STK[START_STK_SIZE];

//任务控制块
OS_TCB TouchTaskTCB;
CPU_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];

//任务控制块
OS_TCB AdcTaskTCB;
CPU_STK ADC_TASK_STK[ADC_STK_SIZE];

//任务控制块
OS_TCB EmwindemoTaskTCB;
CPU_STK EMWINDEMO_TASK_STK[EMWINDEMO_STK_SIZE];

//任务控制块
OS_TCB UartTaskTCB;
CPU_STK UART_TASK_STK[UART_STK_SIZE];


OS_SEM sem_adc;
// OS_Q key_msg;
OS_Q adc_data;


//全局变量
static LIMIT_TEM mylimit;
volatile static int warning_state = 0;





/**
 * @description: 开始函数
 * @param {void} *p_arg
 * @return {*}
 */
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC,ENABLE);//开启CRC时钟
	WM_SetCreateFlags(WM_CF_MEMDEV);
	GUI_Init();  			//STemWin初始化
	
	OS_CRITICAL_ENTER();	//进入临界区

	OSSemCreate((OS_SEM* )&sem_adc,//创建信号量
				(CPU_CHAR*)"sem_adc",
				(OS_SEM_CTR)1,
				(OS_ERR*)&err);
	
	// OSQCreate((OS_Q* )&key_msg,//创建消息队列
	// 		(CPU_CHAR* )"key_msg",
	// 		(OS_MSG_QTY)KEY_MSG_Q_NUM,
	// 		(OS_ERR* )&err);
	OSQCreate((OS_Q* )&adc_data,//创建消息队列
			(CPU_CHAR* )"adc_data",
			(OS_MSG_QTY)ADC_DATA_Q_NUM,
			(OS_ERR* )&err);
	
	//STemWin Demo任务	
	OSTaskCreate((OS_TCB*     )&EmwindemoTaskTCB,		
				 (CPU_CHAR*   )"Emwindemo task", 		
                 (OS_TASK_PTR )emwindemo_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )EMWINDEMO_TASK_PRIO,     
                 (CPU_STK*    )&EMWINDEMO_TASK_STK[0],	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE/10,	
                 (CPU_STK_SIZE)EMWINDEMO_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);
	//按键任务
	OSTaskCreate((OS_TCB*     )&TouchTaskTCB,		
				 (CPU_CHAR*   )"Touch task", 		
                 (OS_TASK_PTR )touch_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )TOUCH_TASK_PRIO,     
                 (CPU_STK*    )&TOUCH_TASK_STK[0],	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE/10,	
                 (CPU_STK_SIZE)TOUCH_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			 
	//adc任务
	OSTaskCreate((OS_TCB*     )&AdcTaskTCB,		
				 (CPU_CHAR*   )"adc task", 		
                 (OS_TASK_PTR )adc_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )ADC_TASK_PRIO,     
                 (CPU_STK*    )&ADC_TASK_STK[0],	
                 (CPU_STK_SIZE)ADC_STK_SIZE/10,	
                 (CPU_STK_SIZE)ADC_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);			
				 
	//uart任务
	OSTaskCreate((OS_TCB*     )&UartTaskTCB,		
				 (CPU_CHAR*   )"uart task", 		
                 (OS_TASK_PTR )uart_task, 			
                 (void*       )0,					
                 (OS_PRIO	  )UART_TASK_PRIO,     
                 (CPU_STK*    )&UART_TASK_STK[0],	
                 (CPU_STK_SIZE)UART_STK_SIZE/10,	
                 (CPU_STK_SIZE)UART_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,  					
                 (void*       )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR*     )&err);		

	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//退出临界区
}


/**
 * @description: 显示任务，负责当前一些信息的显示，1：包括当前温度，警报温度，已记录的时间。2：显示温度时间曲线
 * @param {void} *p_arg
 * @return {*}
 */
void emwindemo_task(void *p_arg)
{
	OS_ERR err;
	display_init();
	while(1)
	{
		OSSemPend(&sem_adc,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
		//GUIDEMO_Main(); 
		display_temp_line(mylimit);
		display_information(mylimit,warning_state);
		GUI_Delay(100);
	}
}



/**
 * @description: 按键任务，负责设置报警温度，左右按键选择个位十位，上下按键改变大小，注意有大小的限位
 * @param {void} *p_arg
 * @return {*}
 */
void touch_task(void *p_arg)
{
	OS_ERR err;
	int key_msg = 0;
	mylimit.p_value = 'N';

	mylimit.value = 50;
	while(1)
	{
		mylimit.decade = mylimit.value/10;
		mylimit.theuint = mylimit.value%10;
		//GUI_TOUCH_Exec();	
		OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);//延时50ms
		key_msg = KEY_Scan(1);
		switch (key_msg)
		{
		case 0://没有任何按键按下
			break;
		case 1://KEY0
			mylimit.p_value = 'R';//修改十位
			break;
		case 2://KEY1
			if(mylimit.p_value == 'R')
			{
				mylimit.theuint--;
				if(mylimit.theuint == -1)
				{
					mylimit.decade--;
					mylimit.theuint = 9;
				}
				if(mylimit.decade < 0)
				{
					mylimit.decade = 0;
					mylimit.theuint = 0;
				}
			}
			else if(mylimit.p_value == 'L')
			{
				if(mylimit.decade > 0)mylimit.decade--;
				else mylimit.decade = 0;
			}
			mylimit.value = mylimit.decade*10 + mylimit.theuint;
			delay_ms(150);
			break;
		case 3://KEY2
			mylimit.p_value = 'L';//修改个位	
			break;
		case 4://WKUP
			if(mylimit.p_value == 'R')
			{
				mylimit.theuint++;
				if(mylimit.theuint == 10)
				{
					mylimit.decade++;
					mylimit.theuint = 0;
				}
				if(mylimit.decade > 9)
				{
					mylimit.decade = 9;
					mylimit.theuint = 9;
				}
			}
			else if(mylimit.p_value == 'L')
			{
				if(mylimit.decade < 9)mylimit.decade++;
				else mylimit.decade = 9;
			}
			mylimit.value = mylimit.decade*10 + mylimit.theuint;
			delay_ms(150);
			break;
		default:
			break;
		}
	}
}


/**
 * @description: adc任务，负责adc采集电压，并且使用消息队列发送输出，并且将已有采集到的数据存储到数组中,当当前温度大于警报温度的时候蜂鸣器响
 * @param {void} *p_arg
 * @return {*}
 */
void adc_task(void *p_arg)
{
	u16 adc_now = 0;
	OS_ERR err;
	static u8 q_data_input[2];
	u16 num = 0;
	u16 num_ms = 0;
	mylimit.value = 50;
	while(1)
	{
		LED0 = !LED0;
		adc_now = Get_Adc_Average(ADC_Channel_5,20);//获取通道5的转换值，20次取平均
		u16_to_u8(&adc_now,0,q_data_input);
		OSQPost((OS_Q*		)&adc_data,		
				(void*		)q_data_input,
				(OS_MSG_SIZE)2,
				(OS_OPT		)OS_OPT_POST_LIFO,
				(OS_ERR*	)&err);
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);

		num_ms++;
		num_ms %= 4; //每4次采集一次数据，避免频繁采集数据
		if(num_ms == 3)
		{
			//adc_record[num] = adc_now;
			adc_record_write(adc_now);
			num++;
			num%=512;
		}
		OSSemPost(&sem_adc,OS_OPT_POST_1,&err);//发送信号量
		if(warning_state)
		{
			BEEP = !BEEP;
			OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);
			BEEP = !BEEP;
		}
	}
}

/**
 * @description: 检测串口是否发送了指令，1、查看指令打印温度。2、设置指令设置报警温度。3、信息指令显示当前设置的报警温度以及实际温度。
 * @param {void} *p_arg
 * @return {*}
 */
void uart_task(void *p_arg)
{
	OS_ERR err;
	char command = 'E';
	char temp_com;
	float temp;
	u16 adcx;
	u16 adc_now_u = 0;
	u8 *p;
	OS_MSG_SIZE size;
	u8 len;
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_PERIODIC,&err);
		switch(command)
		{
		case 'A'://读取当前adc
			printf("A now_adc: %d.%dV\r\n",adcx,(u16)temp);	//输出ADC值
			printf("A now_adc_record: %d\r\n",adc_record_read(0));
			command = 'E';
			break;

			
		case 'B'://各种信息
			printf("B now_adc: %d.%dV\r\n",adcx,(u16)temp);	//输出ADC值
			printf("limit tem:%d\r\n",mylimit.value);
			printf("warning state : %d\r\n",warning_state);
			printf("mylimit.decade %d \r\n",mylimit.decade);
			printf("mylimit.theuint %d \r\n",mylimit.theuint);
			printf("mylimit.p_value %c \r\n",mylimit.p_value);
			command = 'E';
			break;


		case 'C'://修改限制温度
			printf("C input limit tem  ");
			USART_RX_STA = 0;
			delay_ms(100);
			while(!(USART_RX_STA&0x8000))//等待有串口通信
			{
				OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);
			}
			// 将接收缓冲区转换为数字
			len = USART_RX_STA & 0x3FFF;
			USART_RX_BUF[len] = '\0';  
			mylimit.value = atoi((char*)USART_RX_BUF);
			if(mylimit.value >= LIMIT_MAX && mylimit.value <= LIMIT_MIN)
			mylimit.value = 101;
			printf("set limit tem = %d\r\n",mylimit.value);
			USART_RX_STA=0;
			command = 'E';
			break;

			
		case 'D'://输出记录的所有adc值
			printf("this is all the record data:\r\n");
			printf("lenght :%d\r\n",adc_record_length());
			adc_record_print();
			printf("ending------");
			command = 'E';
			break;


		case 'E':
			//请求消息
			p=OSQPend((OS_Q*		)&adc_data,   
					(OS_TICK		)0,
					(OS_OPT		)OS_OPT_PEND_BLOCKING,
					(OS_MSG_SIZE*	)&size,	
					(CPU_TS*		)0,
					(OS_ERR*		)&err);
			u8_to_u16(p,0,&adc_now_u);
			temp=(float)adc_now_u*(100.0f/4095.0f);//获取计算后的带小数的实际电压值
			adcx=temp;
			temp-=adcx;
			temp*=1000;

			if(adcx >= mylimit.value){warning_state = 1;}
			else{warning_state = 0;}
	
			if(USART_RX_STA&0x8000)
			{
				temp_com = 'E';
				len = USART_RX_STA & 0x3FFF;
				USART_RX_BUF[len] = '\0';  
				temp_com = USART_RX_BUF[0];
				if(temp_com == 'A')command = 'A';
				else if (temp_com == 'B')command = 'B';
				else if (temp_com == 'C')command = 'C';
				else if (temp_com == 'D')command = 'D';
				else if (temp_com == 'F')command = 'F';
				else{
					printf("imput error ,please input A B C D F");
					temp_com = 'E';
					command = 'E';
				}
				delay_ms(10);
				memset(USART_RX_BUF,0,USART_REC_LEN);
				USART_RX_STA=0;
			}
			break;
			
		case 'F':  //调试使用
			printf("F input debug  information \r\n");
			printf("first: mylimit.p_value");
			USART_RX_STA = 0;
			delay_ms(100);
			while(!(USART_RX_STA&0x8000))//等待有串口通信
			{
				OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_PERIODIC,&err);
			} 
			mylimit.p_value = USART_RX_BUF[0];
			USART_RX_STA=0;
			command = 'E';
			break;
		default:                
			break;
		}

	}
}

