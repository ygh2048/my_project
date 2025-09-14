/*
 * bsp_tjc.c
 *
 *  Created on: Jul 23, 2024
 *      Author: ygh20
 */

#include "bsp_tjc.h"

void copy(unsigned char *buf,uint8_t *data,int len)
{
	for(int i=0;i<len;i++)
	{
		buf[i]=data[i];
	}
}
uint8_t end[3]={0xff,0xff,0xff};
void refresh(float x,float y,float z)
{
	char frame_info[3][30]; //定义的文本长度
	char str[3][15]; //定义的数字长度
	int len[3][2];
	int16_t px,py,pz;
	px=(int16_t)x;
	py=(int16_t)y;
	pz=(int16_t)z;
	len[0][0]=strlen((const char *)"x.val=");
	itoa(px,str[0],10);
	len[0][1]=strlen((const char *)str[0]);
	memcpy(frame_info[0],"x.val=",len[0][0]);
//	copy(frame_info[0],"x.val=",len[0][0]);
	len[1][0]=strlen((const char *)"y.val=");
	itoa(py,str[1],10);
	len[1][1]=strlen((const char *)str[1]);
	memcpy(frame_info[1],"y.val=",len[1][0]);
//	copy(frame_info[1],"y.val=",len[1][0]);
	len[2][0]=strlen((const char *)"z.val=");
	itoa(pz,str[2],10);
	len[2][1]=strlen((const char *)str[2]);
	memcpy(frame_info[2],"z.val=",len[2][0]);
//	copy(frame_info[0],"z.val=",len[2][0]);
	for(int i=0;i<3;i++)
	{
		memcpy(&frame_info[i][len[i][0]],str[i],len[i][1]);
		frame_info[i][len[i][1]+len[i][0]]=end[i];
		frame_info[i][1+len[i][1]+len[i][0]]=end[i];
		frame_info[i][2+len[i][1]+len[i][0]]=end[i];
		USART1_Send_ArrayU8((unsigned  char *)frame_info[i], 3+len[i][0]+len[i][1]);
		HAL_Delay(10);
	}
}

void refresh_new(float x,float y,float z)//x.val<320,y.val<240
{
	TJCPrintf("x.val=%d",(int)x);
	TJCPrintf("y.val=%d",(int)y);
	TJCPrintf("z.val=%d",(int)z);
}

void TJCPrintf(const char *str, ...)
{
	uint8_t end = 0xff;
	char buffer[STR_LENGTH+1];  // 数据长度
	uint8_t i = 0;
	va_list arg_ptr;
	va_start(arg_ptr, str);
	vsnprintf(buffer, STR_LENGTH+1, str, arg_ptr);
	va_end(arg_ptr);
	while ((i < STR_LENGTH) && (i < strlen(buffer)))
	{
		HAL_UART_Transmit_IT(&huart1, (uint8_t *)(buffer) + i++, 1);
		while(huart1.gState != HAL_UART_STATE_READY);	//等待发送完毕
	}
	HAL_UART_Transmit_IT(&huart1, &end, 1);			//这个函数改为你的单片机的串口发送单字节函数
	while(huart1.gState != HAL_UART_STATE_READY);	//等待发送完毕
	HAL_UART_Transmit_IT(&huart1, &end, 1);			//这个函数改为你的单片机的串口发送单字节函数
	while(huart1.gState != HAL_UART_STATE_READY);	//等待发送完毕
	HAL_UART_Transmit_IT(&huart1, &end, 1);			//这个函数改为你的单片机的串口发送单字节函数
	while(huart1.gState != HAL_UART_STATE_READY);	//等待发送完毕

}

