/*
 * @Author: ygh && “ygh2048009576@outlook.com”
 * @Date: 2025-05-05 20:37:03
 * @LastEditors: ygh && “ygh2048009576@outlook.com”
 * @LastEditTime: 2025-05-12 08:11:36
 * @FilePath: \project\experient4\USER\mine.c
 * @Description: 自写函数，包括存储区函数，类型转换函数，GUI显示函数
 * 
 * Copyright (c) 2025 by ygh, All Rights Reserved. 
 */
#include "mine.h"
#include "stdio.h"
#include "GUI.h"

union
{
    unsigned char u8data[2];
    short u16data;
}mytransform;//负责类型转换

/**
 * @description: u8数据转u16数据
 * @param {unsigned char} *Byte
 * @param {unsigned char} Subscript
 * @param {unsigned short} *u16Value
 * @return {*}
 */
void u8_to_u16(unsigned char *Byte,unsigned char Subscript,unsigned short *u16Value)
{
	mytransform.u8data[0] = Byte[Subscript];
	mytransform.u8data[1] = Byte[Subscript + 1];
    *u16Value = mytransform.u16data;
}


/**
 * @description: u16数据转u8数据
 * @param {unsigned short} *u16Value
 * @param {unsigned char} Subscript
 * @param {unsigned char} *Byte
 * @return {*}
 */
void u16_to_u8(unsigned short *u16Value,unsigned char Subscript,unsigned char *Byte)
{
    mytransform.u16data = (unsigned short)2;
	if(mytransform.u8data[1] == 0)//小端模式
	{
		mytransform.u16data = *u16Value;
		Byte[Subscript]     = mytransform.u8data[0];
		Byte[Subscript + 1] = mytransform.u8data[1];
	}
	else//大端模式
	{
		mytransform.u16data = *u16Value;
		Byte[Subscript]     = mytransform.u8data[1];
		Byte[Subscript + 1] = mytransform.u8data[0];
	}
}



volatile u16 data_count = 0;//统计写入数据长度
volatile u16 write_index = 0; // 当前写入位置索引（volatile防止编译器优化）

u16 adc_record[ADC_BUF_LENGTH] = {0};//adc缓冲区

/**
 * @description: adc数据写入函数，使用循环数组防止越界
 * @param {u16} value
 * @return {*}
 */
void adc_record_write(u16 value) {
    adc_record[write_index] = value;//当前写入
    write_index = (write_index + 1) % ADC_BUF_LENGTH;//下一个写入的索引
    
    // 更新有效数据数量（不超过缓冲区容量）
    if (data_count < ADC_BUF_LENGTH) {
        data_count++;
    }
}

/**
 * @description: adc数据读取函数，将配合曲线绘制使用
 * @param {u16} index 读取的索引  0代表最新写入的数据
 * @return {*}
 */
u16 adc_record_read(u16 index) {
    u16 read_pos;
    if (index >= data_count) {
		printf("warning--------input adc record index out of data");
        return 0; // 索引越界保护（实际可添加错误处理）
    }
    
    // 计算实际存储位置
    read_pos = (write_index - 1 - index + ADC_BUF_LENGTH) % ADC_BUF_LENGTH;
    return adc_record[read_pos];
}


/**
 * @description: adc数据调试函数，打印所有的adc数据
 * @return {*}
 */
void adc_record_print(void) {
    u16 i;
    printf("Buffer Content (Newest -> Oldest):\n");
    for (i = 0; i < data_count; i++) 
    {
        printf("[%2u] = %4u\n", i, adc_record_read(i));
    }
    printf("\n");
}


/**
 * @description: 返回已记录的数据量
 * @return {*}
 */
u16 adc_record_length(void) {
    return data_count;
}


/**
 * @description: adc值转温度
 * @param {u16} adc_value
 * @return {*}
 */
float adc_to_temperature(u16 adc_value) {
    const float voltage = adc_value * 100.0f / 4095.0f; // 假设参考电压3.3V
    return voltage;
}


/**
 * @description: 显示初始化，负责清屏和绘制背景--蓝色
 * @return {*}
 */
void display_init(void)
{
    // 1. 清屏和绘制背景
    GUI_SetBkColor(GUI_BLUE);
    GUI_Clear();
}


/**
 * @description: 创建一个矩形窗口，在里面绘制温度曲线，需要一直刷新
 * @return {*}
 */
