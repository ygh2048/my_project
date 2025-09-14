/*
 * @Author: ygh && “ygh2048009576@outlook.com”
 * @Date: 2025-05-11 15:15:46
 * @LastEditors: ygh && “ygh2048009576@outlook.com”
 * @LastEditTime: 2025-05-11 21:21:00
 * @FilePath: \task_E\project\test_ai\MDK-ARM\communication.c
 * @Description: 
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
 
#include "communication.h"
#include "bsp_uart.h"
#include "stdio.h"
#include "string.h"


union
{
    unsigned char u8data[2];
    short u16data;
}mytransform_u16;//负责类型转换   u16< -->u8



/**
 * @description: u8数据转u16数据
 * @param {unsigned char} *Byte //输入数组
 * @param {unsigned char} Subscript //从数组的第几个字节开始转换
 * @param {unsigned short} *u16Value   //输出值
 * @return {*}
 */
void u8_to_u16(unsigned char *Byte,unsigned char Subscript,unsigned short *u16Value)
{
	mytransform_u16.u8data[0] = Byte[Subscript];
	mytransform_u16.u8data[1] = Byte[Subscript + 1];
    *u16Value = mytransform_u16.u16data;
}


/**
 * @description: u16数据转u8数据
 * @param {unsigned short} *u16Value    //输入值
 * @param {unsigned char} Subscript  //从数组的第几个字节开始转换
 * @param {unsigned char} *Byte   //输出数组
 * @return {*}
 */
void u16_to_u8(unsigned short *u16Value,unsigned char Subscript,unsigned char *Byte)
{
    mytransform_u16.u16data = (unsigned short)2;
	if(mytransform_u16.u8data[1] == 0)//小端模式
	{
		mytransform_u16.u16data = *u16Value;
		Byte[Subscript]     = mytransform_u16.u8data[0];
		Byte[Subscript + 1] = mytransform_u16.u8data[1];
	}
	else//大端模式
	{
		mytransform_u16.u16data = *u16Value;
		Byte[Subscript]     = mytransform_u16.u8data[1];
		Byte[Subscript + 1] = mytransform_u16.u8data[0];
	}
}


union
{
    unsigned char u8data[4];
    int intdata;
}mytransform_int;//负责类型转换   int< -->u8

/**
 * @description: u8数据转int数据
 * @param {unsigned char} *Byte //输入数组
 * @param {unsigned char} Subscript //从数组的第几个字节开始转换
 * @param {int} *intValue   //输出值
 * @return {*}
 */
void u8_to_int(unsigned char *Byte,unsigned char Subscript,int *intValue)
{
	mytransform_int.u8data[0] = Byte[Subscript];
	mytransform_int.u8data[1] = Byte[Subscript + 1];
	mytransform_int.u8data[2] = Byte[Subscript + 2];
	mytransform_int.u8data[3] = Byte[Subscript + 3];
    *intValue = mytransform_int.intdata;
}


/**
 * @description: int数据转u8数据
 * @param {int} *intValue    //输入值
 * @param {unsigned char} Subscript  //从数组的第几个字节开始转换
 * @param {unsigned char} *Byte   //输出数组
 * @return {*}
 */
void int_to_u8(int *intValue,unsigned char Subscript,unsigned char *Byte)
{
    mytransform_int.intdata = (unsigned short)2;
	if(mytransform_int.u8data[1] == 0)//小端模式
	{
		mytransform_int.intdata = *intValue;
		Byte[Subscript]     = mytransform_int.u8data[0];
		Byte[Subscript + 1] = mytransform_int.u8data[1];
		Byte[Subscript + 2] = mytransform_int.u8data[2];
		Byte[Subscript + 3] = mytransform_int.u8data[3];
	}
	else//大端模式
	{
		mytransform_int.intdata = *intValue;
		Byte[Subscript]     = mytransform_int.u8data[3];
		Byte[Subscript + 1] = mytransform_int.u8data[2];
		Byte[Subscript + 2] = mytransform_int.u8data[1];
		Byte[Subscript + 3] = mytransform_int.u8data[0];
	}
}

