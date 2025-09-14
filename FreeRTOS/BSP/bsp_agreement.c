/*
 * bsp_agreement.c
 *
 *  Created on: Jul 21, 2024
 *      Author: ygh20
 */
#include "bsp_agreement.h"
#include "CRC_communication.h"
extern uint8_t uart_buff_ctrl;
extern uint8_t uart_buff_ctrl_last;
extern osMessageQId uart_queueHandle;
extern Usedata mydata;


int unpack_overflag=0;

extern void useget(void);

void unpack_process(UART_RX_TypeDef *pRecvUartData){
	unsigned char * Rx_Buf;
    if (xQueueReceive(uart_queueHandle, &pRecvUartData, portMAX_DELAY) == pdTRUE)
    {
//	NCLink_Data_Prase_Process(pRecvUartData->buffer,pRecvUartData->size);
    	Rx_Buf=pRecvUartData->buffer;
    	for(uint16_t i=0;i<pRecvUartData->size;i++)
    	{
    		//NCLink_Data_Prase_Prepare(Rx_Buf[i]);
			gh
    	}
    	if(unpack_overflag==1)
    	{
    	unpack_overflag=0;
    	print3dpos();
    	}
    }
}

void unpack_process_gh(UART_RX_TypeDef *pRecvUartData)
{
	unsigned char * Rx_Buf;
    	Rx_Buf=pRecvUartData->buffer;
    	for(uint16_t i=0;i<pRecvUartData->size;i++)
    	{
    		NCLink_Data_Prase_Prepare_gh(Rx_Buf[i]);
    	}
    	if(unpack_overflag==1)
    	{
    	unpack_overflag=0;
//    	useget();
//    	print3dpos();
//    	refresh_new(mydata.q0, mydata.q1, mydata.z);
//    	choose_pos();
    	data_to_array();
    	show();
    	}
}

void print3dpos(void)
{
	printf("x.val=%f\r\n",mydata.x);
	printf("y.val=%f\r\n",mydata.y);
	printf("z.val=%f\r\n",mydata.z);
	printf("q0.val=%d\r\n",mydata.q0);
	printf("q1.val=%d\r\n",mydata.q1);
	printf("q2.val=%d\r\n",mydata.q2);
	printf("q3.val=%d\r\n",mydata.q3);
}


void speaker_gh(float x,float y,float z,int16_t q0,int16_t q1,int16_t q2,int16_t q3)
{
		unsigned char sendbuf[24];
		unsigned int len;
		int cnt=14;
		len = sizeof(sendbuf);
		sendbuf[0]=NCLink_Head[0];
		sendbuf[1]=NCLink_Head[1];
		sendbuf[22]=NCLink_End[0];
		sendbuf[23]=NCLink_End[1];
		Float2Byte(&x,sendbuf,2);
		Float2Byte(&y,sendbuf,6);
		Float2Byte(&z,sendbuf,10);
		sendbuf[cnt++]=BYTE1(q0);
		sendbuf[cnt++]=BYTE0(q0);
		sendbuf[cnt++]=BYTE1(q1);
		sendbuf[cnt++]=BYTE0(q1);
		sendbuf[cnt++]=BYTE1(q2);
		sendbuf[cnt++]=BYTE0(q2);
		sendbuf[cnt++]=BYTE1(q3);
		sendbuf[cnt++]=BYTE0(q3);
		USART1_Send_ArrayU8(sendbuf,len);
}