void display_temp_line(LIMIT_TEM mylimit) {
    int i,x,y,limit_y;
    u16 prev_x = 0, prev_y = 0;
    float temp_value;
    
    //绘制标题
    GUI_SetColor(GUI_YELLOW);
    GUI_SetFont(&GUI_Font24_ASCII); 
    GUI_DispStringHCenterAt("Temperature Line", 400, 10);

    //绘制曲线区域背景
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(TEMP_AREA_LEFT, TEMP_AREA_TOP, TEMP_AREA_RIGHT, TEMP_AREA_BOTTOM);
    GUI_SetColor(GUI_BLACK);
    GUI_DrawRect(TEMP_AREA_LEFT,TEMP_AREA_TOP, TEMP_AREA_RIGHT, TEMP_AREA_BOTTOM);

    limit_y = TEMP_AREA_BOTTOM - (int)((adc_to_temperature(mylimit.value*4095.0f/100.0f)/MAX_TEMP_VALUE) * 
    (TEMP_AREA_BOTTOM-TEMP_AREA_TOP));
    limit_y = GUI_MIN(GUI_MAX(limit_y, TEMP_AREA_TOP), TEMP_AREA_BOTTOM);
    GUI_SetColor(GUI_BLACK);
    GUI_DrawLine(TEMP_AREA_LEFT, limit_y, TEMP_AREA_RIGHT, limit_y);

    //绘制温度曲线

    GUI_SetColor(GUI_RED);
    for (i = 0; i < adc_record_length(); i++) {
        //获取温度值（需要你实现ADC转温度函数）
        temp_value = adc_to_temperature(adc_record_read(i)); 

        //计算坐标
        x = TEMP_AREA_LEFT + i * (TEMP_AREA_RIGHT-TEMP_AREA_LEFT)/ADC_BUF_LENGTH;
        y = TEMP_AREA_BOTTOM - (int)((temp_value/MAX_TEMP_VALUE) * 
                                      (TEMP_AREA_BOTTOM-TEMP_AREA_TOP));
        //限制坐标范围
        y = GUI_MIN(GUI_MAX(y, TEMP_AREA_TOP), TEMP_AREA_BOTTOM);

        //绘制线段
        if (i > 0) {
            GUI_DrawLine(prev_x, prev_y, x, y);
        }
        prev_x = x;
        prev_y = y;
    }
}

/**
 * @description: 显示相关信息
 * @param {LIMIT_TEM} mylimit //关于温度限制的所有信息
 * @return {*}
 */
void display_information(LIMIT_TEM mylimit,int warning_state) {
    #define LABEL_COL   50
    #define VALUE_COL   200
    #define LINE_HEIGHT 24  // 确保此处定义存在
    
    u16 y_pos = 300;

    static char last_p_value = 'N';

    GUI_SetFont(&GUI_Font16_ASCII);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLUE);

    // 显示温度限值
    GUI_DispStringAt("Limit Value:", LABEL_COL, y_pos);
    GUI_DispDecAt(mylimit.decade, VALUE_COL, y_pos, 1);
    GUI_DispDecAt(mylimit.theuint, VALUE_COL + 12, y_pos, 1);
    GUI_DispStringAt("C", VALUE_COL + 24, y_pos);

	GUI_SetPenSize(3);				//设置画笔颜色,单位像素点
    if(mylimit.p_value == 'L')
    {
        GUI_DrawLine(VALUE_COL - 2,y_pos+18,VALUE_COL + 6,y_pos+18);   //绘制线条
    }
    else if(mylimit.p_value == 'R')
    {
        GUI_DrawLine(VALUE_COL + 10,y_pos+18,VALUE_COL + 18,y_pos+18);   //绘制线条
    }

    GUI_DispCharAt(mylimit.p_value,VALUE_COL + 48 ,y_pos);

    y_pos += LINE_HEIGHT; // 确保此行前无语法错误
    
    // 显示分解值
    GUI_DispStringAt("tempture now:", LABEL_COL, y_pos);

    //GUI_DispDecAt(adc_record_read(0), VALUE_COL, y_pos, 1);

    GUI_GotoXY(VALUE_COL,y_pos);
	GUI_DispFloat(adc_to_temperature(adc_record_read(0)),5);	

    if(warning_state)
    {
        GUI_SetColor(GUI_RED);
        GUI_FillRect(660,350,760,450);	//在当前窗口中指定位置绘制填充的矩形区域
    }
    else{
        GUI_SetColor(GUI_WHITE);
        GUI_FillRect(660,350,760,450);	//在当前窗口中指定位置绘制填充的矩形区域       
    }

    if(last_p_value != mylimit.p_value)
    {
        last_p_value = mylimit.p_value;
        GUI_Clear();
    }
}






