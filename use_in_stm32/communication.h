/*
 * @Author: ygh && “ygh2048009576@outlook.com”
 * @Date: 2025-05-11 15:15:58
 * @LastEditors: ygh && “ygh2048009576@outlook.com”
 * @LastEditTime: 2025-05-11 19:48:08
 * @FilePath: \task_E\project\test_ai\MDK-ARM\communication.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H



#include "stdint.h"

//访问固定字节内容
#define BYTE0(dwTemp)  (*((unsigned char *)(&dwTemp)))
#define BYTE1(dwTemp)  (*((unsigned char *)(&dwTemp)+1))
#define BYTE2(dwTemp)  (*((unsigned char *)(&dwTemp)+2))
#define BYTE3(dwTemp)  (*((unsigned char *)(&dwTemp)+3))




//u8 <-> u16
void u8_to_u16(unsigned char *Byte,unsigned char Subscript,unsigned short *u16Value);
void u16_to_u8(unsigned short *u16Value,unsigned char Subscript,unsigned char *Byte);
//u8 <-> int
void u8_to_int(unsigned char *Byte,unsigned char Subscript,int *intValue);
void int_to_u8(int *intValue,unsigned char Subscript,unsigned char *Byte);

extern uint8_t GHLink_Head[2];
extern uint8_t GHLink_End[2];

#define MODE_COM 2 
/*
#define MODE_COM 1   float 数组
#define MODE_COM 2   自用组合数据结构    CRC
#define MODE_COM 3   int 数组
*/

#define FLOAT_LENGTH 14


#ifdef MODE_COM == 1
#define INT_LENGTH 12

extern int array_data[INT_LENGTH];


//数据打包函数
void gh_link_pack(void);

//测试函数
void test_data_send(float array_data[]);

//数据解包函数
void gh_link_unpack(void);

//数组清零函数
void clear_array_data(void);

#elif MODE_COM == 2 //自写自用数据协议 CRC版本

extern float array_data[FLOAT_LENGTH];
extern uint8_t if_float_data;



//数据打包函数
void gh_link_pack(void);

//测试函数
void test_data_send(float array_data[]);

//数据解包函数
void gh_link_unpack(void);

//数组清零函数
void clear_array_data(void);



#endif


#elif MUDE_COM == 3 //全uint_8版本数据协议


extern float array_data[FLOAT_LENGTH];
extern uint8_t if_float_data;



//数据打包函数
void gh_link_pack(void);

//测试函数
void test_data_send(float array_data[]);

//数据解包函数
void gh_link_unpack(void);

//数组清零函数
void clear_array_data(void);


#endif