union
{
unsigned char floatByte[4];
float floatValue;
}FloatUnion;



/**
 * @description: 浮点数转u8数据
 * @param {float} *FloatValue
 * @param {unsigned char} *Byte
 * @param {unsigned char} Subscript
 * @return {*}
 */
void Float2Byte(float *FloatValue, unsigned char *Byte, unsigned char Subscript)
{
	FloatUnion.floatValue = (float)2;
	if(FloatUnion.floatByte[0] == 0)//小端模式
	{
		FloatUnion.floatValue = *FloatValue;
		Byte[Subscript]     = FloatUnion.floatByte[0];
		Byte[Subscript + 1] = FloatUnion.floatByte[1];
		Byte[Subscript + 2] = FloatUnion.floatByte[2];
		Byte[Subscript + 3] = FloatUnion.floatByte[3];
	}
	else//大端模式
	{
		FloatUnion.floatValue = *FloatValue;
		Byte[Subscript]     = FloatUnion.floatByte[3];
		Byte[Subscript + 1] = FloatUnion.floatByte[2];
		Byte[Subscript + 2] = FloatUnion.floatByte[1];
		Byte[Subscript + 3] = FloatUnion.floatByte[0];
	}
}


/**
 * @description: u8数据转浮点数
 * @param {unsigned char} *Byte
 * @param {unsigned char} Subscript
 * @param {float} *FloatValue
 * @return {*}
 */
void Byte2Float(unsigned char *Byte, unsigned char Subscript, float *FloatValue)
{
	FloatUnion.floatByte[0]=Byte[Subscript];
	FloatUnion.floatByte[1]=Byte[Subscript + 1];
	FloatUnion.floatByte[2]=Byte[Subscript + 2];
	FloatUnion.floatByte[3]=Byte[Subscript + 3];
	*FloatValue=FloatUnion.floatValue;
}


uint8_t GHLink_Head[2]={0xFF,0xFC};//数据帧头
uint8_t GHLink_End[2] ={0xA1,0xA2};//数据帧尾




#if MODE_COM == 1

//数据结构定义
float array_data[FLOAT_LENGTH] = {0.0};
uint8_t if_float_data = 0;

//函数定义



/**
 * @description: 单片机发送数据打包函数
 * @param {float} *data
 * @return {*}
 */
void gh_link_pack(void)
{
	uint8_t data_to_send[FLOAT_LENGTH*4+4];
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;
	sum = FLOAT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
		Float2Byte(&array_data[i],data_to_send,cnt);
		cnt+=4;
	}
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}


/**
 * @description: 串口数据解析函数，解析为数组方便处理
 * @param {uint8_t} data
 * @return {*}
 */
void gh_link_unpack_process(uint8_t data)
{
    static uint8_t buf[100];
	static uint8_t data_cnt = 0;
	static uint8_t state = 0;
	  if(state==0&&data==GHLink_Head[0])//判断帧头1
	  {
	    state=1;
	    buf[0]=data;
	  }
	  else if(state==1&&data==GHLink_Head[1])//判断帧头2
	  {
	    state=2;
	    buf[1]=data;
	    data_cnt=0;
	  }
	  else if(state==2)//数据接收
	  {
	    buf[2+data_cnt++]=data;
	    if(data_cnt== FLOAT_LENGTH*4)
	    {
	    	state=3;
	    }
	  }
	  else if(state==3&&data==GHLink_End[0])//帧尾0
		{
				state = 4;
				buf[2+data_cnt++]=data;
		}
	   else if(state==4&&data==GHLink_End[1])//帧尾1
		{
				state = 0;
				buf[2+data_cnt]=data;
				gh_link_data(buf,data_cnt+3);//数据解析
		}
	  else state = 0;
}

/**
 * @description: 串口整体解析函数
 * @param 
 * @return {*}
 */
void gh_link_unpack(void)
{
    	for(uint16_t i=0;i<uart_rx_data_t[uart_buff_ctrl_last].size;i++)
    	{
    		gh_link_unpack_process(uart_rx_data_t[uart_buff_ctrl_last].buffer[i]);
    	}
		
}

/**
 * @description: 根据提取出的数据帧，解析每一帧数据数据    float 版本
 * @param {uint8_t} *data_buf 待解析数据帧
 * @param {uint8_t} num 数据帧长
 * @return {*}
 */
void gh_link_data(uint8_t *data_buf,uint8_t num)
{
    if(!(*(data_buf)==GHLink_Head[0]&&*(data_buf+1)==GHLink_Head[1]))         return;//判断帧头
	if(!(*(data_buf+num-2)==GHLink_End[0]&&*(data_buf+num-1)==GHLink_End[1])) return;//帧尾校验


	if(num == FLOAT_LENGTH * 4 + 4)
	{
		if_float_data = 1;
	}
	
	if(if_float_data)
	{
		for (size_t i = 0; i < FLOAT_LENGTH; i++)
		{
			Byte2Float(data_buf,2+i*4,&array_data[i]);
		}
	}
}
//正常
//temp[0]
//FF FC FE A2 FE 44 2E E6 C3 40 AE F2 44 3E E0 01 26 42 C0 A1 77 44 6A 15 28 40 53 5B 06 42 15 A1 B8 44 A0 96 97 3F BE 65 EB 40 00 00 48 43 6E 79 AE 3F 80 32 51 42 F8 C7 4C 42 A1 A2 
//temp[1]
//FF FC B3 F6 00 45 BB 53 CA 40 B0 D0 48 3E 2B 5A 24 42 40 F7 0B 45 6A 4A 91 40 89 9E 08 42 DB 57 B9 44 4F F3 9A 3F 21 8F E7 40 00 00 A3 43 21 03 B3 3F 1A 40 54 42 0C 9B 50 42 A1 A2
//temp[2]
//FF FC B8 54 FE 44 FA 18 D6 40 AA 80 57 3E 41 63 28 42 A0 1D 1C 45 37 3B A3 40 4E 78 06 42 C7 01 B9 44 FE C9 98 3F 93 EA EE 40 00 00 A1 43 60 C8 B7 3F 72 12 54 42 5A 6D 51 42 A1 A2

//异常
//temp[88]
//FF FC 02 25 08 45 DF D7 C5 40 F8 01 3A 3E 07 40 23 42 00 33 81 44 8D 1D 51 40 BA 3D 0A 42 CA A6 B1 44 48 F3 82 3F BE DB DF 40 00 00 C5 43 8D C5 95 3F 8B 66 E9 41 C0 39 33 42 A1 A2
//temp[89]
//FF FC 84 D5 07 45 DF 5D BF 40 72 54 34 3E 4A 88 24 42 00 33 81 44 51 07 54 40 AD FF 0A 42 91 08 B3 44 F0 CA 85 3F D9 DF E1 40 00 00 1A 43 B6 78 97 3F 59 A4 E7 41 58 C5 2B 42 A1 A2

void test_data_send(float array_data[])
{
	uint8_t data_to_send[FLOAT_LENGTH*4+4];
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;
	sum = FLOAT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
		Float2Byte(&array_data[i],data_to_send,cnt);
		cnt+=4;
	}
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}


void clear_array_data(void)
{
	memset(array_data, 0, FLOAT_LENGTH * sizeof(int));
}




#elif MODE_COM == 2 //自写自用数据协议  CRC
//CRC-8: x^8 + x^2 + x^1 + 1 (0x07)
// CRC8校验表（多项式0x07）
const uint8_t crc8_table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

/**
 * @description: CRC8校验计算函数
 * @param {uint8_t} *data 数据指针
 * @param {uint16_t} len 数据长度
 * @return {uint8_t} CRC校验值
 */
uint8_t calculate_crc8(uint8_t *data, uint16_t len)
{
    uint8_t crc = 0;
    for(uint16_t i = 0; i < len; i++)
    {
        crc = crc8_table[crc ^ data[i]];
    }
    return crc;
}
//数据结构定义
float array_data[FLOAT_LENGTH] = {0.0};
uint8_t if_float_data = 0;

//函数定义

/**
 * @description: 单片机发送数据打包函数（添加CRC校验）
 * @param {*} 
 * @return {*}
 */
void gh_link_pack(void)
{
	uint8_t data_to_send[FLOAT_LENGTH*4+5]; // 增加1字节CRC空间
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;
	sum = FLOAT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
		Float2Byte(&array_data[i],data_to_send,cnt);
		cnt+=4;
	}
	
	// 计算CRC（从帧头后开始到数据结束）
	uint8_t crc = calculate_crc8(&data_to_send[2], cnt-2);
	data_to_send[cnt++] = crc;
	
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}

/**
 * @description: 串口数据解析函数，解析为数组方便处理
 * @param {uint8_t} data
 * @return {*}
 */
void gh_link_unpack_process(uint8_t data)
{
    static uint8_t buf[100];
	static uint8_t data_cnt = 0;
	static uint8_t state = 0;
	
	if(state==0&&data==GHLink_Head[0])//判断帧头1
	{
		state=1;
		buf[0]=data;
	}
	else if(state==1&&data==GHLink_Head[1])//判断帧头2
	{
		state=2;
		buf[1]=data;
		data_cnt=0;
	}
	else if(state==2)//数据接收
	{
		buf[2+data_cnt++]=data;
		// 接收数据 + CRC（1字节）
		if(data_cnt== FLOAT_LENGTH*4 + 1)
		{
			state=3;
		}
	}
	else if(state==3&&data==GHLink_End[0])//帧尾0
	{
		state = 4;
		buf[2+data_cnt++]=data;
	}
	else if(state==4&&data==GHLink_End[1])//帧尾1
	{
		state = 0;
		buf[2+data_cnt]=data;
		gh_link_data(buf,data_cnt+3);//数据解析
	}
	else state = 0;
}

/**
 * @description: 串口整体解析函数
 * @param 
 * @return {*}
 */
void gh_link_unpack(void)
{
    for(uint16_t i=0;i<uart_rx_data_t[uart_buff_ctrl_last].size;i++)
    {
    	gh_link_unpack_process(uart_rx_data_t[uart_buff_ctrl_last].buffer[i]);
    }
}

/**
 * @description: 根据提取出的数据帧，解析每一帧数据数据    float 版本（添加CRC校验）
 * @param {uint8_t} *data_buf 待解析数据帧
 * @param {uint8_t} num 数据帧长
 * @return {*}
 */
void gh_link_data(uint8_t *data_buf,uint8_t num)
{
    if(!(data_buf[0]==GHLink_Head[0]&&data_buf[1]==GHLink_Head[1]))         return;//判断帧头
	if(!(data_buf[num-2]==GHLink_End[0]&&data_buf[num-1]==GHLink_End[1])) return;//帧尾校验

	// 检查数据长度是否正确（帧头2 + 数据FLOAT_LENGTH*4 + CRC1 + 帧尾2）
	if(num != FLOAT_LENGTH*4 + 5)
	{
		return;//长度错误
	}
	
	// 验证CRC（从帧头后开始到CRC前）
	uint8_t received_crc = data_buf[2 + FLOAT_LENGTH*4]; // CRC字节位置
	uint8_t calculated_crc = calculate_crc8(&data_buf[2], FLOAT_LENGTH*4);
	
	if(received_crc != calculated_crc)
	{
		return;//CRC校验失败
	}
	
	// CRC校验通过，解析数据
	if_float_data = 1;
	
	if(if_float_data)
	{
		for (size_t i = 0; i < FLOAT_LENGTH; i++)
		{
			Byte2Float(data_buf, 2+i*4, &array_data[i]);
		}
	}
}

void test_data_send(float array_data[])
{
	uint8_t data_to_send[FLOAT_LENGTH*4+5]; // 增加1字节CRC空间
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;
	sum = FLOAT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
		Float2Byte(&array_data[i],data_to_send,cnt);
		cnt+=4;
	}
	
	// 计算CRC（从帧头后开始到数据结束）
	uint8_t crc = calculate_crc8(&data_to_send[2], cnt-2);
	data_to_send[cnt++] = crc;
	
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}

void clear_array_data(void)
{
	memset(array_data, 0, FLOAT_LENGTH * sizeof(int));
}



#elif MODE_COM == 3 // int 版本


/**
 * @description: 单片机发送数据打包函数
 * @param 
 * @return {*}
 */
void gh_link_pack(void)
{
	uint8_t data_to_send[INT_LENGTH*4+4];
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;

	sum = INT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
		INT2Byte(&array_data[i],data_to_send,cnt);
		cnt+=4;
	}
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}

/**
 * @description: 串口数据解析函数，解析为数组方便处理
 * @param {uint8_t} data
 * @return {*}
 */
void gh_link_unpack_process(uint8_t data)
{
    static uint8_t buf[100];
	static uint8_t data_cnt = 0;
	static uint8_t state = 0;
	  if(state==0&&data==GHLink_Head[0])//判断帧头1
	  {
	    state=1;
	    buf[0]=data;
	  }
	  else if(state==1&&data==GHLink_Head[1])//判断帧头2
	  {
	    state=2;
	    buf[1]=data;
	    data_cnt=0;
	  }
	  else if(state==2)//数据接收
	  {
	    buf[2+data_cnt++]=data;
	    if(data_cnt== INT_LENGTH*4)
	    {
	    	state=3;
	    }
	  }
	  else if(state==3&&data==GHLink_End[0])//帧尾0
		{
				state = 4;
				buf[2+data_cnt++]=data;
		}
	   else if(state==4&&data==GHLink_End[1])//帧尾1
		{
				state = 0;
				buf[2+data_cnt]=data;
				gh_link_data(buf,data_cnt+3);//数据解析
		}
	  else state = 0;
}

/**
 * @description: 串口整体解析函数
 * @param 
 * @return {*}
 */
void gh_link_unpack(void)
{
    	for(uint16_t i=0;i<uart_rx_data_t[uart_buff_ctrl_last].size;i++)
    	{
    		gh_link_unpack_process(uart_rx_data_t[uart_buff_ctrl_last].buffer[i]);
    	}
		
}

/**
 * @description: 根据提取出的数据帧，解析每一帧数据数据    float 版本
 * @param {uint8_t} *data_buf 待解析数据帧
 * @param {uint8_t} num 数据帧长
 * @return {*}
 */
void gh_link_data(uint8_t *data_buf,uint8_t num)
{
    if(!(*(data_buf)==GHLink_Head[0]&&*(data_buf+1)==GHLink_Head[1]))         return;//判断帧头
	if(!(*(data_buf+num-2)==GHLink_End[0]&&*(data_buf+num-1)==GHLink_End[1])) return;//帧尾校验

	if(num == INT_LENGTH * 4 + 4)
	{
		for (size_t i = 0; i < INT_LENGTH; i++)
		{
			Byte2INT(data_buf,2+i*4,&array_data[i]);
		}
	}
}

void test_data_send(float array_data[])
{
	uint8_t data_to_send[INT_LENGTH*4+4];
	uint8_t i=0,cnt=0;
	uint8_t sum = 0;
	sum = INT_LENGTH;
	data_to_send[cnt++]=GHLink_Head[0];
  	data_to_send[cnt++]=GHLink_Head[1];

	for(i=0;i<sum;i++)
	{
		INT2Byte(&array_data[i],data_to_send,cnt);
		cnt+=4;
	}
	data_to_send[cnt++] = GHLink_End[0];
	data_to_send[cnt++] = GHLink_End[1];

	USART1_Send_ArrayU8(data_to_send,cnt);
}

void clear_array_data(void)
{
	memset(array_data, 0, INT_LENGTH * sizeof(int));
}



#endif
